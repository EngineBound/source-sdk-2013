
#include "meshdx11.h"

//-------------------------------------------//
//             CVertexBufferDX11             //
//-------------------------------------------//

// NOTE: The following two methods are only valid for static vertex buffers
// Returns the number of vertices and the format of the vertex buffer
int CVertexBufferDX11::VertexCount() const
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
	return -1;
}

VertexFormat_t CVertexBufferDX11::GetVertexFormat() const
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
	return VERTEX_FORMAT_UNKNOWN;
}


// Is this vertex buffer dynamic?
bool CVertexBufferDX11::IsDynamic() const
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
	return false;
}


// NOTE: For dynamic vertex buffers only!
// Casts the memory of the dynamic vertex buffer to the appropriate type
void CVertexBufferDX11::BeginCastBuffer(VertexFormat_t format)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}

void CVertexBufferDX11::EndCastBuffer()
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}


// Returns the number of vertices that can still be written into the buffer
int CVertexBufferDX11::GetRoomRemaining() const
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
	return -1;
}


bool CVertexBufferDX11::Lock(int nVertexCount, bool bAppend, VertexDesc_t &desc)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
	return false;
}

void CVertexBufferDX11::Unlock(int nVertexCount, VertexDesc_t &desc)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}


// Spews the mesh data
void CVertexBufferDX11::Spew(int nVertexCount, const VertexDesc_t &desc)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}


// Call this in debug mode to make sure our data is good.
void CVertexBufferDX11::ValidateData(int nVertexCount, const VertexDesc_t & desc)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}


//-------------------------------------------//
//              CIndexBufferDX11             //
//-------------------------------------------//

// NOTE: The following two methods are only valid for static index buffers
// Returns the number of indices and the format of the index buffer
int CIndexBufferDX11::IndexCount() const
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
	return -1;
}

MaterialIndexFormat_t CIndexBufferDX11::IndexFormat() const
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
	return MATERIAL_INDEX_FORMAT_UNKNOWN;
}


// Is this index buffer dynamic?
bool CIndexBufferDX11::IsDynamic() const
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
	return false;
}


// NOTE: For dynamic index buffers only!
// Casts the memory of the dynamic index buffer to the appropriate type
void CIndexBufferDX11::BeginCastBuffer(MaterialIndexFormat_t format)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}

void CIndexBufferDX11::EndCastBuffer()
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}


// Returns the number of indices that can still be written into the buffer
int CIndexBufferDX11::GetRoomRemaining() const
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
	return -1;
}


// Locks, unlocks the index buffer
bool CIndexBufferDX11::Lock(int nMaxIndexCount, bool bAppend, IndexDesc_t &desc)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
	return false;
}

void CIndexBufferDX11::Unlock(int nWrittenIndexCount, IndexDesc_t &desc)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}


// FIXME: Remove this!!
// Locks, unlocks the index buffer for modify
void CIndexBufferDX11::ModifyBegin(bool bReadOnly, int nFirstIndex, int nIndexCount, IndexDesc_t& desc)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}

void CIndexBufferDX11::ModifyEnd(IndexDesc_t& desc)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}


// Spews the mesh data
void CIndexBufferDX11::Spew(int nIndexCount, const IndexDesc_t &desc)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}


// Ensures the data in the index buffer is valid
void CIndexBufferDX11::ValidateData(int nIndexCount, const IndexDesc_t &desc)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}


//-------------------------------------------//
//                 CMeshDX11                 //
//-------------------------------------------//

// Is the mesh dynamic?
bool CMeshDX11::IsDynamic() const
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
	return false;
}


// NOTE: For dynamic meshes only!
void CMeshDX11::EndCastBuffer()
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}


// Returns room remaining :)
int CMeshDX11::GetRoomRemaining() const
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
	return -1;
}


// IVertexBufferDX11

// NOTE: The following two methods are only valid for static vertex buffers
// Returns the number of vertices and the format of the vertex buffer
int CMeshDX11::VertexCount() const
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
	return -1;
}

VertexFormat_t CMeshDX11::GetVertexFormat() const
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
	return VERTEX_FORMAT_UNKNOWN;
}


// NOTE: For dynamic vertex buffers only!
// Casts the memory of the dynamic vertex buffer to the appropriate type
void CMeshDX11::BeginCastBuffer(VertexFormat_t format)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}


