#ifndef MESHDX11_H
#define MESHDX11_H

#ifdef _WIN32
#pragma once
#endif

#include "imeshdx11.h"
#include "ishaderdevice.h"

struct ID3D11Buffer;

inline void ComputeVertexDesc(unsigned char *pBuffer, VertexFormat_t vertexFormat, VertexDesc_t &desc)
{
	// Static vertex data so meshbuilder can write to nothing
	static ALIGN32 ModelVertexDX8_t dummyVerts[4];
	float *dummyData = (float *)&dummyVerts;

	int *pVertexSizePtrs[64];
	int nVertexSizePtrs = 0;

	VertexCompressionType_t compressionType = CompressionType(vertexFormat);
	desc.m_CompressionType = compressionType;

	int offset = 0;

	if (vertexFormat & VERTEX_POSITION)
	{
		pVertexSizePtrs[nVertexSizePtrs++] = &desc.m_VertexSize_Position;
		desc.m_pPosition = reinterpret_cast<float *>(pBuffer + offset);
		offset += GetVertexElementSize(VERTEX_ELEMENT_POSITION, compressionType);

		if (vertexFormat & VERTEX_WRINKLE)
		{
			pVertexSizePtrs[nVertexSizePtrs++] = &desc.m_VertexSize_Wrinkle;
			desc.m_pWrinkle = reinterpret_cast<float *>(pBuffer + offset);
			offset += GetVertexElementSize(VERTEX_ELEMENT_WRINKLE, compressionType);
		}
		else
		{
			desc.m_VertexSize_Wrinkle = 0;
			desc.m_pWrinkle = dummyData;
		}
	}
	else
	{
		desc.m_VertexSize_Position = 0;
		desc.m_pPosition = dummyData;

		desc.m_VertexSize_Wrinkle = 0;
		desc.m_pWrinkle = dummyData;
	}

	desc.m_NumBoneWeights = NumBoneWeights(vertexFormat);

	// Only allow 0 or 2, with index flag set respectively
	Assert(((desc.m_NumBoneWeights == 2) && ((vertexFormat & VERTEX_BONE_INDEX) != 0)) ||
		((desc.m_NumBoneWeights == 0) && ((vertexFormat & VERTEX_BONE_INDEX) == 0)));

	if (vertexFormat & VERTEX_BONE_INDEX)
	{
		if (desc.m_NumBoneWeights > 0)
		{
			pVertexSizePtrs[nVertexSizePtrs++] = &desc.m_VertexSize_BoneWeight;
			desc.m_pBoneWeight = reinterpret_cast<float *>(pBuffer + offset);
			offset += GetVertexElementSize(VERTEX_ELEMENT_BONEWEIGHTS2, compressionType);
		}
		else
		{
			desc.m_VertexSize_BoneWeight = 0;
			desc.m_pBoneWeight = dummyData;
		}

		pVertexSizePtrs[nVertexSizePtrs++] = &desc.m_VertexSize_BoneMatrixIndex;
		desc.m_pBoneMatrixIndex = (pBuffer + offset);
		offset += GetVertexElementSize(VERTEX_ELEMENT_BONEINDEX, compressionType);
	}
	else
	{
		desc.m_VertexSize_BoneWeight = 0;
		desc.m_pBoneWeight = dummyData;

		desc.m_VertexSize_BoneMatrixIndex = 0;
		desc.m_pBoneMatrixIndex = (unsigned char *)dummyData;
	}

	if (vertexFormat & VERTEX_NORMAL)
	{
		pVertexSizePtrs[nVertexSizePtrs++] = &desc.m_VertexSize_Normal;
		desc.m_pNormal = reinterpret_cast<float *>(pBuffer + offset);
		offset += GetVertexElementSize(VERTEX_ELEMENT_NORMAL, compressionType);
	}
	else
	{
		desc.m_VertexSize_Normal = 0;
		desc.m_pNormal = dummyData;
	}

	if (vertexFormat & VERTEX_COLOR)
	{
		pVertexSizePtrs[nVertexSizePtrs++] = &desc.m_VertexSize_Color;
		desc.m_pColor = (pBuffer + offset);
		offset += GetVertexElementSize(VERTEX_ELEMENT_COLOR, compressionType);
	}
	else
	{
		desc.m_VertexSize_Color = 0;
		desc.m_pColor = (unsigned char *)dummyData;
	}

	if (vertexFormat & VERTEX_SPECULAR)
	{
		pVertexSizePtrs[nVertexSizePtrs++] = &desc.m_VertexSize_Specular;
		desc.m_pSpecular = (pBuffer + offset);
		offset += GetVertexElementSize(VERTEX_ELEMENT_SPECULAR, compressionType);
	}
	else
	{
		desc.m_VertexSize_Specular = 0;
		desc.m_pSpecular = (unsigned char *)dummyData;
	}

	for (int i = 0; i < VERTEX_MAX_TEXTURE_COORDINATES; ++i)
	{
		int coordSize = TexCoordSize(i, vertexFormat);

		if (coordSize != 0)
		{
			pVertexSizePtrs[nVertexSizePtrs++] = &desc.m_VertexSize_TexCoord[i];
			desc.m_pTexCoord[i] = reinterpret_cast<float*>(pBuffer + offset);

			// VERTEX_ELEMENT_TEXCOORD{coordSize}D_{i}
			offset += GetVertexElementSize((VertexElement_t)(VERTEX_ELEMENT_TEXCOORD2D_0 +
				(coordSize - 1) * VERTEX_MAX_TEXTURE_COORDINATES + i), compressionType
			);
		}
		else
		{
			desc.m_pTexCoord[i] = dummyData;
			desc.m_VertexSize_TexCoord[i] = 0;
		}
	}

	if (vertexFormat & VERTEX_TANGENT_S)
	{
		pVertexSizePtrs[nVertexSizePtrs++] = &desc.m_VertexSize_TangentS;
		desc.m_pTangentS = reinterpret_cast<float*>(pBuffer + offset);
		offset += GetVertexElementSize(VERTEX_ELEMENT_TANGENT_S, compressionType);
	}
	else
	{
		desc.m_VertexSize_TangentS = 0;
		desc.m_pTangentS = dummyData;
	}

	if (vertexFormat & VERTEX_TANGENT_T)
	{
		pVertexSizePtrs[nVertexSizePtrs++] = &desc.m_VertexSize_TangentT;
		desc.m_pTangentT = reinterpret_cast<float*>(pBuffer + offset);
		offset += GetVertexElementSize(VERTEX_ELEMENT_TANGENT_T, compressionType);
	}
	else
	{
		desc.m_VertexSize_TangentT = 0;
		desc.m_pTangentT = dummyData;
	}

	int userDataSize = UserDataSize(vertexFormat);
	if (userDataSize > 0)
	{
		pVertexSizePtrs[nVertexSizePtrs++] = &desc.m_VertexSize_UserData;
		desc.m_pUserData = reinterpret_cast<float*>(pBuffer + offset);
		offset += GetVertexElementSize((VertexElement_t)(VERTEX_ELEMENT_USERDATA1 + (userDataSize - 1)), compressionType);
	}
	else
	{
		desc.m_VertexSize_UserData = 0;
		desc.m_pUserData = dummyData;
	}

	// Round up to next 16
	if ((vertexFormat & VERTEX_FORMAT_USE_EXACT_FORMAT) == 0 && offset > 16)
		offset = (offset + 0xF) & (~0xF);

	desc.m_ActualVertexSize = offset;

	for (int i = 0; i < nVertexSizePtrs; ++i)
	{
		*pVertexSizePtrs[i] = offset;
	}
}

