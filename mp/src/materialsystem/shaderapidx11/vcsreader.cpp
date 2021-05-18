
#include "vcsreader.h"

#include "lzmaDecoder.h"

#include "dx11global.h"
#include "shaderdevicedx11.h"

#include "memdbgon.h"

// VCS FILE:
// A VCS file contains:
// HEADER:
// 28 bytes - assume for now we are in shader version 6 as that is what is compiling
// This stores:
//	Version
//	Number of combos total
//	Number of dynamic combos
//	Shader flags
//	Centroid mask??? what is this (maybe something to do with centroid hack)
//	Number of static combos - 8 bytes each after header
//  CRC32 of shader
// Then
// Static combos - 4 bytes ID, 4 bytes offset of shader? * m_nNumStaticCombos
// (V6) 4 Bytes - num duplicate records
// (V6) Static combo duplicates- 4 bytes id, 4 bytes id of 'master' combo * num duplicates
// There are static * dynamic different shader byte code instances in the shader.
// Each static index is m_nDynamicCombos different to the last, 2d array type thing
// From here, we have offsets of combos
// Using these, we can get a 'chunk' of data by finding the chunk between one offset and the next (allocoptimalreadbuffer)
//
// We then check if the shader is compressed by the following:
//	Get next uint32 from the block
//	If it's 0xFFFFFFFF we're done
//  Check the 2 most significant bits
//  if 0x8, Copy the block as is, uncompressed. Block size is ident uint32 & 0x3FFFFFFF
//  if 0x4, LZMA decode the block. Block size is ident uint32 & 0x3FFFFFFF
//  if 0x0, BZIP2 decode the block. Block size is given by BZ2 decompression
//  otherwise shader is corrupt probably
//
//	Now we have a decompressed block of max size MAX_SHADER_UNPACKED_BLOCK_SIZE
//  the first 2 uint32 are comboID and shader bytecode size respectively.
//  After these shifts, we have the shader itself and can pass the decompressed block to g_pShaderDeviceDX11 with shader bytecode size

#define SHADER_FILE_EXTENSION ".vcs"

bool StaticComboRecordLessFunc(const StaticComboRecord_t &a, const StaticComboRecord_t &b)
{
	return a.m_nStaticComboID < b.m_nStaticComboID;
}

bool StaticComboAliasLessFunc(const StaticComboAliasRecord_t &a, const StaticComboAliasRecord_t &b)
{
	return a.m_nStaticComboID < b.m_nStaticComboID;
}

CVCSReader::CVCSReader()
{
	V_memset(&m_ShaderHeader, 0, sizeof(m_ShaderHeader));
	V_memset(&m_pFileLocation, 0, sizeof(m_pFileLocation));

	m_pStaticComboRecords = new CUtlRBTree<StaticComboRecord_t>(StaticComboRecordLessFunc);
	m_pDuplicateStaticComboRecords = new CUtlRBTree<StaticComboAliasRecord_t>(StaticComboAliasLessFunc);

	m_ppShaderComboHandles = NULL;
	m_bIsVertexShader = false;
}

CVCSReader::~CVCSReader()
{
	if (m_ppShaderComboHandles)
	{
		for (unsigned int i = 0; i < m_ShaderHeader.m_nNumStaticCombos; ++i)
		{
			if (m_ppShaderComboHandles[i])
				free(m_ppShaderComboHandles[i]);
		}
		free(m_ppShaderComboHandles);
	}

	if (m_pStaticComboRecords)
	{
		m_pStaticComboRecords->RemoveAll();
		delete m_pStaticComboRecords;
	}

	if (m_pDuplicateStaticComboRecords)
	{
		m_pDuplicateStaticComboRecords->RemoveAll();
		delete m_pDuplicateStaticComboRecords;
	}

	m_ppShaderComboHandles = NULL;
}

