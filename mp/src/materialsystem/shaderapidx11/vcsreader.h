#ifndef VCSREADER_H
#define VCSREADER_H

#ifdef _WIN32
#pragma once
#endif

#include "filesystem.h"
#include "materialsystem/shader_vcs_version.h"


// Holds methods for handling VCS files
class CVCSReader
{
public:

	CVCSReader();
	~CVCSReader();

	bool InitReader(const char *pFileName, bool bIsVertexShader);
	void CreateShadersForStaticComboIfNeeded(int nStaticIndex);

private:

	// FREE AFTER USE
	unsigned char *GetComboChunk(int nStaticIndex, uint32 &nChunkSize); 
	uint32 GetOffsetForStaticCombo(int nStaticIndex);

private:

	bool m_bIsVertexShader;

	char m_pFileLocation[MAX_PATH];
	ShaderHeader_t m_ShaderHeader;

	CUtlRBTree<StaticComboRecord_t> *m_pStaticComboRecords;

	int m_nNumDuplicateStaticRecords;
	CUtlRBTree<StaticComboAliasRecord_t> *m_pDuplicateStaticComboRecords;

	char **m_ppShaderComboByteCode; // TODO: Might not be the best way to do this, investigate later

};

#endif