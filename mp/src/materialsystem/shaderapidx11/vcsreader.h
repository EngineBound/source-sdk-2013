#ifndef VCSREADER_H
#define VCSREADER_H

#ifdef _WIN32
#pragma once
#endif

#include "filesystem.h"
#include "materialsystem/shader_vcs_version.h"

#include "dx11global.h"

// Holds methods for handling VCS files
class CVCSReader
{
public:

	CVCSReader();
	~CVCSReader();

	bool InitReader(const char *pFileName, bool bIsVertexShader);
	void CreateShadersForStaticComboIfNeeded(int nStaticIndex);

	inline StaticShaderHandle_t GetStaticShader(int nStaticIndex)
	{
		if (!m_ppShaderComboHandles)
			return 0;

		return m_ppShaderComboHandles[ResolveAliasCombo(nStaticIndex / m_ShaderHeader.m_nDynamicCombos)];
	}

	static inline ShaderHandle_t GetShader(StaticShaderHandle_t hStaticShader, int nDynamicIndex)
	{
		return hStaticShader[nDynamicIndex];
	}

private:

	uint32 ResolveAliasCombo(uint32 aliasID);

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

	ShaderHandle_t **m_ppShaderComboHandles; // TODO: Might not be the best way to do this, investigate later

};

#endif