inline int SizeForIndex(MaterialIndexFormat_t indexFormat) {
	switch (indexFormat)
	{
	default:
	case MATERIAL_INDEX_FORMAT_UNKNOWN:
		return 0;
	case MATERIAL_INDEX_FORMAT_16BIT:
		return 2;
	case MATERIAL_INDEX_FORMAT_32BIT:
		return 4;
	}
}

class CVertexBufferDX11 : public IVertexBufferDX11
{
public:
	CVertexBufferDX11(ShaderBufferType_t type, VertexFormat_t fmt, int nVertexCount, const char *pBudgetGroup);
	~CVertexBufferDX11();

	virtual bool CreateBuffer();
	virtual void DestroyBuffer();

public:
	// NOTE: The following two methods are only valid for static vertex buffers
	// Returns the number of vertices and the format of the vertex buffer
	virtual int VertexCount() const;
	virtual VertexFormat_t GetVertexFormat() const;

	// Is this vertex buffer dynamic?
	virtual bool IsDynamic() const;

	// NOTE: For dynamic vertex buffers only!
	// Casts the memory of the dynamic vertex buffer to the appropriate type
	virtual void BeginCastBuffer(VertexFormat_t format);
	virtual void EndCastBuffer();

	// Returns the number of vertices that can still be written into the buffer
	virtual int GetRoomRemaining() const;

