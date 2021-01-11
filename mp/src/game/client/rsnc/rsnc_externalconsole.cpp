//=============================================================================//
//
//  Resonance Team
//  External Console
//  Launch game with -ext_con
//
//=============================================================================//
#include "cbase.h"

#ifdef _WIN32

#include "filesystem.h"
#include "icommandline.h"

// Valve redefines a lot of winapi
#undef GetCommandLine
#undef INVALID_HANDLE_VALUE
#undef ReadConsoleInput
#undef RegCreateKey
#undef RegCreateKeyEx
#undef RegOpenKey
#undef RegOpenKeyEx
#undef RegQueryValue
#undef RegQueryValueEx
#undef RegSetValue
#undef RegSetValueEx

#include <Windows.h>
// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

#define OUT_PIPE_NAME "\\\\.\\pipe\\ResonanceExternalConsoleSTDOUT"
#define IN_PIPE_NAME  "\\\\.\\pipe\\ResonanceExternalConsoleSTDIN"

static bool s_externalConsoleConnected = false;
static HANDLE s_inPipe;
static HANDLE s_outPipe;

// Wrapper for strings so we don't lose data
struct packString_t
{
    packString_t() {}
    packString_t(const char* str)
    {
        len = strlen(str);
        this->str = strcpy(new char[len + 1], str);
    }
    ~packString_t()
    {
        if(str)
            delete[] str;
    }

    const char* str = 0;
    size_t len = 0;
};

// Write strings to the out pipe
unsigned long WriteString(const char* str, size_t length)
{
    unsigned long written;
    WriteFile(s_outPipe, &length, sizeof(length), &written, NULL);
    WriteFile(s_outPipe, str, length, &written, NULL);
    return written;
}
unsigned long WriteString(const char* str)
{
    size_t size = strlen(str);
    return WriteString(str, size);
}
unsigned long WriteString(packString_t* str)
{
    return WriteString(str->str, str->len);
}

// REQUIRED Base packet to inherit from
// Every packet must begin with a 5 character header!
struct extConPacket_t
{
    virtual void Write() = 0;
};


static CThreadFastMutex			   s_backQueueMutex;
static CUtlVector<extConPacket_t*> s_backQueue;
static ThreadHandle_t s_dispatchThread;

unsigned int DispatchPacketThread(void*)
{
    // We copy data out of the back queue instead of working on it to keep our mutex time low
    CUtlVector<extConPacket_t*> workingQueue;
    while (s_externalConsoleConnected)
    {
        if (s_backQueue.Count() > 0)
        {

            s_backQueueMutex.Lock();
            for (int i = 0; i < s_backQueue.Count(); i++)
                workingQueue.AddToTail(s_backQueue.Element(i));
            s_backQueue.Purge();
            s_backQueueMutex.Unlock();
        }
        while (workingQueue.Count() > 0)
        {

            extConPacket_t* pack = workingQueue.Head();
            pack->Write();
            delete pack;
            workingQueue.Remove(0);
        }
        Sleep(100);
    }
    ThreadJoin(s_dispatchThread);
    return 0;
}

// We need to make sure we don't spend much time in our spew so we queue up packets to be sent later
void QueuePacket(extConPacket_t* packet)
{
    s_backQueueMutex.Lock();
    s_backQueue.AddToTail(packet);
    s_backQueueMutex.Unlock();
}

// To be sent on game connection
static const char s_connectSignature[] = "\xFF" "CON\xFF";
static size_t s_lenConnectSignature = sizeof(s_connectSignature) - 1; // -1 to cut the 0
struct extConConnect_t : public extConPacket_t
{
    const char* gameName;
    const char* gamePath;
    CUtlVector<packString_t*> mapList;

    ~extConConnect_t()
    {
        for(int i = 0; i < mapList.Count(); i++)
            delete mapList.Element(i);
    }
    virtual void Write()
    {
        DWORD written;
        WriteFile(s_outPipe, s_connectSignature, s_lenConnectSignature, &written, NULL);
        WriteString(gameName);
        WriteString(gamePath);
        int count = mapList.Count();
        WriteFile(s_outPipe, &count, sizeof(count), &written, NULL);
        for (int i = 0; i < mapList.Count(); i++)
        {
            WriteString(mapList.Element(i));
        }
    }
};

// To be sent on spew message
static const char s_messageSignature[] = "\xFFMSG\xFF";
static size_t s_lenMessageSignature = sizeof(s_messageSignature) - 1; // -1 to cut the 0
struct extConMessage_t : public extConPacket_t
{
    unsigned char spewType;
    packString_t* pMsg;

    extConMessage_t(SpewType_t _spewType, const tchar* _pMsg)
    {
        spewType = _spewType;
        pMsg = new packString_t((const char*)_pMsg);
    }
    ~extConMessage_t()
    {
        delete pMsg;
    }
    virtual void Write()
    {
        DWORD written;
        WriteFile(s_outPipe, s_messageSignature, s_lenMessageSignature, &written, NULL);
        WriteFile(s_outPipe, &spewType, sizeof(spewType), &written, NULL);
        WriteString(pMsg);
    }
};

