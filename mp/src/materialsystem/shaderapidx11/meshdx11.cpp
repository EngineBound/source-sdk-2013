
#include "dx11global.h"
#include "shaderdevicedx11.h"

#include "meshdx11.h"

#include <d3d11.h>

#include "memdbgon.h"

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


//-------------------------------------------//
//             CVertexBufferDX11             //
//-------------------------------------------//

CVertexBufferDX11::CVertexBufferDX11(ShaderBufferType_t type, VertexFormat_t fmt, int nVertexCount, const char *pBudgetGroup)
{
	Assert(nVertexCount != 0);
	if (nVertexCount == 0)
		return;

	m_bIsDynamic = IsDynamicBufferType(type);
	m_nVertexCount = nVertexCount;
	m_VertexFormat = fmt;

	VertexDesc_t tmpDesc;
	ComputeVertexDesc(0, fmt, tmpDesc);

	m_nVertexSize = tmpDesc.m_ActualVertexSize;
	m_nBufferSize = (fmt == VERTEX_FORMAT_UNKNOWN) ? nVertexCount : m_nVertexSize * m_nVertexCount;
	m_nBufferPosition = 0;

	if (!m_bIsDynamic)
		m_pVertBuffer = (unsigned char*)malloc(m_nBufferSize);
	else
		m_pVertBuffer = NULL;
}

CVertexBufferDX11::~CVertexBufferDX11()
{
	DestroyBuffer();

	if (m_pVertBuffer)
		free(m_pVertBuffer);
}

bool CVertexBufferDX11::CreateBuffer()
{
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
	m_pD3DBuffer->Release();
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
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}

void CVertexBufferDX11::EndCastBuffer()
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}


// Returns the number of vertices that can still be written into the buffer
int CVertexBufferDX11::GetRoomRemaining() const
{
	return m_nVertexCount - m_nBufferPosition / m_nVertexSize;
}


bool CVertexBufferDX11::Lock(int nVertexCount, bool bAppend, VertexDesc_t &desc)
{
	Assert(!m_bIsLocked);

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
		HRESULT hr = g_pShaderDeviceDX11->GetDeviceContext()->Map(m_pD3DBuffer, 0, bAppend ? D3D11_MAP_WRITE_NO_OVERWRITE : D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		if (FAILED(hr))
		{
			Assert(0);
			ComputeVertexDesc(0, 0, desc);
			desc.m_nFirstVertex = 0;
			desc.m_nOffset = 0;

			return false;
		}

		ComputeVertexDesc((unsigned char *)mappedResource.pData + m_nBufferPosition, m_VertexFormat, desc);
		desc.m_nFirstVertex = m_nBufferPosition == 0 ? 0 : m_nBufferPosition / m_nVertexSize;
		desc.m_nOffset = m_nBufferPosition;
	}
	else
	{
		if (GetRoomRemaining() < nVertexCount)
		{
			AssertMsg(0, "%d < %d !", GetRoomRemaining(), nVertexCount);
			ComputeVertexDesc(0, 0, desc);
			desc.m_nFirstVertex = 0;
			desc.m_nOffset = 0;

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

	int nMemWritten = nVertexCount * m_nVertexSize;

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
		
		g_pShaderDeviceDX11->GetDeviceContext()->UpdateSubresource(m_pD3DBuffer, 0, &box, m_pVertBuffer + m_nBufferPosition, 0, 0);
	}

	m_nBufferPosition += nMemWritten;
	m_bIsLocked = false;
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

	if (!m_bIsDynamic)
		m_pIndBuffer = (unsigned short*)malloc(m_nBufferSize);
	else
		m_pIndBuffer = NULL;
}

CIndexBufferDX11::~CIndexBufferDX11()
{
	DestroyBuffer();

	if (m_pIndBuffer)
		free(m_pIndBuffer);
}

bool CIndexBufferDX11::CreateBuffer()
{
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
	m_pD3DBuffer->Release();
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
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}

void CIndexBufferDX11::EndCastBuffer()
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
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
		HRESULT hr = g_pShaderDeviceDX11->GetDeviceContext()->Map(m_pD3DBuffer, 0, bAppend ? D3D11_MAP_WRITE_NO_OVERWRITE : D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		if (FAILED(hr))
		{
			Assert(0);
			desc.m_nFirstIndex = 0;
			desc.m_nOffset = 0;
			desc.m_nIndexSize = 0;
			desc.m_pIndices = NULL;

			return false;
		}

		desc.m_nFirstIndex = m_nBufferPosition == 0 ? 0 : m_nBufferPosition / m_nIndexSize;
		desc.m_nOffset = m_nBufferPosition;
		desc.m_nIndexSize = 1; // For some reason indexdesc requires this and not actual size
		desc.m_pIndices = (unsigned short *)mappedResource.pData + m_nBufferPosition;
	}
	else
	{
		if (GetRoomRemaining() < nMaxIndexCount)
		{
			Assert(0);
			desc.m_nFirstIndex = 0;
			desc.m_nOffset = 0;
			desc.m_nIndexSize = 0;
			desc.m_pIndices = NULL;

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
	// DYNAMIC ONLY for now
	if (!m_bIsLocked) return;

	Assert(nWrittenIndexCount <= m_nIndexCount); // probably incorrect :)

	int nMemWritten = nWrittenIndexCount * m_nIndexSize;

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

CMeshDX11::CMeshDX11(bool bIsDynamic, VertexFormat_t fmt)
{
	// ONLY WORKS FOR DYNAMIC RIGHT NOW

	m_bIsDynamic = bIsDynamic;

	m_pIndexBufferDX11 = NULL;
	m_pVertexBufferDX11 = NULL;

	m_VertFormat = fmt;

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
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}


// Draws the mesh
void CMeshDX11::Draw(int nFirstIndex, int nIndexCount)
{
	_AssertMsg(0, "Incomplete implementation! " __FUNCTION__, 0, 0);
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
void CMeshDX11::CopyToMeshBuilder(
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
	ValidateData(nVertexCount, static_cast<const VertexDesc_t &>(desc));
	ValidateData(nIndexCount, static_cast<const IndexDesc_t &>(desc));
}


// New version
// Locks/unlocks the mesh, providing space for nVertexCount and nIndexCount.
// nIndexCount of -1 means don't lock the index buffer...
void CMeshDX11::LockMesh(int nVertexCount, int nIndexCount, MeshDesc_t &desc)
{
	Lock(nVertexCount, false, static_cast<VertexDesc_t &>(desc));
	if (nIndexCount > -1)
		Lock(nIndexCount, false, static_cast<IndexDesc_t &>(desc));
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
	Unlock(nVertexCount, static_cast<VertexDesc_t &>(desc));
	Unlock(nIndexCount, static_cast<IndexDesc_t &>(desc));
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