	virtual bool Lock(int nVertexCount, bool bAppend, VertexDesc_t &desc);
	virtual void Unlock(int nVertexCount, VertexDesc_t &desc);

	// Spews the mesh data
	virtual void Spew(int nVertexCount, const VertexDesc_t &desc);

	// Call this in debug mode to make sure our data is good.
	virtual void ValidateData(int nVertexCount, const VertexDesc_t & desc);
public:
	inline ID3D11Buffer* GetBuffer()
	{
		return m_pD3DBuffer;
	}

	inline UINT GetBufferSize()
	{
		return m_nBufferSize;
	}
	
	inline UINT VertexSize()
	{
		return m_nVertexSize;
	}

	inline void SetVertexFormat(VertexFormat_t fmt)
	{
		if (m_bIsDynamic)
			m_VertexFormat = fmt;
	}

private:

	bool m_bIsDynamic;
	int m_nVertexCount;
	VertexFormat_t m_VertexFormat;

	int m_nVertexSize;
	size_t m_nBufferSize;
	size_t m_nBufferPosition;

	bool m_bIsLocked;

	unsigned char* m_pVertBuffer;

	ID3D11Buffer *m_pD3DBuffer;
};

class CIndexBufferDX11 : public IIndexBufferDX11
{
public:
	CIndexBufferDX11(ShaderBufferType_t type, MaterialIndexFormat_t fmt, int nIndexCount, const char *pBudgetGroup);
	~CIndexBufferDX11();

	virtual bool CreateBuffer();
	virtual void DestroyBuffer();

public:
	// NOTE: The following two methods are only valid for static index buffers
	// Returns the number of indices and the format of the index buffer
	virtual int IndexCount() const;
	virtual MaterialIndexFormat_t IndexFormat() const;

	// Is this index buffer dynamic?
	virtual bool IsDynamic() const;

	// NOTE: For dynamic index buffers only!
	// Casts the memory of the dynamic index buffer to the appropriate type
	virtual void BeginCastBuffer(MaterialIndexFormat_t format);
	virtual void EndCastBuffer();

	// Returns the number of indices that can still be written into the buffer
	virtual int GetRoomRemaining() const;

	// Locks, unlocks the index buffer
	virtual bool Lock(int nMaxIndexCount, bool bAppend, IndexDesc_t &desc);
	virtual void Unlock(int nWrittenIndexCount, IndexDesc_t &desc);

	// FIXME: Remove this!!
	// Locks, unlocks the index buffer for modify
	virtual void ModifyBegin(bool bReadOnly, int nFirstIndex, int nIndexCount, IndexDesc_t& desc);
	virtual void ModifyEnd(IndexDesc_t& desc);

	// Spews the mesh data
	virtual void Spew(int nIndexCount, const IndexDesc_t &desc);

	// Ensures the data in the index buffer is valid
	virtual void ValidateData(int nIndexCount, const IndexDesc_t &desc);
public:
	inline ID3D11Buffer* GetBuffer()
	{
		return m_pD3DBuffer;
	}

	inline UINT GetBufferSize()
	{
		return m_nBufferSize;
	}

	inline UINT IndexSize()
	{
		return m_nIndexSize;
	}

private:

	bool m_bIsDynamic;
	int m_nIndexCount;
	MaterialIndexFormat_t m_IndexFormat;

	int m_nIndexSize;
	size_t m_nBufferSize;
	size_t m_nBufferPosition;

	bool m_bIsLocked;

	unsigned short *m_pIndBuffer;

	ID3D11Buffer *m_pD3DBuffer;
};

class CMeshDX11 : public IMeshDX11
{
public:
	CMeshDX11(bool bIsDynamic, VertexFormat_t fmt);
	~CMeshDX11();

	// DX11 Buffer Commons
public:
	// Is the mesh dynamic?
	virtual bool IsDynamic() const;

	// NOTE: For dynamic meshes only!
	virtual void EndCastBuffer();

	// Returns room remaining :)
	virtual int GetRoomRemaining() const;

	// IVertexBufferDX11
public:
	// NOTE: The following two methods are only valid for static vertex buffers
	// Returns the number of vertices and the format of the vertex buffer
	virtual int VertexCount() const;
	virtual VertexFormat_t GetVertexFormat() const;

	// NOTE: For dynamic vertex buffers only!
	// Casts the memory of the dynamic vertex buffer to the appropriate type
	virtual void BeginCastBuffer(VertexFormat_t format);

	virtual bool Lock(int nVertexCount, bool bAppend, VertexDesc_t &desc);
	virtual void Unlock(int nVertexCount, VertexDesc_t &desc);

