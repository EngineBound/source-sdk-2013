
#include "dx11global.h"
#include "shaderdevicedx11.h"
#include "shaderapidx11.h"

#include "meshdx11.h"

#include <d3d11.h>

#include "memdbgon.h"

extern CShaderAPIDX11 *g_pShaderAPIDX11;


//-------------------------------------------//
//             CVertexBufferDX11             //
//-------------------------------------------//

CVertexBufferDX11::CVertexBufferDX11(ShaderBufferType_t type, VertexFormat_t fmt, int nVertexCount, const char *pBudgetGroup)
{

	m_bIsDynamic = IsDynamicBufferType(type);
	m_nVertexCount = nVertexCount;
	m_VertexFormat = fmt;

	Assert(m_nVertexCount != 0);
	
	VertexDesc_t tmpDesc;
	ComputeVertexDesc(0, fmt, tmpDesc);

	m_nVertexSize = tmpDesc.m_ActualVertexSize;
	m_nBufferSize = (fmt == VERTEX_FORMAT_UNKNOWN) ? nVertexCount : m_nVertexSize * m_nVertexCount;
	m_nBufferPosition = 0;

	m_pD3DBuffer = NULL;
	m_pVertBuffer = NULL;

	m_nBufferPosition = 0;
	m_bIsLocked = false;

	if (m_nVertexCount == 0) return;

	if (!m_bIsDynamic)
		m_pVertBuffer = (unsigned char*)malloc(m_nBufferSize);
}

CVertexBufferDX11::~CVertexBufferDX11()
{
	DestroyBuffer();

	if (m_pVertBuffer)
		free(m_pVertBuffer);
}

bool CVertexBufferDX11::CreateBuffer()
{
	DestroyBuffer();

	if (m_nBufferSize == 0) return true;

	D3D11_BUFFER_DESC bufferDesc;
	if (m_bIsDynamic)
	{
		bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	}
	else
	{
		bufferDesc.Usage = D3D11_USAGE_DEFAULT;
		bufferDesc.CPUAccessFlags = 0;
	}

	bufferDesc.ByteWidth = m_nBufferSize;
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.MiscFlags = 0;

	HRESULT hr = g_pShaderDeviceDX11->GetDevice()->CreateBuffer(&bufferDesc, NULL, &m_pD3DBuffer);
	Assert(!FAILED(hr));

	m_nBufferPosition = 0;

	m_bIsLocked = false;

	return !FAILED(hr);
}

void CVertexBufferDX11::DestroyBuffer()
{
	if (m_pD3DBuffer)
		m_pD3DBuffer->Release();
	m_pD3DBuffer = NULL;
}

// NOTE: The following two methods are only valid for static vertex buffers
// Returns the number of vertices and the format of the vertex buffer
int CVertexBufferDX11::VertexCount() const
{
	return m_nVertexCount;
}

VertexFormat_t CVertexBufferDX11::GetVertexFormat() const
{
	return m_VertexFormat;
}


// Is this vertex buffer dynamic?
bool CVertexBufferDX11::IsDynamic() const
{
	return m_bIsDynamic;
}


// NOTE: For dynamic vertex buffers only!
// Casts the memory of the dynamic vertex buffer to the appropriate type
void CVertexBufferDX11::BeginCastBuffer(VertexFormat_t format)
{
	ALERT_NOT_IMPLEMENTED();
}

void CVertexBufferDX11::EndCastBuffer()
{
	ALERT_NOT_IMPLEMENTED();
}


// Returns the number of vertices that can still be written into the buffer
int CVertexBufferDX11::GetRoomRemaining() const
{
	if (m_nVertexSize == 0) // Just stick a massive number in because you can write however many you want
		return 0xFFFF;

	return m_nVertexCount - m_nBufferPosition / m_nVertexSize;
}


