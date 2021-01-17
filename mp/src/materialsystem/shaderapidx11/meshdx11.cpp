#include "meshdx11.h"
#include "shaderdevicedx11.h"
#include "shaderapidx11_global.h"
#include "ishaderutil.h"

CBaseMeshDX11::CBaseMeshDX11(bool bIsDynamic) : m_bIsDynamic(bIsDynamic), m_nNumInds(0), m_nNumVerts(0),
m_pIndexBuffer(0), m_pVertexBuffer(0), m_Type(MATERIAL_TRIANGLES), m_VertexFormat(0)
{
	m_pVertexData = new unsigned char[VERTEX_DATA_SIZE];

	m_bIsIBufLocked = false;
	m_bIsVBufLocked = false;
	m_bMeshLocked = false;
}

CBaseMeshDX11::~CBaseMeshDX11()
{
	delete[] m_pVertexData;

	if (m_bIsDynamic)
	{
		if (m_pIndexBuffer)
		{
			g_pShaderDeviceDX11->DestroyIndexBuffer(m_pIndexBuffer);
		}
	}
}

//--------------------------------------------------------------//
//                          IVertexBuffer                       //
//--------------------------------------------------------------//

// NOTE: The following two methods are only valid for static vertex buffers
	// Returns the number of vertices and the format of the vertex buffer
int CBaseMeshDX11::VertexCount() const
{
	return m_pVertexBuffer ? m_pVertexBuffer->VertexCount() : 0;
}

void CBaseMeshDX11::SetVertexFormat(VertexFormat_t format)
{
	m_VertexFormat = format;
}

VertexFormat_t CBaseMeshDX11::GetVertexFormat() const
{
	return m_VertexFormat;
}

// Is this vertex buffer dynamic?
bool CBaseMeshDX11::IsDynamic() const
{
	return m_bIsDynamic;
}

// NOTE: For dynamic vertex buffers only!
// Casts the memory of the dynamic vertex buffer to the appropriate type
void CBaseMeshDX11::BeginCastBuffer(VertexFormat_t format)
{
	
}

void CBaseMeshDX11::EndCastBuffer()
{

}

// Returns the number of vertices that can still be written into the buffer
int CBaseMeshDX11::GetRoomRemaining() const
{
	return m_pVertexBuffer->GetRoomRemaining();
}

bool CBaseMeshDX11::Lock(int nVertexCount, bool bAppend, VertexDesc_t &desc)
{
	Assert(!m_bIsVBufLocked);

	if (!g_pShaderDeviceDX11->IsActivated() || nVertexCount == 0)
	{
		CVertexBufferDX11::ComputeVertexDesc(0, 0, desc);
		desc.m_nFirstVertex = 0;
		return false;
	}

	if (!m_pVertexBuffer)
	{
		m_pVertexBuffer = static_cast<CVertexBufferDX11 *>(
			g_pShaderDeviceDX11->CreateVertexBuffer(SHADER_BUFFER_TYPE_STATIC,
				m_VertexFormat, nVertexCount, NULL));
	}


	if (!m_pVertexBuffer->Lock(nVertexCount, bAppend, desc))
	{
		Assert(0);
		Error("Vertex buffer lock failed!!!\n");
		return false;
	}

	m_bIsVBufLocked = true;
	return true;
}
void CBaseMeshDX11::Unlock(int nVertexCount, VertexDesc_t &desc) 
{
	if (!m_bIsVBufLocked)
		return;

	Assert(m_pVertexBuffer);
	m_pVertexBuffer->Unlock(nVertexCount, desc);
	m_bIsVBufLocked = false;
}

// Spews the mesh data
void CBaseMeshDX11::Spew(int nVertexCount, const VertexDesc_t &desc)
{

}

// Call this in debug mode to make sure our data is good.
void CBaseMeshDX11::ValidateData(int nVertexCount, const VertexDesc_t & desc)
{
}

//--------------------------------------------------------------//
//                          IIndexBuffer                        //
//--------------------------------------------------------------//

	// NOTE: The following two methods are only valid for static index buffers
	// Returns the number of indices and the format of the index buffer
int CBaseMeshDX11::IndexCount() const
{
	return m_pIndexBuffer ? m_pIndexBuffer->IndexCount() : 0;
}

MaterialIndexFormat_t CBaseMeshDX11::IndexFormat() const
{
	return m_pIndexBuffer->IndexFormat();
}

// NOTE: For dynamic index buffers only!
// Casts the memory of the dynamic index buffer to the appropriate type
void CBaseMeshDX11::BeginCastBuffer(MaterialIndexFormat_t format)
{

}

// Locks, unlocks the index buffer
bool CBaseMeshDX11::Lock(int nMaxIndexCount, bool bAppend, IndexDesc_t &desc)
{
	Assert(!m_bIsIBufLocked);

	if (!m_pIndexBuffer)
	{
		m_pIndexBuffer = static_cast<CIndexBufferDX11 *>(
			g_pShaderDeviceDX11->CreateIndexBuffer(SHADER_BUFFER_TYPE_STATIC,
				MATERIAL_INDEX_FORMAT_16BIT,
				nMaxIndexCount, NULL));
	}

	if (!m_pIndexBuffer->Lock(nMaxIndexCount, bAppend, desc))
	{
		return false;
	}

	m_bIsIBufLocked = true;
	return true;
}