bool CVCSReader::InitReader(const char *pFileName, bool bIsVertexShader)
{
	// TODO: Version < 6 handling

	m_bIsVertexShader = bIsVertexShader;

	// Open the file
	char pProcessedFile[MAX_PATH] = { 0 };
	V_memcpy(pProcessedFile, pFileName, strlen(pFileName));
	V_SetExtension(pProcessedFile, SHADER_FILE_EXTENSION, sizeof(pProcessedFile));

	FileHandle_t hVCSFile = g_pFullFileSystem->Open(pProcessedFile, "rb", NULL); // TODO: Preferred path

	if (hVCSFile == FILESYSTEM_INVALID_HANDLE)
	{
		char pFileTmp[MAX_PATH] = { 0 };
		V_snprintf(pFileTmp, MAX_PATH, "shaders\\fxc\\%s", pProcessedFile);

		hVCSFile = g_pFullFileSystem->Open(pFileTmp, "rb", NULL); // TODO: Preferred path
		if (hVCSFile == FILESYSTEM_INVALID_HANDLE)
		{
			Warning(__FUNCTION__ ": Unable to load shader %s!\n", pFileName);
			return false;
		}

		V_memcpy(m_pFileLocation, pFileTmp, strlen(pFileTmp));
	}
	else
	{
		V_memcpy(m_pFileLocation, pProcessedFile, strlen(pProcessedFile));
	}

	// Read the header
	g_pFullFileSystem->Read(&m_ShaderHeader, sizeof(m_ShaderHeader), hVCSFile);

	// Create byte code array
	m_ppShaderComboHandles = (ShaderHandle_t **)calloc(m_ShaderHeader.m_nNumStaticCombos, sizeof(ShaderHandle_t *));

	Assert(m_ShaderHeader.m_nVersion == 6); // For now

	// Read in the static combos - last one (id FFFFFFFF) gives file size
	m_pStaticComboRecords->EnsureCapacity(m_ShaderHeader.m_nNumStaticCombos);

	for (unsigned int i = 0; i < m_ShaderHeader.m_nNumStaticCombos; ++i)
	{
		StaticComboRecord_t readCombo;
		V_memset(&readCombo, 0, sizeof(readCombo));

		g_pFullFileSystem->Read(&readCombo, sizeof(readCombo), hVCSFile);

		m_pStaticComboRecords->Insert(readCombo);
	}

	// Read in the duplicate records
	g_pFullFileSystem->Read(&m_nNumDuplicateStaticRecords, sizeof(m_nNumDuplicateStaticRecords), hVCSFile);

	m_pDuplicateStaticComboRecords->EnsureCapacity(m_nNumDuplicateStaticRecords);
	for (int i = 0; i < m_nNumDuplicateStaticRecords; ++i)
	{
		StaticComboAliasRecord_t readCombo;
		V_memset(&readCombo, 0, sizeof(readCombo));

		g_pFullFileSystem->Read(&readCombo, sizeof(readCombo), hVCSFile);

		m_pDuplicateStaticComboRecords->Insert(readCombo);
	}

	g_pFullFileSystem->Close(hVCSFile);

	return true;
}

uint32 CVCSReader::ResolveAliasCombo(uint32 aliasID)
{
	// There is definitely a better way to do this but we'll use this for now
	StaticComboRecord_t lookup;
	lookup.m_nStaticComboID = aliasID;

	unsigned short nInd = m_pStaticComboRecords->Find(lookup);
	if (m_pStaticComboRecords->IsValidIndex(nInd))
		return m_pStaticComboRecords->Element(nInd).m_nStaticComboID;

	StaticComboAliasRecord_t aliasLookup;
	aliasLookup.m_nStaticComboID = aliasID;

	nInd = m_pDuplicateStaticComboRecords->Find(aliasLookup);
	if (m_pDuplicateStaticComboRecords->IsValidIndex(nInd))
		return ResolveAliasCombo(m_pDuplicateStaticComboRecords->Element(nInd).m_nStaticComboID);

	// EOF combo
	return 0xFFFFFFFF;
}

static uint32 Next32(unsigned char * &pBuf)
{
	uint32 nOut;
	V_memcpy(&nOut, pBuf, sizeof(nOut));
	pBuf += sizeof(nOut);
	return nOut;
}