bool CVertexBufferDX11::Lock(int nVertexCount, bool bAppend, VertexDesc_t &desc)
{
	Assert(!m_bIsLocked);

	g_ShaderAPIMutex.Lock();

	if (m_nVertexCount == 0) // No lock, but technically a success
	{
		ComputeVertexDesc(0, 0, desc);
		desc.m_nFirstVertex = 0;
		desc.m_nOffset = 0;

		m_bIsLocked = true;
		return true;
	}

	if (m_bIsDynamic)
	{
		D3D11_MAPPED_SUBRESOURCE mappedResource;

		D3D11_MAP map = D3D11_MAP_WRITE_NO_OVERWRITE;
		if (!bAppend || GetRoomRemaining() < nVertexCount)
		{
			map = D3D11_MAP_WRITE_DISCARD;
			m_nBufferPosition = 0;
		}

		// ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
		HRESULT hr = g_pShaderDeviceDX11->GetDeviceContext()->Map(m_pD3DBuffer, 0, map, 0, &mappedResource);
		if (FAILED(hr))
		{
			Assert(0);
			ComputeVertexDesc(0, 0, desc);
			desc.m_nFirstVertex = 0;
			desc.m_nOffset = 0;

			g_ShaderAPIMutex.Unlock();

			return false;
		}

		ComputeVertexDesc((unsigned char *)mappedResource.pData + m_nBufferPosition, m_VertexFormat, desc);
		desc.m_nFirstVertex = m_nBufferPosition == 0 ? 0 : m_nBufferPosition / m_nVertexSize;
		desc.m_nOffset = m_nBufferPosition;
	}
	else
	{
		if (nVertexCount > m_nVertexCount)
		{
			AssertMsg(0, "%d < %d !", GetRoomRemaining(), nVertexCount);
			ComputeVertexDesc(0, 0, desc);
			desc.m_nFirstVertex = 0;
			desc.m_nOffset = 0;

			g_ShaderAPIMutex.Unlock();

			return false;
		}

		// Don't append ever for now
		m_nBufferPosition = 0;

		ComputeVertexDesc(m_pVertBuffer, m_VertexFormat, desc);
		desc.m_nFirstVertex = 0;
		desc.m_nOffset = 0;
	}

	m_bIsLocked = true;
	return true;

}

void CVertexBufferDX11::Unlock(int nVertexCount, VertexDesc_t &desc)
{
	if (!m_bIsLocked) return;

	Assert(nVertexCount <= m_nVertexCount);

	if (m_nVertexCount == 0)
	{
		g_ShaderAPIMutex.Unlock();

		m_bIsLocked = false;
		return;
	}

	int nMemWritten = nVertexCount * m_nVertexSize;	

	if (m_bIsDynamic)
	{
		g_pShaderDeviceDX11->GetDeviceContext()->Unmap(m_pD3DBuffer, 0);

	}
	else if (nMemWritten > 0)
	{
		D3D11_BOX box;
		box.left = m_nBufferPosition;
		box.right = m_nBufferPosition + nMemWritten;
		box.top = 0;
		box.bottom = 1;
		box.back = 1;
		box.front = 0;

		g_pShaderDeviceDX11->GetDeviceContext()->UpdateSubresource(m_pD3DBuffer, 0, &box, m_pVertBuffer + m_nBufferPosition, 0, 0);
	}

	m_nBufferPosition += nMemWritten;
	m_bIsLocked = false;

	g_ShaderAPIMutex.Unlock();
}


// Spews the mesh data
void CVertexBufferDX11::Spew(int nVertexCount, const VertexDesc_t &desc)
{
	AUTO_LOCK_FM(g_ShaderAPIMutex);

	ALERT_NOT_IMPLEMENTED();
}


// Call this in debug mode to make sure our data is good.
void CVertexBufferDX11::ValidateData(int nVertexCount, const VertexDesc_t & desc)
{
	ALERT_NOT_IMPLEMENTED();
}


//-------------------------------------------//
//              CIndexBufferDX11             //
//-------------------------------------------//

CIndexBufferDX11::CIndexBufferDX11(ShaderBufferType_t type, MaterialIndexFormat_t fmt, int nIndexCount, const char *pBudgetGroup)
{
	Assert(fmt != MATERIAL_INDEX_FORMAT_UNKNOWN);

	m_bIsDynamic = IsDynamicBufferType(type);
	m_nIndexCount = nIndexCount * 2; // FIXME: FOR SOME REASON * 2????
	m_IndexFormat = fmt;

	m_nIndexSize = SizeForIndex(fmt);
	m_nBufferSize = m_nIndexSize * m_nIndexCount;
	m_nBufferPosition = 0;

	m_bIsLocked = false;

	m_pD3DBuffer = NULL;
	m_pIndBuffer = NULL;

	m_nBufferPosition = 0;
	m_bIsLocked = false;

	if (m_nIndexCount == 0)
		return;

	if (!m_bIsDynamic)
		m_pIndBuffer = (unsigned short*)malloc(m_nBufferSize);
		
}

