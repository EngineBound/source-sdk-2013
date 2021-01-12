#include "meshdx11.h"

CBaseMeshDX11::CBaseMeshDX11(bool bIsDynamic) : m_bIsDynamic( bIsDynamic )
{
	m_pVertexData = new unsigned char[VERTEX_DATA_SIZE];
}

CBaseMeshDX11::~CBaseMeshDX11()
{
	delete[] m_pVertexData;
}

//--------------------------------------------------------------//
//                          IVertexBuffer                       //
//--------------------------------------------------------------//

// NOTE: The following two methods are only valid for static vertex buffers
	// Returns the number of vertices and the format of the vertex buffer
int CBaseMeshDX11::VertexCount() const
{
	return 0;
}

VertexFormat_t CBaseMeshDX11::GetVertexFormat() const
{
	return VERTEX_POSITION;
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
	return 0;
}

bool CBaseMeshDX11::Lock(int nVertexCount, bool bAppend, VertexDesc_t &desc)
{
	desc.m_VertexSize_Position = 0;
	desc.m_VertexSize_BoneWeight = 0;
	desc.m_VertexSize_BoneMatrixIndex = 0;
	desc.m_VertexSize_Normal = 0;
	desc.m_VertexSize_Color = 0;
	desc.m_VertexSize_Specular = 0;

	for (int i = 0; i < VERTEX_MAX_TEXTURE_COORDINATES; ++i)
		desc.m_VertexSize_TexCoord[i] = 0;

	desc.m_VertexSize_TangentS = 0;
	desc.m_VertexSize_TangentT = 0;
	desc.m_VertexSize_Wrinkle = 0;

	desc.m_VertexSize_UserData = 0;

	desc.m_ActualVertexSize = 0;

	desc.m_CompressionType = VERTEX_COMPRESSION_NONE;

	desc.m_NumBoneWeights = 0; // 2

	desc.m_pPosition = (float*)m_pVertexData;

	desc.m_pBoneWeight = (float*)m_pVertexData;

	desc.m_pBoneMatrixIndex = (unsigned char*)m_pVertexData;

	desc.m_pNormal = (float*)m_pVertexData;

	desc.m_pColor = (unsigned char*)m_pVertexData;
	desc.m_pSpecular = (unsigned char*)m_pVertexData;

	for (int i = 0; i < VERTEX_MAX_TEXTURE_COORDINATES; ++i)
		desc.m_pTexCoord[i] = (float*)m_pVertexData;

	desc.m_pTangentS = (float*)m_pVertexData;
	desc.m_pTangentT = (float*)m_pVertexData;

	desc.m_pWrinkle = (float*)m_pVertexData;

	desc.m_pUserData = (float*)m_pVertexData;

	desc.m_nFirstVertex = 0;

	desc.m_nOffset = 0;

	return true;
}
void CBaseMeshDX11::Unlock(int nVertexCount, VertexDesc_t &desc) 
{
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
	return 0;
}

MaterialIndexFormat_t CBaseMeshDX11::IndexFormat() const
{
	return MATERIAL_INDEX_FORMAT_UNKNOWN;
}

// NOTE: For dynamic index buffers only!
// Casts the memory of the dynamic index buffer to the appropriate type
void CBaseMeshDX11::BeginCastBuffer(MaterialIndexFormat_t format)
{

}

// Locks, unlocks the index buffer
bool CBaseMeshDX11::Lock(int nMaxIndexCount, bool bAppend, IndexDesc_t &desc)
{
	static int tmpIndex;
	desc.m_pIndices = (unsigned short*)&tmpIndex;
	desc.m_nOffset = 0;
	desc.m_nFirstIndex = 0;
	desc.m_nIndexSize = 0;

	return true;
}

void CBaseMeshDX11::Unlock(int nWrittenIndexCount, IndexDesc_t &desc)
{
}

void CBaseMeshDX11::ModifyBegin(bool bReadOnly, int nFirstIndex, int nIndexCount, IndexDesc_t& desc)
{
	Lock(nIndexCount, false, desc);
}

void CBaseMeshDX11::ModifyEnd(IndexDesc_t& desc)
{
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
	Lock(nVertexCount, false, *static_cast<VertexDesc_t*>(&desc));
	Lock(nIndexCount, false, *static_cast<IndexDesc_t*>(&desc));
}

void CBaseMeshDX11::ModifyBegin(int nFirstVertex, int nVertexCount, int nFirstIndex, int nIndexCount, MeshDesc_t& desc)
{
	LockMesh(nVertexCount, nIndexCount, desc);
}
void CBaseMeshDX11::ModifyEnd(MeshDesc_t& desc)
{
}
void CBaseMeshDX11::UnlockMesh(int nVertexCount, int nIndexCount, MeshDesc_t &desc)
{
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