	// Spews the mesh data
	virtual void Spew(int nVertexCount, const VertexDesc_t &desc);

	// Call this in debug mode to make sure our data is good.
	virtual void ValidateData(int nVertexCount, const VertexDesc_t & desc);

	// IIndexBufferDX11
public:
	// NOTE: The following two methods are only valid for static index buffers
	// Returns the number of indices and the format of the index buffer
	virtual int IndexCount() const;
	virtual MaterialIndexFormat_t IndexFormat() const;

	// NOTE: For dynamic index buffers only!
	// Casts the memory of the dynamic index buffer to the appropriate type
	virtual void BeginCastBuffer(MaterialIndexFormat_t format);

	// Locks, unlocks the index buffer
	virtual bool Lock(int nMaxIndexCount, bool bAppend, IndexDesc_t &desc);
	virtual void Unlock(int nWrittenIndexCount, IndexDesc_t &desc);

	// FIXME: Remove this!!
	// Locks, unlocks the index buffer for modify
	virtual void ModifyBegin(bool bReadOnly, int nFirstIndex, int nIndexCount, IndexDesc_t& desc);
	virtual void ModifyEnd(IndexDesc_t& desc);

	// Spews the mesh data
	virtual void Spew(int nIndexCount, const IndexDesc_t &desc);

	// Ensures the data in the index buffer is valid
	virtual void ValidateData(int nIndexCount, const IndexDesc_t &desc);

	// IMesh
public:
	// -----------------------------------

	// Sets/gets the primitive type
	virtual void SetPrimitiveType(MaterialPrimitiveType_t type);

	// Draws the mesh
	virtual void Draw(int nFirstIndex = -1, int nIndexCount = 0);

	virtual void SetColorMesh(IMesh *pColorMesh, int nVertexOffset);

	// Draw a list of (lists of) primitives. Batching your lists together that use
	// the same lightmap, material, vertex and index buffers with multipass shaders
	// can drastically reduce state-switching overhead.
	// NOTE: this only works with STATIC meshes.
	virtual void Draw(CPrimList *pLists, int nLists);

	// Copy verts and/or indices to a mesh builder. This only works for temp meshes!
	virtual void CopyToMeshBuilder(
		int iStartVert,		// Which vertices to copy.
		int nVerts,
		int iStartIndex,	// Which indices to copy.
		int nIndices,
		int indexOffset,	// This is added to each index.
		CMeshBuilder &builder);

	// Spews the mesh data
	virtual void Spew(int nVertexCount, int nIndexCount, const MeshDesc_t &desc);

	// Call this in debug mode to make sure our data is good.
	virtual void ValidateData(int nVertexCount, int nIndexCount, const MeshDesc_t &desc);

	// New version
	// Locks/unlocks the mesh, providing space for nVertexCount and nIndexCount.
	// nIndexCount of -1 means don't lock the index buffer...
	virtual void LockMesh(int nVertexCount, int nIndexCount, MeshDesc_t &desc);
	virtual void ModifyBegin(int nFirstVertex, int nVertexCount, int nFirstIndex, int nIndexCount, MeshDesc_t& desc);
	virtual void ModifyEnd(MeshDesc_t& desc);
	virtual void UnlockMesh(int nVertexCount, int nIndexCount, MeshDesc_t &desc);

	virtual void ModifyBeginEx(bool bReadOnly, int nFirstVertex, int nVertexCount, int nFirstIndex, int nIndexCount, MeshDesc_t &desc);

	virtual void SetFlexMesh(IMesh *pMesh, int nVertexOffset);

	virtual void DisableFlexMesh();

	virtual void MarkAsDrawn();

	virtual unsigned ComputeMemoryUsed();

	inline CVertexBufferDX11 *GetVertexBuffer() { return m_pVertexBufferDX11; }
	inline CIndexBufferDX11 *GetIndexBuffer() { return m_pIndexBufferDX11; }

	inline void SetVertexFormat(VertexFormat_t fmt) { 
		m_pVertexBufferDX11->SetVertexFormat(fmt);
		m_VertFormat = fmt;
	}

	inline MaterialPrimitiveType_t GetTopology() { return m_PrimitiveType; }
private:
	bool m_bIsDynamic;

	bool m_bRequiresIndexUnlock;

	CVertexBufferDX11 *m_pVertexBufferDX11;
	CIndexBufferDX11 *m_pIndexBufferDX11;

	VertexFormat_t m_VertFormat;

	MaterialPrimitiveType_t m_PrimitiveType;

};

#endif