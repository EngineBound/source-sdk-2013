#include "buffersdx11.h"
#include "shaderapidx11_global.h"
#include "shaderdevicedx11.h"

// Dummy static int for referencing
static unsigned int s_nScratchIndexBuffer = 0;

CIndexBufferDX11::CIndexBufferDX11(int nIndexCount, MaterialIndexFormat_t indexFormat, ShaderBufferType_t bufferType)
{
	if (indexFormat == MATERIAL_INDEX_FORMAT_UNKNOWN)
	{
		indexFormat = MATERIAL_INDEX_FORMAT_16BIT;
		nIndexCount /= 2;
	}

	Assert(IsDynamicBufferType(bufferType) || (indexFormat != MATERIAL_INDEX_FORMAT_UNKNOWN));

	// Buffer constants, won't change
	m_bIsDynamic = IsDynamicBufferType(bufferType);
	m_nIndexCount = nIndexCount;
	m_IndexFormat = indexFormat;
	m_nIndexSize = SizeForIndex(indexFormat);
	m_nBufferSize = nIndexCount * m_nIndexSize;

	// Buffer data
	m_pIndexBuffer = NULL;
	m_pIndexMemory = NULL;
	m_nBufferPos = 0;
	m_bIsLocked = false;
	m_nLockedIndices = 0;
}

CIndexBufferDX11::~CIndexBufferDX11()
{
	Free();
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
	Assert(format != MATERIAL_INDEX_FORMAT_UNKNOWN);
	Assert(m_bIsDynamic);
	if (!m_bIsDynamic)
		return;

	m_IndexFormat = format;
	m_nIndexSize = SizeForIndex(m_IndexFormat);
	m_nIndexCount = m_nBufferSize / m_nIndexSize;
}

void CIndexBufferDX11::EndCastBuffer()
{
	Assert(m_IndexFormat != MATERIAL_INDEX_FORMAT_UNKNOWN);
	Assert(m_bIsDynamic);
	if (!m_bIsDynamic)
		return;

	m_IndexFormat = MATERIAL_INDEX_FORMAT_UNKNOWN;
	m_nIndexCount = 0;
	m_nIndexSize = 0;
}


// Returns the number of indices that can still be written into the buffer
int CIndexBufferDX11::GetRoomRemaining() const
{
	return (m_nBufferSize - m_nBufferPos) / m_nIndexSize;
}


// Locks, unlocks the index buffer
bool CIndexBufferDX11::Lock(int nMaxIndexCount, bool bAppend, IndexDesc_t &desc)
{
	Assert(!m_bIsLocked && (nMaxIndexCount <= m_nIndexCount));
	Assert(m_IndexFormat != MATERIAL_INDEX_FORMAT_UNKNOWN);

	if (m_IndexFormat == MATERIAL_INDEX_FORMAT_UNKNOWN || !g_pShaderDeviceDX11->IsActivated() || nMaxIndexCount == 0)
	{
		DummyDataDesc(desc);
		return false;
	}

	if (nMaxIndexCount > m_nIndexCount)
	{
		Warning("Too many indices for index buffer (%d>%d)\n", nMaxIndexCount, m_nIndexCount);
		DummyDataDesc(desc);
		return false;
	}

	if (!m_pIndexBuffer)
	{
		if (!Allocate())
		{
			DummyDataDesc(desc);
			return false;
		}
	}

	int nMemoryRequired = nMaxIndexCount * m_nIndexSize;

	if (m_bIsDynamic)
	{
		D3D11_MAPPED_SUBRESOURCE lockedData;
		D3D11_MAP map;

		if (m_nBufferPos + nMemoryRequired <= m_nBufferSize)
		{
			// Ensure never overwritten, loopable
			map = (m_nBufferPos == 0) ? D3D11_MAP_WRITE_DISCARD : D3D11_MAP_WRITE_NO_OVERWRITE;
		}
		else
		{
			// Reset buffer
			map = D3D11_MAP_WRITE_DISCARD;
			m_nBufferPos = 0;
		}

		HRESULT hr = g_pD3DDeviceContext->Map(m_pIndexBuffer, 0, map, 0, &lockedData);
		if (FAILED(hr))
		{
			Warning("Failed to lock index buffer\n");
			DummyDataDesc(desc);
			return false;
		}

		desc.m_pIndices = (unsigned short *)((unsigned char *)lockedData.pData + m_nBufferPos);
		desc.m_nIndexSize = m_nIndexSize >> 1;
		desc.m_nOffset = m_nBufferPos;
		desc.m_nFirstIndex = desc.m_nOffset / m_nIndexSize;

		m_bIsLocked = true;
		return true;
	}
	else
	{
		if (nMemoryRequired > m_nBufferSize)
		{
			DummyDataDesc(desc);
			return false;
		}

		m_nBufferPos = 0;

		desc.m_pIndices = (unsigned short *)(m_pIndexMemory);

		desc.m_nIndexSize = m_nIndexSize >> 1;
		desc.m_nOffset = 0;
		desc.m_nFirstIndex = 0;

		m_nLockedIndices = nMaxIndexCount;
		m_bIsLocked = true;
		return true;
	}
}