void CVCSReader::CreateShadersForStaticComboIfNeeded(int nStaticIndex)
{
	uint32 nActualStaticIndex = ResolveAliasCombo(nStaticIndex / m_ShaderHeader.m_nDynamicCombos);

	if (m_ppShaderComboHandles[nActualStaticIndex])
		return;

	m_ppShaderComboHandles[nActualStaticIndex] = (ShaderHandle_t *)calloc(m_ShaderHeader.m_nDynamicCombos, sizeof(ShaderHandle_t));

	uint32 nChunkSize = 0;
	unsigned char *pChunk = GetComboChunk(nActualStaticIndex, nChunkSize);
	unsigned char *pChunkRead = pChunk; // Ensure pChunk is not changed
	unsigned char pUncompressedChunk[MAX_SHADER_UNPACKED_BLOCK_SIZE];

	bool bIsValid = true;
	for (int nDynamicIndex = 0;bIsValid && nDynamicIndex < m_ShaderHeader.m_nDynamicCombos;nDynamicIndex++)
	{
		uint32 nBlockHeader = Next32(pChunkRead);
		if (nBlockHeader == 0xFFFFFFFF)
			break; // Reached end of chunk

		uint32 nBlockSize = nBlockHeader & 0x3FFFFFFF;
		switch (nBlockHeader & 0xc0000000)
		{
			case 0x80000000: // Uncompressed
			{
				V_memcpy(pUncompressedChunk, pChunkRead, nBlockSize);
				pChunkRead += nBlockSize;

			}
			break;

			case 0x40000000: // LZMA
			{
				size_t nUncompressedSize = CLZMA::Uncompress(pChunkRead, pUncompressedChunk);
				pChunkRead += nBlockSize;
				nBlockSize = nUncompressedSize;

			}
			break;

			case 0x00000000: // TODO: BZIP2 - Unsupported currently
			default:
			{
				Assert(0);
				Error("Invalid shader compression type in %s", m_pFileLocation);
				bIsValid = false;
			}
			break;
		}

		unsigned char *pLocation = pUncompressedChunk;
		while (pLocation < pUncompressedChunk + nBlockSize)
		{
			/*uint32 nComboID = */Next32(pLocation);
			uint32 nByteCodeSize = Next32(pLocation);

			if (*(pLocation + 77) != 0x5)
			{
				free(pChunk);
				free(m_ppShaderComboHandles[nActualStaticIndex]);

				Warning("Incorrect shader model supplied to game when sm50 required!\n");
				return;
			}

			ShaderHandle_t shader;

			if (m_bIsVertexShader)
				shader = g_pShaderDeviceDX11->CreateVertexShader((char *)pLocation, (size_t)nByteCodeSize, "vs_5_0"); // TODO: Other shader versions?
			else
				shader = g_pShaderDeviceDX11->CreatePixelShader((char *)pLocation, (size_t)nByteCodeSize, "ps_5_0");

			Assert(shader != 0); // Invalid
			m_ppShaderComboHandles[nActualStaticIndex][nDynamicIndex] = shader;

			pLocation += nByteCodeSize;
		}
	}

	free(pChunk);
}

uint32 CVCSReader::GetOffsetForStaticCombo(int nStaticIndex)
{
	StaticComboRecord_t lookupRecord;
	lookupRecord.m_nStaticComboID = nStaticIndex; //ResolveAliasCombo(nStaticIndex); - This is ensured now

	unsigned short nStaticRecordIndex = m_pStaticComboRecords->Find(lookupRecord);
	Assert(m_pStaticComboRecords->IsValidIndex(nStaticRecordIndex));

	return m_pStaticComboRecords->Element(nStaticRecordIndex).m_nFileOffset;
}

unsigned char *CVCSReader::GetComboChunk(int nStaticIndex, uint32 &nChunkSize)
{
	uint32 nFileOffset = GetOffsetForStaticCombo(nStaticIndex);

	FileHandle_t hVCSFile = g_pFullFileSystem->Open(m_pFileLocation, "rb", NULL); // TODO: Preferred path
	g_pFullFileSystem->Seek(hVCSFile, nFileOffset, FILESYSTEM_SEEK_HEAD);

	nChunkSize = GetOffsetForStaticCombo(ResolveAliasCombo(nStaticIndex + 1)) - nFileOffset;

	unsigned char *pOutBuf = (unsigned char *)malloc(nChunkSize);
	g_pFullFileSystem->Read(pOutBuf, nChunkSize, hVCSFile);

	g_pFullFileSystem->Close(hVCSFile);

	return pOutBuf;
}