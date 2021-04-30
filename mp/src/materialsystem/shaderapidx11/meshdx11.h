#ifndef MESHDX11_H
#define MESHDX11_H

#ifdef _WIN32
#pragma once
#endif

#include "imeshdx11.h"
#include "ishaderdevice.h"
#include <d3d11.h>

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

private:

	bool m_bIsDynamic;
	int m_nVertexCount;
	VertexFormat_t m_VertexFormat;

	int m_nVertexSize;
	size_t m_nBufferSize;

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

private:

	bool m_bIsDynamic;
	int m_nIndexCount;
	MaterialIndexFormat_t m_IndexFormat;

	int m_nIndexSize;
	size_t m_nBufferSize;

	ID3D11Buffer *m_pD3DBuffer;
};

class CMeshDX11 : public IMeshDX11
{
public:
	CMeshDX11(bool bIsDynamic);
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
private:
	bool m_bIsDynamic;

	CVertexBufferDX11 *m_pVertexBufferDX11;
	CIndexBufferDX11 *m_pIndexBufferDX11;

};

#endif