void CIndexBufferDX11::Unlock(int nWrittenIndexCount, IndexDesc_t &desc)
{
	Assert(nWrittenIndexCount <= m_nIndexCount);

	if (!m_bIsLocked)
		return;

	if (m_pIndexBuffer)
	{
		if (m_bIsDynamic)
		{
			g_pD3DDeviceContext->Unmap(m_pIndexBuffer, 0);
			m_nBufferPos += nWrittenIndexCount * m_nIndexSize;
		}
		else
		{
			D3D11_BOX dataRegion;
			dataRegion.back = 1;
			dataRegion.front = 0;
			dataRegion.bottom = 1;
			dataRegion.top = 0;
			dataRegion.left = m_nBufferPos;
			dataRegion.right = dataRegion.left + m_nLockedIndices * m_nIndexSize;

			g_pD3DDeviceContext->UpdateSubresource(m_pIndexBuffer, NULL, &dataRegion, m_pIndexMemory + m_nBufferPos, 0, 0);
			m_nBufferPos += m_nLockedIndices * m_nIndexSize;
		}
	}

	m_bIsLocked = false;
}

void CIndexBufferDX11::DummyDataDesc(IndexDesc_t &desc)
{
	desc.m_nFirstIndex = 0;
	desc.m_nIndexSize = 0;
	desc.m_nOffset = 0;
	desc.m_pIndices = (unsigned short *)&s_nScratchIndexBuffer;
}


// FIXME: Remove this!!
// Locks, unlocks the index buffer for modify
void CIndexBufferDX11::ModifyBegin(bool bReadOnly, int nFirstIndex, int nIndexCount, IndexDesc_t& desc)
{

}

void CIndexBufferDX11::ModifyEnd(IndexDesc_t& desc)
{

}


// Spews the mesh data
void CIndexBufferDX11::Spew(int nIndexCount, const IndexDesc_t &desc)
{

}


// Ensures the data in the index buffer is valid
void CIndexBufferDX11::ValidateData(int nIndexCount, const IndexDesc_t &desc)
{

}

bool CIndexBufferDX11::Allocate()
{
	Assert(!m_pIndexBuffer);

	m_nBufferPos = 0;

	D3D11_BUFFER_DESC bufferDesc;

	if (m_bIsDynamic)
	{
		m_pIndexMemory = NULL;

		bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	}
	else
	{
		m_pIndexMemory = (unsigned char *)malloc(m_nBufferSize);
		memset(m_pIndexMemory, 0, m_nBufferSize);

		bufferDesc.Usage = D3D11_USAGE_DEFAULT;
		bufferDesc.CPUAccessFlags = 0;
	}

	bufferDesc.ByteWidth = m_nBufferSize;
	bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bufferDesc.MiscFlags = 0;

	HRESULT hr = g_pD3DDevice->CreateBuffer(&bufferDesc, NULL, &m_pIndexBuffer);

	return !(FAILED(hr)) && (m_pIndexBuffer != NULL);
}