CIndexBufferDX11::~CIndexBufferDX11()
{
	DestroyBuffer();

	if (m_pIndBuffer)
		free(m_pIndBuffer);
}

bool CIndexBufferDX11::CreateBuffer()
{
	DestroyBuffer();

	if (m_nBufferSize == 0) return true;

	D3D11_BUFFER_DESC bufferDesc;
	if (m_bIsDynamic)
	{
		bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	}
	else
	{
		bufferDesc.Usage = D3D11_USAGE_DEFAULT;
		bufferDesc.CPUAccessFlags = 0;
	}

	bufferDesc.ByteWidth = m_nBufferSize;
	bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bufferDesc.MiscFlags = 0;

	HRESULT hr = g_pShaderDeviceDX11->GetDevice()->CreateBuffer(&bufferDesc, NULL, &m_pD3DBuffer);
	Assert(!FAILED(hr));

	m_nBufferPosition = 0;

	return !FAILED(hr);
}

void CIndexBufferDX11::DestroyBuffer()
{
	if (m_pD3DBuffer)
		m_pD3DBuffer->Release();
	m_pD3DBuffer = NULL;
}

// NOTE: The following two methods are only valid for static index buffers
// Returns the number of indices and the format of the index buffer
int CIndexBufferDX11::IndexCount() const
{
	return m_nIndexCount;
}

MaterialIndexFormat_t CIndexBufferDX11::IndexFormat() const
{
	return m_IndexFormat;
}


// Is this index buffer dynamic?
bool CIndexBufferDX11::IsDynamic() const
{
	return m_bIsDynamic;
}


// NOTE: For dynamic index buffers only!
// Casts the memory of the dynamic index buffer to the appropriate type
void CIndexBufferDX11::BeginCastBuffer(MaterialIndexFormat_t format)
{
	ALERT_NOT_IMPLEMENTED();
}

void CIndexBufferDX11::EndCastBuffer()
{
	ALERT_NOT_IMPLEMENTED();
}


// Returns the number of indices that can still be written into the buffer
int CIndexBufferDX11::GetRoomRemaining() const
{
	return m_nIndexCount - m_nBufferPosition / m_nIndexSize;
}


// Locks, unlocks the index buffer
bool CIndexBufferDX11::Lock(int nMaxIndexCount, bool bAppend, IndexDesc_t &desc)
{
	Assert(!m_bIsLocked);

	g_ShaderAPIMutex.Lock();

	if (m_nIndexCount == 0)
	{
		desc.m_nFirstIndex = 0;
		desc.m_nOffset = 0;
		desc.m_nIndexSize = 0;
		desc.m_pIndices = NULL;

		m_bIsLocked = true;
		return true;
	}

	if (m_bIsDynamic)
	{
		D3D11_MAPPED_SUBRESOURCE mappedResource;

		D3D11_MAP map = D3D11_MAP_WRITE_NO_OVERWRITE;
		if (!bAppend || GetRoomRemaining() < nMaxIndexCount)
		{
			map = D3D11_MAP_WRITE_DISCARD;
			m_nBufferPosition = 0;
		}

		// ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
		HRESULT hr = g_pShaderDeviceDX11->GetDeviceContext()->Map(m_pD3DBuffer, 0, map, 0, &mappedResource);
		if (FAILED(hr))
		{
			Assert(0);
			desc.m_nFirstIndex = 0;
			desc.m_nOffset = 0;
			desc.m_nIndexSize = 0;
			desc.m_pIndices = NULL;

			g_ShaderAPIMutex.Unlock();
			return false;
		}

		desc.m_nFirstIndex = m_nBufferPosition == 0 ? 0 : m_nBufferPosition / m_nIndexSize;
		desc.m_nOffset = m_nBufferPosition;
		desc.m_nIndexSize = 1; // For some reason indexdesc requires this and not actual size
		desc.m_pIndices = (unsigned short *)mappedResource.pData + m_nBufferPosition;
	}
	else
	{
		if (nMaxIndexCount > m_nIndexCount)
		{
			Assert(0);
			desc.m_nFirstIndex = 0;
			desc.m_nOffset = 0;
			desc.m_nIndexSize = 0;
			desc.m_pIndices = NULL;

			g_ShaderAPIMutex.Unlock();
			return false;
		}

		// Don't append ever for now
		m_nBufferPosition = 0;

		desc.m_nFirstIndex = 0;
		desc.m_nOffset = 0;
		desc.m_nIndexSize = 1;
		desc.m_pIndices = m_pIndBuffer;
	}

	m_bIsLocked = true;
	return true;
}

