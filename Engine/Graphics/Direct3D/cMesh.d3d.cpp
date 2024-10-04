// Includes
//=========

#include "../cMesh.h"

#include "Includes.h"
#include "../sContext.h"
#include "../cVertexFormat.h"
#include "../VertexFormats.h"

#include <Engine/Asserts/Asserts.h>
#include <Engine/Logging/Logging.h>
#include <vector>

// Helper Declarations
//====================

namespace
{
	void ConvertIndicesToCCW(uint16_t* io_indexData, size_t i_indexCount);
}

eae6320::cResult eae6320::Graphics::cMesh::Initialize(const void* i_vertexData, const uint16_t i_vertexCount,
													  const uint16_t* i_indexData, const uint16_t i_indexCount)
{
	auto result = eae6320::Results::Success;

	// Adjust the index to CCW
	std::vector<uint16_t> adjustedIndices(i_indexData, i_indexData + i_indexCount);
	ConvertIndicesToCCW(adjustedIndices.data(), adjustedIndices.size());
	i_indexData = adjustedIndices.data();
	
	// m_vertexCount = static_cast<unsigned int>(i_vertexCount);
	m_indexCount = static_cast<unsigned int>(i_indexCount);
	m_indexOfFirstVertexToRender = 0;

	auto* const direct3dDevice = eae6320::Graphics::sContext::g_context.direct3dDevice;
	EAE6320_ASSERT(direct3dDevice);

	// Vertex Format
	{
		if (!(result = eae6320::Graphics::cVertexFormat::Load(eae6320::Graphics::eVertexType::Mesh, m_vertexFormat,
			"data/Shaders/Vertex/vertexInputLayout_mesh.shader")))
		{
			EAE6320_ASSERTF(false, "Can't initialize geometry without vertex format");
			return result;
		}
	}
	// Vertex Buffer
	{
		const auto bufferSize = static_cast<UINT>(i_vertexCount * sizeof(eae6320::Graphics::VertexFormats::sVertex_mesh));
		EAE6320_ASSERT(bufferSize <= std::numeric_limits<decltype(D3D11_BUFFER_DESC::ByteWidth)>::max());

		D3D11_BUFFER_DESC bufferDescription{};
		bufferDescription.ByteWidth = bufferSize;
		bufferDescription.Usage = D3D11_USAGE_IMMUTABLE;
		bufferDescription.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bufferDescription.CPUAccessFlags = 0;
		bufferDescription.MiscFlags = 0;
		bufferDescription.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA initialData{};
		initialData.pSysMem = i_vertexData;

		const auto result_create = direct3dDevice->CreateBuffer(&bufferDescription, &initialData, &m_vertexBuffer);
		if (FAILED(result_create))
		{
			result = eae6320::Results::Failure;
			EAE6320_ASSERTF(false, "3D object vertex buffer creation failed (HRESULT %#010x)", result_create);
			eae6320::Logging::OutputError("Direct3D failed to create a vertex buffer (HRESULT %#010x)", result_create);
			return result;
		}
	}
	// Index Buffer
	{
		const auto bufferSize = static_cast<UINT>(i_indexCount * sizeof(uint16_t));
		EAE6320_ASSERT(bufferSize <= std::numeric_limits<decltype(D3D11_BUFFER_DESC::ByteWidth)>::max());

		D3D11_BUFFER_DESC bufferDescription{};
		bufferDescription.ByteWidth = bufferSize;
		bufferDescription.Usage = D3D11_USAGE_IMMUTABLE;
		bufferDescription.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bufferDescription.CPUAccessFlags = 0;
		bufferDescription.MiscFlags = 0;
		bufferDescription.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA initialData{};
		initialData.pSysMem = i_indexData;

		const auto result_create = direct3dDevice->CreateBuffer(&bufferDescription, &initialData, &m_indexBuffer);
		if (FAILED(result_create))
		{
			result = eae6320::Results::Failure;
			EAE6320_ASSERTF(false, "3D object index buffer creation failed (HRESULT %#010x)", result_create);
			eae6320::Logging::OutputError("Direct3D failed to create an index buffer (HRESULT %#010x)", result_create);
			return result;
		}
	}

	return result;
}

void eae6320::Graphics::cMesh::Draw() const
{
	auto* const direct3dImmediateContext = sContext::g_context.direct3dImmediateContext;
	EAE6320_ASSERT(direct3dImmediateContext);

	// Bind a vertex buffer
	{
		EAE6320_ASSERT(m_vertexBuffer != nullptr);
		constexpr unsigned int startingSlot = 0;
		constexpr unsigned int vertexBufferCount = 1;
		// The "stride" defines how large a single vertex is in the stream of data
		constexpr unsigned int bufferStride = sizeof(eae6320::Graphics::VertexFormats::sVertex_mesh);
		// It's possible to start streaming data in the middle of a vertex buffer
		constexpr unsigned int bufferOffset = 0;
		direct3dImmediateContext->IASetVertexBuffers(startingSlot, vertexBufferCount, &m_vertexBuffer, &bufferStride, &bufferOffset);
	}
	// Bind the vertex format
	// Set the topology
	{
		EAE6320_ASSERT(m_vertexFormat != nullptr);
		m_vertexFormat->Bind();
		// the vertex buffer was defined as a triangle list
		// (meaning that every primitive is a triangle and will be defined by three vertices)
		direct3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	}
	// Bind the index buffer
	{
		EAE6320_ASSERT(m_indexBuffer != nullptr);
		direct3dImmediateContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R16_UINT, 0);
	}
	// Render triangles from the currently-bound indexed buffer
	{
		EAE6320_ASSERT(m_indexCount > 0);
		direct3dImmediateContext->DrawIndexed(m_indexCount, m_indexOfFirstVertexToRender, 0);
	}
}

eae6320::cResult eae6320::Graphics::cMesh::CleanUp()
{
	if (m_indexBuffer)
	{
		m_indexBuffer->Release();
		m_indexBuffer = nullptr;
	}
	if (m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = nullptr;
	}
	if (m_vertexFormat)
	{
		m_vertexFormat->DecrementReferenceCount();
		m_vertexFormat = nullptr;
	}

	return Results::Success;
}

// Helper Definitions
//===================

namespace
{
	void ConvertIndicesToCCW(uint16_t* io_indexData, size_t i_indexCount)
	{
		EAE6320_ASSERT(i_indexCount % 3 == 0);  // index count must be 3 times

		for (size_t i = 0; i < i_indexCount; i += 3)
		{
			std::swap(io_indexData[i + 1], io_indexData[i + 2]);  // swap every 2 after indexes of triangle
		}
	}
}