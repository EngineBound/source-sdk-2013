
#include <windows.h>

#include "checksum_crc.h"
#include "checksum_md5.h"

static HINSTANCE g_hinstDLL;

extern "C"
{
	void __declspec(dllexport) _ftol3(char* input);

	BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD, LPVOID);
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD, LPVOID)
{

	g_hinstDLL = hinstDLL;

	return TRUE;  // Successful DLL_PROCESS_ATTACH.
}

class CCRCBypass
{
public:
	CCRCBypass() { m_pBypass1Input = 0; }

	virtual CRC32_t BypassCRC(unsigned char* input);
	virtual void BypassMD5();

	virtual void BypassEmpty1() {}
	virtual void BypassEmpty2() {}
	virtual CRC32_t BypassEmpty3() { return 0x7ea7; }
private:
	unsigned char *m_pBypass1Input;
};

static CCRCBypass g_CRCBypass;

CRC32_t CCRCBypass::BypassCRC(unsigned char* input)
{
	input += 0x2b;
	m_pBypass1Input = input;

	CRC32_t crc;
	CRC32_Init(&crc);

	CRC32_ProcessBuffer(&crc, input, 0x1005);
	CRC32_ProcessBuffer(&crc, &g_hinstDLL, 4);

	const void *pCRCBypass = &g_CRCBypass;
	CRC32_ProcessBuffer(&crc, &pCRCBypass, 4);

	CRC32_Final(&crc);

	return crc;
}

void CCRCBypass::BypassMD5()
{
	MD5Context_t md5Context;

	MD5Init(&md5Context);
	MD5Update(&md5Context, m_pBypass1Input + 0x2b, 0xfda);
	MD5Final(m_pBypass1Input, &md5Context);
}

void __declspec(dllexport) _ftol3(char *input)
{
	// input is a pointer to a pointer of the crc bypasser - 0x2b
	*(char **)(input + 0x2b) = (char*)&g_CRCBypass;
}