void CIndexBufferDX11::Unlock(int nWrittenIndexCount, IndexDesc_t &desc)
{
	if (!m_bIsLocked) return;

	Assert(nWrittenIndexCount <= m_nIndexCount); // probably incorrect :)

	int nMemWritten = nWrittenIndexCount * m_nIndexSize;

	if (m_nIndexCount == 0)
	{
		m_bIsLocked = false;
		g_ShaderAPIMutex.Unlock();

		return;
	}

	if (m_bIsDynamic)
	{
		g_pShaderDeviceDX11->GetDeviceContext()->Unmap(m_pD3DBuffer, 0);
	}
	else
	{
		D3D11_BOX box;
		box.left = m_nBufferPosition;
		box.right = m_nBufferPosition + nMemWritten;
		box.top = 0;
		box.bottom = 1;
		box.back = 1;
		box.front = 0;

		g_pShaderDeviceDX11->GetDeviceContext()->UpdateSubresource(m_pD3DBuffer, 0, &box, m_pIndBuffer + m_nBufferPosition, 0, 0);
	}

	m_nBufferPosition += nMemWritten;
	m_bIsLocked = false;

	g_ShaderAPIMutex.Unlock();
}


// FIXME: Remove this!!
// Locks, unlocks the index buffer for modify
void CIndexBufferDX11::ModifyBegin(bool bReadOnly, int nFirstIndex, int nIndexCount, IndexDesc_t& desc)
{
	ALERT_NOT_IMPLEMENTED();
}

void CIndexBufferDX11::ModifyEnd(IndexDesc_t& desc)
{
	ALERT_NOT_IMPLEMENTED();
}


// Spews the mesh data
void CIndexBufferDX11::Spew(int nIndexCount, const IndexDesc_t &desc)
{
	AUTO_LOCK_FM(g_ShaderAPIMutex);

	ALERT_NOT_IMPLEMENTED();
}


// Ensures the data in the index buffer is valid
void CIndexBufferDX11::ValidateData(int nIndexCount, const IndexDesc_t &desc)
{
	ALERT_NOT_IMPLEMENTED();
}


//-------------------------------------------//
//                 CMeshDX11                 //
//-------------------------------------------//

CMeshDX11::CMeshDX11(bool bIsDynamic, VertexFormat_t fmt)
{
	m_bIsDynamic = bIsDynamic;

	m_pIndexBufferDX11 = NULL;
	m_pVertexBufferDX11 = NULL;

	m_VertFormat = fmt;

	m_PrimitiveType = MATERIAL_TRIANGLES; // Easiest to handle by default

	if (m_bIsDynamic)
	{
		// REMOVE THIS MORON
		//VertexFormat_t tmpFmt = VERTEX_POSITION | VERTEX_COLOR | VERTEX_NORMAL | 2 << TEX_COORD_SIZE_BIT |
		//	VERTEX_BONEWEIGHT(2) | VERTEX_BONE_INDEX | VERTEX_USERDATA_SIZE(4) | VERTEX_SPECULAR;

		m_pVertexBufferDX11 = new CVertexBufferDX11(SHADER_BUFFER_TYPE_DYNAMIC, fmt, DYNAMIC_VERTEX_BUFFER_MEMORY, "");
		m_pIndexBufferDX11 = new CIndexBufferDX11(SHADER_BUFFER_TYPE_DYNAMIC, MATERIAL_INDEX_FORMAT_16BIT, INDEX_BUFFER_SIZE, "");

		m_pVertexBufferDX11->CreateBuffer();
		m_pIndexBufferDX11->CreateBuffer();
	}

	m_bRequiresIndexUnlock = false;
}

CMeshDX11::~CMeshDX11()
{
	if (m_pVertexBufferDX11)
		delete m_pVertexBufferDX11;
	if (m_pIndexBufferDX11)
		delete m_pIndexBufferDX11;
}

// Is the mesh dynamic?
bool CMeshDX11::IsDynamic() const
{
	return m_bIsDynamic;
}