// To be sent on level load
static const char s_loadLevelSignature[] = "\xFF" "LDL\xFF";
static size_t s_lenLoadLevelSignature = sizeof(s_loadLevelSignature) - 1; // -1 to cut the 0
struct extConLoadLevel_t : public extConPacket_t
{
    packString_t* levelName;

    extConLoadLevel_t(const char* level)
    {
        levelName = new packString_t(level);
    }
    ~extConLoadLevel_t()
    {
        delete levelName;
    }
    virtual void Write()
    {
        DWORD written;
        WriteFile(s_outPipe, s_loadLevelSignature, s_lenLoadLevelSignature, &written, NULL);
        WriteString(levelName);
    }
};

// Queue up the spew to be sent off later
SpewRetval_t SpewToPipe(SpewType_t spewType, const tchar* pMsg)
{
    QueuePacket(new extConMessage_t(spewType, pMsg));
   
    return SPEW_CONTINUE;
}

// External console in is always just commands
static ThreadHandle_t s_inThread;
unsigned ReadExtConIn(void* pParam)
{
    while (s_externalConsoleConnected)
    {
        DWORD numRead = 0;

        // Read for the header
        char header[5];
        bool result = ReadFile(
            s_inPipe,
            header, // the data from the pipe will be put here
            sizeof(header),
            &numRead, // this will store number of bytes actually read
            NULL // not using overlapped IO
        );

        if (result && numRead == 5 && strncmp("\x7F""CMD""\x7F", header, 5) == 0)
        {
            // Read for string size
            size_t length = 0;
            result = ReadFile(
                s_inPipe,
                &length, // the data from the pipe will be put here
                sizeof(length), // number of bytes allocated
                &numRead, // this will store number of bytes actually read
                NULL // not using overlapped IO
            );

            if (result && numRead == sizeof(length))
            {

                // Read for command
                char* str = new char[length + 1];
                result = ReadFile(
                    s_inPipe,
                    str, // the data from the pipe will be put here
                    length, // number of bytes allocated
                    &numRead, // this will store number of bytes actually read
                    NULL // not using overlapped IO
                );
                if (numRead == length)
                {
                    str[numRead] = '\0'; // null terminate the string
                    engine->ClientCmd(str);
                }
                delete[] str;
            }
        }
    }
    ThreadJoin(s_inThread);
    return 0;
}

class CExternalConsole : public CAutoGameSystem
{
public:

	virtual char const* Name() { return "ExternalConsole"; }

	// Init, shutdown
	// return true on success. false to abort DLL init!
	virtual bool Init() 
	{
		if (CommandLine()->CheckParm("-ext_con"))
		{
            s_outPipe = CreateFile(
                TEXT(OUT_PIPE_NAME),
                GENERIC_WRITE,
                FILE_SHARE_WRITE,
                NULL,
                OPEN_EXISTING,
                FILE_ATTRIBUTE_NORMAL,
                NULL
            );
            s_inPipe = CreateFile(
                TEXT(IN_PIPE_NAME),
                GENERIC_READ,
                FILE_SHARE_READ,
                NULL,
                OPEN_EXISTING,
                FILE_ATTRIBUTE_NORMAL,
                NULL
            );
            if (s_outPipe == INVALID_HANDLE_VALUE || s_inPipe == INVALID_HANDLE_VALUE) {
                Error("Failed to connect to external console!");
                // look up error code here using GetLastError()
                return false;
            }

            s_externalConsoleConnected = true;
			
            // Send connection info
            {
                extConConnect_t* connect = new extConConnect_t;
                connect->gameName = engine->GetGameDirectory();
                connect->gamePath = engine->GetGameDirectory();

                FileFindHandle_t findH;
                char const* mapName = filesystem->FindFirst("maps/*.bsp", &findH);
                while (mapName)
                {
                    connect->mapList.AddToTail(new packString_t(mapName));
                    mapName = filesystem->FindNext(findH);
                }

                QueuePacket(connect);
            }
            
            // Start up the in/out threads
            s_inThread = CreateSimpleThread(ReadExtConIn, nullptr);
            s_dispatchThread = CreateSimpleThread(DispatchPacketThread, nullptr);

            // Bind the spew func
            SpewOutputFunc(SpewToPipe);

		}
        return true;
	}
	virtual void Shutdown() 
    {
        if (s_externalConsoleConnected)
        {
            s_externalConsoleConnected = false;
            if (s_outPipe != INVALID_HANDLE_VALUE)
            {
                CloseHandle(s_outPipe);
            }
            if (s_inPipe != INVALID_HANDLE_VALUE)
            {
                CloseHandle(s_inPipe);
            }
        }
    }


    virtual void LevelInitPreEntity() 
    {
        QueuePacket(new extConLoadLevel_t(engine->GetLevelName()));
    }
    virtual void LevelShutdownPreEntity()
    {
        QueuePacket(new extConLoadLevel_t(""));
    }
};

static CExternalConsole g_ExternalConsole;

#else
#error EXTERNAL_CONSOLE_UNSUPPORTED
#endif