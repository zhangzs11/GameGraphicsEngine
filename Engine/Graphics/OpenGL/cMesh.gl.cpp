// Includes
//=========

#include "../cMesh.h"
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
	// std::vector<uint16_t> adjustedIndices(i_indexData, i_indexData + i_indexCount);
	// ConvertIndicesToCCW(adjustedIndices.data(), adjustedIndices.size());
	// i_indexData = adjustedIndices.data();

	// Store the vertex count
	// m_vertexCount = i_vertexCount;
	m_indexCount = i_indexCount;

	// Create a vertex array object (VAO) and make it active
	{
		constexpr GLsizei arrayCount = 1;
		glGenVertexArrays(arrayCount, &m_vertexArrayId);
		const auto errorCode = glGetError();
		if (errorCode == GL_NO_ERROR)
		{
			// Bind VAO
			glBindVertexArray(m_vertexArrayId);
			const auto errorCode = glGetError();
			if (errorCode != GL_NO_ERROR)
			{
				result = eae6320::Results::Failure;
				EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
				eae6320::Logging::OutputError("OpenGL failed to bind a new vertex array: %s",
					reinterpret_cast<const char*>(gluErrorString(errorCode)));
				return result;
			}
		}
		else
		{
			result = eae6320::Results::Failure;
			EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
			eae6320::Logging::OutputError("OpenGL failed to get an unused vertex array ID: %s",
				reinterpret_cast<const char*>(gluErrorString(errorCode)));
			return result;
		}
	}

	// Create a vertex buffer object (VBO) and make it active
	{
		constexpr GLsizei bufferCount = 1;
		glGenBuffers(bufferCount, &m_vertexBufferId);
		const auto errorCode = glGetError();
		if (errorCode == GL_NO_ERROR)
		{
			// Bind VBO
			glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferId);
			const auto errorCode = glGetError();
			if (errorCode != GL_NO_ERROR)
			{
				result = eae6320::Results::Failure;
				EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
				eae6320::Logging::OutputError("OpenGL failed to bind a new vertex buffer: %s",
					reinterpret_cast<const char*>(gluErrorString(errorCode)));
				return result;
			}
		}
		else
		{
			result = eae6320::Results::Failure;
			EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
			eae6320::Logging::OutputError("OpenGL failed to get an unused vertex buffer ID: %s",
				reinterpret_cast<const char*>(gluErrorString(errorCode)));
			return result;
		}
	}

	// Assign the vertex data to the buffer
	{
		const auto bufferSize = static_cast<GLsizeiptr>(i_vertexCount * sizeof(eae6320::Graphics::VertexFormats::sVertex_mesh));
		EAE6320_ASSERT(static_cast<GLsizeiptr>(bufferSize) <= std::numeric_limits<GLsizeiptr>::max());
		glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(bufferSize), i_vertexData, GL_STATIC_DRAW);
		const auto errorCode = glGetError();
		if (errorCode != GL_NO_ERROR)
		{
			result = eae6320::Results::Failure;
			EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
			eae6320::Logging::OutputError("OpenGL failed to allocate the vertex buffer: %s",
				reinterpret_cast<const char*>(gluErrorString(errorCode)));
			return result;
		}
	}

	// Create a Index Buffer
	{
		constexpr GLsizei bufferCount = 1;
		glGenBuffers(bufferCount, &m_indexBufferId);
		const auto errorCode = glGetError();
		if (errorCode == GL_NO_ERROR)
		{
			// Bind IBO
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBufferId);
			const auto errorCode = glGetError();
			if (errorCode != GL_NO_ERROR)
			{
				result = eae6320::Results::Failure;
				EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
				eae6320::Logging::OutputError("OpenGL failed to bind a new index buffer: %s",
					reinterpret_cast<const char*>(gluErrorString(errorCode)));
				return result;
			}
		}
		else
		{
			result = eae6320::Results::Failure;
			EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
			eae6320::Logging::OutputError("OpenGL failed to get an unused index buffer ID: %s",
				reinterpret_cast<const char*>(gluErrorString(errorCode)));
			return result;
		}
	}
	// Assign the Index data to the buffer
	{
		const auto bufferSize = static_cast<GLsizeiptr>(i_indexCount * sizeof(uint16_t));
		EAE6320_ASSERT(static_cast<GLsizeiptr>(bufferSize) <= std::numeric_limits<GLsizeiptr>::max());
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(bufferSize), i_indexData, GL_STATIC_DRAW);
		const auto errorCode = glGetError();
		if (errorCode != GL_NO_ERROR)
		{
			result = eae6320::Results::Failure;
			EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
			eae6320::Logging::OutputError("OpenGL failed to allocate the index buffer: %s",
				reinterpret_cast<const char*>(gluErrorString(errorCode)));
			return result;
		}
	}
	// Initialize vertex format
	{
		constexpr auto stride = static_cast<GLsizei>(sizeof(eae6320::Graphics::VertexFormats::sVertex_mesh));

		// Position (0)
		// 3 floats == 12 bytes
		// Offset = 0
		{
			constexpr GLuint vertexElementLocation = 0;
			constexpr GLint elementCount = 3;
			constexpr GLboolean notNormalized = GL_FALSE;  // The given floats should be used as-is
			glVertexAttribPointer(vertexElementLocation, elementCount, GL_FLOAT, notNormalized, stride,
				reinterpret_cast<GLvoid*>(offsetof(eae6320::Graphics::VertexFormats::sVertex_mesh, x)));
			const auto errorCode = glGetError();
			if (errorCode == GL_NO_ERROR)
			{
				glEnableVertexAttribArray(vertexElementLocation);
				const GLenum errorCode = glGetError();
				if (errorCode != GL_NO_ERROR)
				{
					result = eae6320::Results::Failure;
					EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
					eae6320::Logging::OutputError("OpenGL failed to enable the POSITION vertex attribute at location %u: %s",
						vertexElementLocation, reinterpret_cast<const char*>(gluErrorString(errorCode)));
					return result;
				}
			}
			else
			{
				result = eae6320::Results::Failure;
				EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
				eae6320::Logging::OutputError("OpenGL failed to set the POSITION vertex attribute at location %u: %s",
					vertexElementLocation, reinterpret_cast<const char*>(gluErrorString(errorCode)));
				return result;
			}
		}

		// COLOR (1)
		// 4 uint8 == 4 bytes
		{
			constexpr GLuint vertexElementLocation = 1;
			constexpr GLint elementCount = 4;
			constexpr GLboolean needNormalized = GL_TRUE;  // The given floats should be used as-is
			glVertexAttribPointer(vertexElementLocation, elementCount, GL_UNSIGNED_BYTE, needNormalized, stride,
				reinterpret_cast<GLvoid*>(offsetof(eae6320::Graphics::VertexFormats::sVertex_mesh, r)));
			const auto errorCode = glGetError();
			if (errorCode == GL_NO_ERROR)
			{
				glEnableVertexAttribArray(vertexElementLocation);
				const GLenum errorCode = glGetError();
				if (errorCode != GL_NO_ERROR)
				{
					result = eae6320::Results::Failure;
					EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
					eae6320::Logging::OutputError("OpenGL failed to enable the POSITION vertex attribute at location %u: %s",
						vertexElementLocation, reinterpret_cast<const char*>(gluErrorString(errorCode)));
					return result;
				}
			}
			else
			{
				result = eae6320::Results::Failure;
				EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
				eae6320::Logging::OutputError("OpenGL failed to set the POSITION vertex attribute at location %u: %s",
					vertexElementLocation, reinterpret_cast<const char*>(gluErrorString(errorCode)));
				return result;
			}
		}
	}

	return result;
}