void CIndexBufferDX11::Free()
{
	if (m_pIndexBuffer)
	{
		m_pIndexBuffer->Release();
		m_pIndexBuffer = NULL;
	}

	if (m_pIndexMemory)
	{
		free(m_pIndexMemory);
		m_pIndexMemory = NULL;
	}
}

inline void CVertexBufferDX11::ComputeVertexDesc(unsigned char *pBuffer, VertexFormat_t vertexFormat, VertexDesc_t &desc)
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

CVertexBufferDX11::CVertexBufferDX11(int nVertexCount, VertexFormat_t vertexFormat, ShaderBufferType_t bufferType)
{
	Assert(nVertexCount != 0);

	// Buffer constants, won't change
	MeshDesc_t tmpDesc;
	ComputeVertexDesc(0, vertexFormat, tmpDesc);

	m_bIsDynamic = IsDynamicBufferType(bufferType);
	m_nVertexCount = nVertexCount;
	m_VertexFormat = vertexFormat;
	m_nVertexSize = tmpDesc.m_ActualVertexSize;
	m_nBufferSize = m_nVertexCount * m_nVertexSize;

	// Buffer data
	m_pVertexBuffer = NULL;
	m_pVertexMemory = NULL;
	m_nBufferPos = 0;
	m_bIsLocked = false;
	m_nLockedVertices = 0;
}

CVertexBufferDX11::~CVertexBufferDX11()
{
	Free();
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
	Assert(format != VERTEX_FORMAT_UNKNOWN);
	Assert(m_bIsDynamic);
	if (!m_bIsDynamic)
		return;

	m_VertexFormat = format;
	MeshDesc_t tmpDesc;
	ComputeVertexDesc(0, m_VertexFormat, tmpDesc);
	m_nVertexSize = tmpDesc.m_ActualVertexSize;
	m_nVertexCount = m_nBufferSize / m_nVertexSize;
}

void CVertexBufferDX11::EndCastBuffer()
{
	Assert(m_VertexFormat != VERTEX_FORMAT_UNKNOWN);
	Assert(m_bIsDynamic);
	if (!m_bIsDynamic)
		return;

	m_VertexFormat = VERTEX_FORMAT_UNKNOWN;
	m_nVertexCount = 0;
	m_nVertexSize = 0;
}


// Returns the number of vertices that can still be written into the buffer
int CVertexBufferDX11::GetRoomRemaining() const
{
	return (m_nBufferSize - m_nBufferPos) / m_nVertexSize;
}


bool CVertexBufferDX11::Lock(int nVertexCount, bool bAppend, VertexDesc_t &desc)
{
	Assert(!m_bIsLocked && (nVertexCount <= m_nVertexCount));
	Assert(m_VertexFormat != VERTEX_FORMAT_UNKNOWN);

	if (m_VertexFormat == VERTEX_FORMAT_UNKNOWN || !g_pShaderDeviceDX11->IsActivated() || nVertexCount == 0)
	{
		DummyDataDesc(desc);
		return false;
	}

	if (nVertexCount > m_nVertexCount)
	{
		Warning("Too many vertices for vertex buffer (%d>%d)\n", nVertexCount, m_nVertexCount);
		DummyDataDesc(desc);
		return false;
	}

	if (!m_pVertexBuffer)
	{
		if (!Allocate())
		{
			DummyDataDesc(desc);
			return false;
		}
	}

	if (m_bIsDynamic)
	{
		D3D11_MAPPED_SUBRESOURCE lockedData;
		D3D11_MAP map;

		int nLockOffset = NextLockOffset();

		if (nLockOffset + nVertexCount * m_nVertexSize <= m_nBufferSize)
		{
			// Ensure never overwritten, loopable
			map = (m_nBufferPos == 0) ? D3D11_MAP_WRITE_DISCARD : D3D11_MAP_WRITE_NO_OVERWRITE;
		}
		else
		{
			// Reset buffer
			map = D3D11_MAP_WRITE_DISCARD;
			m_nBufferPos = 0;
		}

		HRESULT hr = g_pD3DDeviceContext->Map(m_pVertexBuffer, 0, map, 0, &lockedData);
		if (FAILED(hr))
		{
			Warning("Failed to lock index buffer\n");
			DummyDataDesc(desc);
			return false;
		}

		ComputeVertexDesc((unsigned char *)lockedData.pData + nLockOffset, m_VertexFormat, desc);
		desc.m_nOffset = nLockOffset;
		desc.m_nFirstVertex = desc.m_nOffset / m_nVertexSize;

		m_bIsLocked = true;
		return true;
	}
	else
	{
		if (nVertexCount > m_nVertexCount)
		{
			DummyDataDesc(desc);
			return false;
		}

		m_nBufferPos = 0;

		ComputeVertexDesc(m_pVertexMemory, m_VertexFormat, desc);
		desc.m_nOffset = 0;
		desc.m_nFirstVertex = 0;

		m_nLockedVertices = nVertexCount;
		m_bIsLocked = true;
		return true;
	}
}