// NOTE: For dynamic meshes only!
void CMeshDX11::EndCastBuffer()
{
	ALERT_NOT_IMPLEMENTED();
}


// Returns room remaining :)
int CMeshDX11::GetRoomRemaining() const
{
	ALERT_NOT_IMPLEMENTED();
	return -1;
}


// IVertexBufferDX11

// NOTE: The following two methods are only valid for static vertex buffers
// Returns the number of vertices and the format of the vertex buffer
int CMeshDX11::VertexCount() const
{
	return m_pVertexBufferDX11->VertexCount();
}

VertexFormat_t CMeshDX11::GetVertexFormat() const
{
	return m_VertFormat;
}


// NOTE: For dynamic vertex buffers only!
// Casts the memory of the dynamic vertex buffer to the appropriate type
void CMeshDX11::BeginCastBuffer(VertexFormat_t format)
{
	m_pVertexBufferDX11->BeginCastBuffer(format);
}


bool CMeshDX11::Lock(int nVertexCount, bool bAppend, VertexDesc_t &desc)
{
	g_ShaderAPIMutex.Lock();

	if (!m_pVertexBufferDX11)
	{
		m_pVertexBufferDX11 = new CVertexBufferDX11(SHADER_BUFFER_TYPE_STATIC, m_VertFormat, nVertexCount, "");

		m_pVertexBufferDX11->CreateBuffer();
	}

	return m_pVertexBufferDX11->Lock(nVertexCount, bAppend, desc);
}

void CMeshDX11::Unlock(int nVertexCount, VertexDesc_t &desc)
{
	m_pVertexBufferDX11->Unlock(nVertexCount, desc);

	g_ShaderAPIMutex.Unlock();
}


// Spews the mesh data
void CMeshDX11::Spew(int nVertexCount, const VertexDesc_t &desc)
{
	m_pVertexBufferDX11->Spew(nVertexCount, desc);
}


// Call this in debug mode to make sure our data is good.
void CMeshDX11::ValidateData(int nVertexCount, const VertexDesc_t & desc)
{
	m_pVertexBufferDX11->ValidateData(nVertexCount, desc);
}


// IIndexBufferDX11

// NOTE: The following two methods are only valid for static index buffers
// Returns the number of indices and the format of the index buffer
int CMeshDX11::IndexCount() const
{
	return m_pIndexBufferDX11->IndexCount();
}

MaterialIndexFormat_t CMeshDX11::IndexFormat() const
{
	return m_pIndexBufferDX11->IndexFormat();
}


// NOTE: For dynamic index buffers only!
// Casts the memory of the dynamic index buffer to the appropriate type
void CMeshDX11::BeginCastBuffer(MaterialIndexFormat_t format)
{
	m_pIndexBufferDX11->BeginCastBuffer(format);
}


// Locks, unlocks the index buffer
bool CMeshDX11::Lock(int nMaxIndexCount, bool bAppend, IndexDesc_t &desc)
{
	g_ShaderAPIMutex.Lock();

	if (!m_pIndexBufferDX11)
	{
		m_pIndexBufferDX11 = new CIndexBufferDX11(SHADER_BUFFER_TYPE_STATIC, MATERIAL_INDEX_FORMAT_16BIT, nMaxIndexCount, "");

		m_pIndexBufferDX11->CreateBuffer();
	}

	return m_pIndexBufferDX11->Lock(nMaxIndexCount, bAppend, desc);
}

void CMeshDX11::Unlock(int nWrittenIndexCount, IndexDesc_t &desc)
{
	m_pIndexBufferDX11->Unlock(nWrittenIndexCount, desc);

	g_ShaderAPIMutex.Unlock();
}


// FIXME: Remove this!!
// Locks, unlocks the index buffer for modify
void CMeshDX11::ModifyBegin(bool bReadOnly, int nFirstIndex, int nIndexCount, IndexDesc_t& desc)
{
	m_pIndexBufferDX11->ModifyBegin(bReadOnly, nFirstIndex, nIndexCount, desc);
}

void CMeshDX11::ModifyEnd(IndexDesc_t& desc)
{
	m_pIndexBufferDX11->ModifyEnd(desc);
}


// Spews the mesh data
void CMeshDX11::Spew(int nIndexCount, const IndexDesc_t &desc)
{
	m_pIndexBufferDX11->Spew(nIndexCount, desc);
}