void eae6320::Graphics::cMesh::Draw() const
{
	// Make sure VAO exist
	EAE6320_ASSERT(m_vertexArrayId != 0);

	// Bind VAO
	glBindVertexArray(m_vertexArrayId);
	EAE6320_ASSERT(glGetError() == GL_NO_ERROR);

	// Draw
	{
		constexpr GLenum mode = GL_TRIANGLES;
		EAE6320_ASSERT(m_indexCount > 0);
		glDrawElements(mode, static_cast<GLsizei>(m_indexCount), GL_UNSIGNED_SHORT, nullptr);
		EAE6320_ASSERT(glGetError() == GL_NO_ERROR);
	}
}

eae6320::cResult eae6320::Graphics::cMesh::CleanUp()
{
	auto result = eae6320::Results::Success;

	// Clean up the VAO
	if (m_vertexArrayId != 0)
	{
		// Make sure the VAO isn't bound
		{
			// Unbind the VAO
			glBindVertexArray( 0 );
			const auto errorCode = glGetError();
			if (errorCode != GL_NO_ERROR)
			{
				if (result)
				{
					result = eae6320::Results::Failure;
				}
				EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
				eae6320::Logging::OutputError("OpenGL failed to unbind all vertex arrays before cleaning up geometry: %s",
					reinterpret_cast<const char*>(gluErrorString(errorCode)));
			}
		}

		// Delete the VAO
		constexpr GLsizei arrayCount = 1;
		glDeleteVertexArrays(arrayCount, &m_vertexArrayId);
		const auto errorCode = glGetError();
		if (errorCode != GL_NO_ERROR)
		{
			if (result)
			{
				result = eae6320::Results::Failure;
			}
			EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
			eae6320::Logging::OutputError("OpenGL failed to delete the vertex array: %s",
				reinterpret_cast<const char*>(gluErrorString(errorCode)));
		}

		m_vertexArrayId = 0;	// Reset to 0 after deletion
	}

	// Clean up the VBO
	if (m_vertexBufferId != 0)
	{
		constexpr GLsizei bufferCount = 1;
		glDeleteBuffers(bufferCount, &m_vertexBufferId);
		const auto errorCode = glGetError();
		if (errorCode != GL_NO_ERROR)
		{
			if (result)
			{
				result = eae6320::Results::Failure;
			}
			EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
			eae6320::Logging::OutputError("OpenGL failed to delete the vertex buffer: %s",
				reinterpret_cast<const char*>(gluErrorString(errorCode)));
		}
		m_vertexBufferId = 0;  // Reset to 0 after deletion
	}

	// Clean up the IBO
	if (m_indexBufferId != 0)
	{
		constexpr GLsizei bufferCount = 1;
		glDeleteBuffers(bufferCount, &m_indexBufferId);
		const auto errorCode = glGetError();
		if (errorCode != GL_NO_ERROR)
		{
			if (result)
			{
				result = eae6320::Results::Failure;
			}
			EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
			eae6320::Logging::OutputError("OpenGL failed to delete the index buffer: %s",
				reinterpret_cast<const char*>(gluErrorString(errorCode)));
		}
		m_indexBufferId = 0;  // Reset to 0 after deletion
	}

	return result;
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