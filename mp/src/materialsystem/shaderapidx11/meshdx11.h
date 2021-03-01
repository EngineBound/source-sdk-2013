#ifndef SHADERUTILDX11_H
#define SHADERUTILDX11_H

#ifdef _WIN32
#pragma once
#endif

#include "materialsystem/imesh.h"
#include "buffersdx11.h"

#define VERTEX_DATA_SIZE 1024

class CBaseMeshDX11 : public IMesh
{
public:
	CBaseMeshDX11( bool bIsDynamic );
	~CBaseMeshDX11();

// IVertexBuffer methods
public:

	// NOTE: The following two methods are only valid for static vertex buffers
	// Returns the number of vertices and the format of the vertex buffer
	virtual int VertexCount() const;
	virtual void SetVertexFormat(VertexFormat_t format);
	virtual VertexFormat_t GetVertexFormat() const;

	// Is this mesh dynamic?
	virtual bool IsDynamic() const;

	// NOTE: For dynamic meshes only!
	// Casts the memory of the dynamic vertex buffer to the appropriate type
	virtual void BeginCastBuffer(VertexFormat_t format);
	virtual void EndCastBuffer();

	// Returns the number of bytes that can still be written into the buffer
	virtual int GetRoomRemaining() const;

	virtual bool Lock(int nVertexCount, bool bAppend, VertexDesc_t &desc);
	virtual void Unlock(int nVertexCount, VertexDesc_t &desc);

	// Spews the mesh data
	virtual void Spew(int nVertexCount, const VertexDesc_t &desc);

	// Call this in debug mode to make sure our data is good.
	virtual void ValidateData(int nVertexCount, const VertexDesc_t & desc);

	virtual void SetVertexBuffer(CVertexBufferDX11 *pBuffer)
	{
		m_pVertexBuffer = pBuffer;
	}

	virtual CVertexBufferDX11 *GetVertexBuffer()
	{
		return m_pVertexBuffer;
	}

// IIndexBuffer methods
public:

	// NOTE: The following two methods are only valid for static index buffers
	// Returns the number of indices and the format of the index buffer
	virtual int IndexCount() const;
	virtual MaterialIndexFormat_t IndexFormat() const;

	// NOTE: For dynamic meshes only!
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

	virtual void SetIndexBuffer(CIndexBufferDX11 *pBuffer)
	{
		m_pIndexBuffer = pBuffer;
	}

	virtual CIndexBufferDX11 *GetIndexBuffer()
	{
		return m_pIndexBuffer;
	}

// IMesh methods
public:

	// Sets/gets the primitive type
	virtual void SetPrimitiveType(MaterialPrimitiveType_t type);

	// Draws the mesh
	virtual void Draw(int nFirstIndex, int nIndexCount);

	virtual void SetColorMesh(IMesh *pColorMesh, int nVertexOffset);

	// Draw a list of (lists of) primitives. Batching your lists together that use
	// the same lightmap, material, vertex and index buffers with multipass shaders
	// can drastically reduce state-switching overhead.
	// NOTE: this only works with STATIC meshes.
	virtual void Draw(CPrimList *pLists, int nLists);

	virtual void DrawPrimLists(CPrimList *pLists, int nLists);

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

protected:
	bool m_bIsDynamic;
	unsigned char *m_pVertexData;

	CIndexBufferDX11 *m_pIndexBuffer;
	CVertexBufferDX11 *m_pVertexBuffer;
	bool m_bIsIBufLocked;
	bool m_bIsVBufLocked;
	bool m_bMeshLocked;
	VertexFormat_t m_VertexFormat;

	MaterialPrimitiveType_t m_Type;
	int m_nNumVerts;
	int m_nNumInds;
};

class CMeshDX11 : public CBaseMeshDX11
{
	typedef CBaseMeshDX11 BaseClass;
public:
	CMeshDX11(bool bIsDynamic) : BaseClass(bIsDynamic) {}

};

#endif