#ifndef BUFFERSDX11_H
#define BUFFERSDX11_H

#ifdef _WIN32
#pragma once
#endif

#include "materialsystem/imesh.h"
#include "IShaderDevice.h"
#include <d3d11.h>

// Helper functions for vertex buffer

#include "materialsystem/imaterial.h"

class CIndexBufferDX11 : public IIndexBuffer
{
public:
	CIndexBufferDX11(int nIndexCount, MaterialIndexFormat_t indexFormat, ShaderBufferType_t bufferType);
	~CIndexBufferDX11();

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

	virtual void DummyDataDesc(IndexDesc_t &desc);

	// FIXME: Remove this!!
	// Locks, unlocks the index buffer for modify
	virtual void ModifyBegin(bool bReadOnly, int nFirstIndex, int nIndexCount, IndexDesc_t& desc);
	virtual void ModifyEnd(IndexDesc_t& desc);

	// Spews the mesh data
	virtual void Spew(int nIndexCount, const IndexDesc_t &desc);

	// Ensures the data in the index buffer is valid
	virtual void ValidateData(int nIndexCount, const IndexDesc_t &desc);

	// Allocates the buffer, returns if the buffer allocated
	virtual bool Allocate();

	// Frees the buffer
	virtual void Free();

	// Size of index in bytes for a given format
	static int SizeForIndex(MaterialIndexFormat_t indexFormat) {
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

	inline MaterialIndexFormat_t GetFormat() const
	{
		return m_IndexFormat;
	}

	inline ID3D11Buffer *GetDXBuffer() const
	{
		return m_pIndexBuffer;
	}

private:
	bool m_bIsDynamic;
	int m_nIndexCount;
	MaterialIndexFormat_t m_IndexFormat;
	int m_nIndexSize;
	int m_nBufferSize;

	ID3D11Buffer *m_pIndexBuffer;
	unsigned char *m_pIndexMemory;
	int m_nBufferPos;
	bool m_bIsLocked;
	int m_nLockedIndices;
};

class CVertexBufferDX11 : public IVertexBuffer
{
public:

	CVertexBufferDX11(int nVertexCount, VertexFormat_t vertexFormat, ShaderBufferType_t bufferType);
	~CVertexBufferDX11();

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

	virtual void DummyDataDesc(VertexDesc_t &desc);

	// Spews the mesh data
	virtual void Spew(int nVertexCount, const VertexDesc_t &desc);

	// Call this in debug mode to make sure our data is good.
	virtual void ValidateData(int nVertexCount, const VertexDesc_t & desc);

	// Allocates the buffer, returns if the buffer allocated
	virtual bool Allocate();

	// Frees the buffer
	virtual void Free();

	inline VertexFormat_t GetFormat() const
	{
		return m_VertexFormat;
	}

	inline ID3D11Buffer *GetDXBuffer() const
	{
		return m_pVertexBuffer;
	}

	int NextLockOffset() const
	{
		int nNextOffset = (m_nBufferPos + m_nVertexSize - 1) / m_nVertexSize;
		nNextOffset *= m_nVertexSize;
		return nNextOffset;
	}

	static void ComputeVertexDesc(unsigned char *pBuffer, VertexFormat_t vertexFormat, VertexDesc_t &desc);

private:
	bool m_bIsDynamic;
	int m_nVertexCount;
	VertexFormat_t m_VertexFormat;
	int m_nVertexSize;
	int m_nBufferSize;

	ID3D11Buffer *m_pVertexBuffer;
	unsigned char *m_pVertexMemory;
	int m_nBufferPos;
	bool m_bIsLocked;
	int m_nLockedVertices;

};

#endif