// Ensures the data in the index buffer is valid
void CMeshDX11::ValidateData(int nIndexCount, const IndexDesc_t &desc)
{
	m_pIndexBufferDX11->ValidateData(nIndexCount, desc);
}


// IMesh

// -----------------------------------

// Sets/gets the primitive type
void CMeshDX11::SetPrimitiveType(MaterialPrimitiveType_t type)
{
	m_PrimitiveType = type;
}


// Draws the mesh
void CMeshDX11::Draw(int nFirstIndex, int nIndexCount)
{
	ALERT_INCOMPLETE();
	g_pShaderAPIDX11->DrawMesh(this);
}


void CMeshDX11::SetColorMesh(IMesh *pColorMesh, int nVertexOffset)
{
	ALERT_NOT_IMPLEMENTED();
}


// Draw a list of (lists of) primitives. Batching your lists together that use
// the same lightmap, material, vertex and index buffers with multipass shaders
// can drastically reduce state-switching overhead.
// NOTE: this only works with STATIC meshes.
void CMeshDX11::Draw(CPrimList *pLists, int nLists)
{
	ALERT_NOT_IMPLEMENTED();
}


// Copy verts and/or indices to a mesh builder. This only works for temp meshes!
void CMeshDX11::CopyToMeshBuilder(
	int iStartVert,		// Which vertices to copy.
	int nVerts,
	int iStartIndex,	// Which indices to copy.
	int nIndices,
	int indexOffset,	// This is added to each index.
	CMeshBuilder &builder)
{
	ALERT_NOT_IMPLEMENTED();
}

// Spews the mesh data
void CMeshDX11::Spew(int nVertexCount, int nIndexCount, const MeshDesc_t &desc)
{
	AUTO_LOCK_FM(g_ShaderAPIMutex);

	ALERT_NOT_IMPLEMENTED();
}


// Call this in debug mode to make sure our data is good.
void CMeshDX11::ValidateData(int nVertexCount, int nIndexCount, const MeshDesc_t &desc)
{
	ValidateData(nVertexCount, static_cast<const VertexDesc_t &>(desc));
	ValidateData(nIndexCount, static_cast<const IndexDesc_t &>(desc));
}


// New version
// Locks/unlocks the mesh, providing space for nVertexCount and nIndexCount.
// nIndexCount of -1 means don't lock the index buffer...
void CMeshDX11::LockMesh(int nVertexCount, int nIndexCount, MeshDesc_t &desc)
{
	g_ShaderAPIMutex.Lock();

	Lock(nVertexCount, false, static_cast<VertexDesc_t&>(desc));
	if (nIndexCount > -1)
	{
		Lock(nIndexCount, false, static_cast<IndexDesc_t&>(desc));

		m_bRequiresIndexUnlock = true;
	}
	else
	{
		m_bRequiresIndexUnlock = false;
	}
}

void CMeshDX11::ModifyBegin(int nFirstVertex, int nVertexCount, int nFirstIndex, int nIndexCount, MeshDesc_t& desc)
{
	ALERT_NOT_IMPLEMENTED();
}

void CMeshDX11::ModifyEnd(MeshDesc_t& desc)
{
	ALERT_NOT_IMPLEMENTED();
}

void CMeshDX11::UnlockMesh(int nVertexCount, int nIndexCount, MeshDesc_t &desc)
{
	Unlock(nVertexCount, static_cast<VertexDesc_t&>(desc));
	if (m_bRequiresIndexUnlock)
		Unlock(nIndexCount, static_cast<IndexDesc_t&>(desc));

	m_bRequiresIndexUnlock = false;

	g_ShaderAPIMutex.Unlock();
}


void CMeshDX11::ModifyBeginEx(bool bReadOnly, int nFirstVertex, int nVertexCount, int nFirstIndex, int nIndexCount, MeshDesc_t &desc)
{
	ALERT_NOT_IMPLEMENTED();
}


void CMeshDX11::SetFlexMesh(IMesh *pMesh, int nVertexOffset)
{
	ALERT_NOT_IMPLEMENTED();
}


void CMeshDX11::DisableFlexMesh()
{
	ALERT_NOT_IMPLEMENTED();
}


void CMeshDX11::MarkAsDrawn()
{
	ALERT_NOT_IMPLEMENTED();
}


unsigned CMeshDX11::ComputeMemoryUsed()
{
	ALERT_NOT_IMPLEMENTED();
	return 0;
}