void CVertexBufferDX11::Unlock(int nVertexCount, VertexDesc_t &desc)
{
	Assert(nVertexCount <= m_nVertexCount);

	if (!m_bIsLocked)
		return;

	if (m_pVertexBuffer)
	{
		if (m_bIsDynamic)
		{
			g_pD3DDeviceContext->Unmap(m_pVertexBuffer, 0);
			m_nBufferPos = NextLockOffset() + nVertexCount * m_nVertexSize;
		}
		else
		{
			D3D11_BOX dataRegion;
			dataRegion.back = 1;
			dataRegion.front = 0;
			dataRegion.bottom = 1;
			dataRegion.top = 0;
			dataRegion.left = m_nBufferPos;
			dataRegion.right = dataRegion.left + m_nLockedVertices * m_nVertexSize;

			g_pD3DDeviceContext->UpdateSubresource(m_pVertexBuffer, NULL, &dataRegion, m_pVertexMemory + m_nBufferPos, 0, 0);
			m_nBufferPos += m_nLockedVertices * m_nVertexSize;
		}
	}

	m_bIsLocked = false;
}

void CVertexBufferDX11::DummyDataDesc(VertexDesc_t &desc)
{
	ComputeVertexDesc(0, 0, desc);
	desc.m_nFirstVertex = 0;
	desc.m_nOffset = 0;
}


// Spews the mesh data
void CVertexBufferDX11::Spew(int nVertexCount, const VertexDesc_t &desc)
{

}


// Call this in debug mode to make sure our data is good.
void CVertexBufferDX11::ValidateData(int nVertexCount, const VertexDesc_t & desc)
{

}

bool CVertexBufferDX11::Allocate()
{
	Assert(!m_pVertexBuffer);

	m_nBufferPos = 0;

	D3D11_BUFFER_DESC bufferDesc;

	if (m_bIsDynamic)
	{
		m_pVertexMemory = NULL;

		bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	}
	else
	{
		m_pVertexMemory = (unsigned char *)malloc(m_nBufferSize);
		memset(m_pVertexMemory, 0, m_nBufferSize);

		bufferDesc.Usage = D3D11_USAGE_DEFAULT;
		bufferDesc.CPUAccessFlags = 0;
	}

	bufferDesc.ByteWidth = m_nBufferSize;
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.MiscFlags = 0;

	HRESULT hr = g_pD3DDevice->CreateBuffer(&bufferDesc, NULL, &m_pVertexBuffer);

	return !(FAILED(hr)) && (m_pVertexBuffer != NULL);
}

void CVertexBufferDX11::Free()
{
	if (m_pVertexBuffer)
	{
		m_pVertexBuffer->Release();
		m_pVertexBuffer = NULL;
	}

	if (m_pVertexMemory)
	{
		free(m_pVertexMemory);
		m_pVertexMemory = NULL;
	}
}