bool CMeshDX11::Lock(int nVertexCount, bool bAppend, VertexDesc_t &desc)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
	return false;
}

void CMeshDX11::Unlock(int nVertexCount, VertexDesc_t &desc)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}


// Spews the mesh data
void CMeshDX11::Spew(int nVertexCount, const VertexDesc_t &desc)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}


// Call this in debug mode to make sure our data is good.
void CMeshDX11::ValidateData(int nVertexCount, const VertexDesc_t & desc)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}


// IIndexBufferDX11

// NOTE: The following two methods are only valid for static index buffers
// Returns the number of indices and the format of the index buffer
int CMeshDX11::IndexCount() const
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
	return -1;
}

MaterialIndexFormat_t CMeshDX11::IndexFormat() const
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
	return MATERIAL_INDEX_FORMAT_UNKNOWN;
}


// NOTE: For dynamic index buffers only!
// Casts the memory of the dynamic index buffer to the appropriate type
void CMeshDX11::BeginCastBuffer(MaterialIndexFormat_t format)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}


// Locks, unlocks the index buffer
bool CMeshDX11::Lock(int nMaxIndexCount, bool bAppend, IndexDesc_t &desc)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
	return false;
}

void CMeshDX11::Unlock(int nWrittenIndexCount, IndexDesc_t &desc)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}


// FIXME: Remove this!!
// Locks, unlocks the index buffer for modify
void CMeshDX11::ModifyBegin(bool bReadOnly, int nFirstIndex, int nIndexCount, IndexDesc_t& desc)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}

void CMeshDX11::ModifyEnd(IndexDesc_t& desc)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}


// Spews the mesh data
void CMeshDX11::Spew(int nIndexCount, const IndexDesc_t &desc)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}


// Ensures the data in the index buffer is valid
void CMeshDX11::ValidateData(int nIndexCount, const IndexDesc_t &desc)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}


// IMesh

// -----------------------------------

// Sets/gets the primitive type
void CMeshDX11::SetPrimitiveType(MaterialPrimitiveType_t type)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}


// Draws the mesh
void CMeshDX11::Draw(int nFirstIndex, int nIndexCount)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}


void CMeshDX11::SetColorMesh(IMesh *pColorMesh, int nVertexOffset)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}


// Draw a list of (lists of) primitives. Batching your lists together that use
// the same lightmap, material, vertex and index buffers with multipass shaders
// can drastically reduce state-switching overhead.
// NOTE: this only works with STATIC meshes.
void CMeshDX11::Draw(CPrimList *pLists, int nLists)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}


// Copy verts and/or indices to a mesh builder. This only works for temp meshes!
void CopyToMeshBuilder(
	int iStartVert,		// Which vertices to copy.
	int nVerts,
	int iStartIndex,	// Which indices to copy.
	int nIndices,
	int indexOffset,	// This is added to each index.
	CMeshBuilder &builder)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}

// Spews the mesh data
void CMeshDX11::Spew(int nVertexCount, int nIndexCount, const MeshDesc_t &desc)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}


// Call this in debug mode to make sure our data is good.
void CMeshDX11::ValidateData(int nVertexCount, int nIndexCount, const MeshDesc_t &desc)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}


// New version
// Locks/unlocks the mesh, providing space for nVertexCount and nIndexCount.
// nIndexCount of -1 means don't lock the index buffer...
void CMeshDX11::LockMesh(int nVertexCount, int nIndexCount, MeshDesc_t &desc)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}

void CMeshDX11::ModifyBegin(int nFirstVertex, int nVertexCount, int nFirstIndex, int nIndexCount, MeshDesc_t& desc)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}

void CMeshDX11::ModifyEnd(MeshDesc_t& desc)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}

void CMeshDX11::UnlockMesh(int nVertexCount, int nIndexCount, MeshDesc_t &desc)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}


void CMeshDX11::ModifyBeginEx(bool bReadOnly, int nFirstVertex, int nVertexCount, int nFirstIndex, int nIndexCount, MeshDesc_t &desc)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}


void CMeshDX11::SetFlexMesh(IMesh *pMesh, int nVertexOffset)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}


void CMeshDX11::DisableFlexMesh()
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}


void CMeshDX11::MarkAsDrawn()
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}


unsigned CMeshDX11::ComputeMemoryUsed()
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
	return 0;
}