void CBaseMeshDX11::Unlock(int nWrittenIndexCount, IndexDesc_t &desc)
{
	if (!m_bIsIBufLocked)
		return;

	Assert(m_pIndexBuffer);

	m_pIndexBuffer->Unlock(nWrittenIndexCount, desc);
	m_bIsIBufLocked = false;
}

void CBaseMeshDX11::ModifyBegin(bool bReadOnly, int nFirstIndex, int nIndexCount, IndexDesc_t& desc)
{
	Lock(nIndexCount, false, desc);
}

void CBaseMeshDX11::ModifyEnd(IndexDesc_t& desc)
{
	Unlock(0, desc);
}

// Spews the mesh data
void CBaseMeshDX11::Spew(int nIndexCount, const IndexDesc_t &desc)
{

}

// Ensures the data in the index buffer is valid
void CBaseMeshDX11::ValidateData(int nIndexCount, const IndexDesc_t &desc)
{
}

//--------------------------------------------------------------//
//                              IMesh                           //
//--------------------------------------------------------------//


// Sets the primitive type
void CBaseMeshDX11::SetPrimitiveType(MaterialPrimitiveType_t type)
{
	Assert(type != MATERIAL_INSTANCED_QUADS);
	if (!ShaderUtil()->OnSetPrimitiveType(this, type))
	{
		return;
	}

	m_Type = type;
}

// Draws the entire mesh
void CBaseMeshDX11::Draw(int firstIndex, int numIndices)
{
}

void CBaseMeshDX11::SetColorMesh(IMesh *pColorMesh, int nVertexOffset)
{
}

// Draw a list of (lists of) primitives. Batching your lists together that use
// the same lightmap, material, vertex and index buffers with multipass shaders
// can drastically reduce state-switching overhead.
// NOTE: this only works with STATIC meshes.
void CBaseMeshDX11::Draw(CPrimList *pLists, int nLists)
{
}

// Copy verts and/or indices to a mesh builder. This only works for temp meshes!
void CBaseMeshDX11::CopyToMeshBuilder(
	int iStartVert,		// Which vertices to copy.
	int nVerts,
	int iStartIndex,	// Which indices to copy.
	int nIndices,
	int indexOffset,	// This is added to each index.
	CMeshBuilder &builder)
{
}

// Spews the mesh data
void CBaseMeshDX11::Spew(int nVertexCount, int nIndexCount, const MeshDesc_t &desc)
{
}

// Call this in debug mode to make sure our data is good.
void CBaseMeshDX11::ValidateData(int nVertexCount, int nIndexCount, const MeshDesc_t &desc)
{
}

// New version
// Locks/unlocks the mesh, providing space for nVertexCount and nIndexCount.
// nIndexCount of -1 means don't lock the index buffer...
void CBaseMeshDX11::LockMesh(int nVertexCount, int nIndexCount, MeshDesc_t &desc)
{
	Assert(!m_bMeshLocked);

	Lock(nVertexCount, false, *static_cast<VertexDesc_t*>(&desc));

	if (m_Type != MATERIAL_POINTS)
		Lock(nIndexCount, false, *static_cast<IndexDesc_t*>(&desc));

	m_bMeshLocked = true;
}

void CBaseMeshDX11::ModifyBegin(int nFirstVertex, int nVertexCount, int nFirstIndex, int nIndexCount, MeshDesc_t& desc)
{
	ModifyBegin(false, nFirstIndex, nIndexCount, *static_cast<IndexDesc_t *>(&desc));
}
void CBaseMeshDX11::ModifyEnd(MeshDesc_t& desc)
{
	UnlockMesh(0, 0, desc);
}
void CBaseMeshDX11::UnlockMesh(int nVertexCount, int nIndexCount, MeshDesc_t &desc)
{
	Assert(m_bMeshLocked);

	Unlock(nVertexCount, *static_cast<VertexDesc_t*>(&desc));

	if (m_Type != MATERIAL_POINTS)
		Unlock(nIndexCount, *static_cast<IndexDesc_t*>(&desc));

	m_nNumInds = nIndexCount;
	m_nNumVerts = nVertexCount;

	m_bMeshLocked = false;
}

void CBaseMeshDX11::ModifyBeginEx(bool bReadOnly, int nFirstVertex, int nVertexCount, int nFirstIndex, int nIndexCount, MeshDesc_t &desc)
{
	LockMesh(nVertexCount, nIndexCount, desc);
}

void CBaseMeshDX11::SetFlexMesh(IMesh *pMesh, int nVertexOffset)
{
}

void CBaseMeshDX11::DisableFlexMesh()
{
}

void CBaseMeshDX11::MarkAsDrawn()
{
}

unsigned CBaseMeshDX11::ComputeMemoryUsed()
{
	return 0;
}