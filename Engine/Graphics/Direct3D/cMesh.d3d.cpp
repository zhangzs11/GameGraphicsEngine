// Includes
//=========

#include "../cMesh.h"

#include "Includes.h"
#include "../sContext.h"
#include "../cVertexFormat.h"
#include "../VertexFormats.h"

#include <Engine/Asserts/Asserts.h>
#include <Engine/Logging/Logging.h>

eae6320::cResult eae6320::Graphics::cMesh::Initialize(const void* i_vertexData, const uint16_t i_vertexCount)
{
	auto result = eae6320::Results::Success;

	m_vertexCount = static_cast<unsigned int>(i_vertexCount);
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
	// Render triangles from the currently-bound vertex buffer
	{
		EAE6320_ASSERT(m_vertexCount > 0);
		direct3dImmediateContext->Draw(m_vertexCount, m_indexOfFirstVertexToRender);
	}

}

eae6320::cResult eae6320::Graphics::cMesh::CleanUp()
{
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