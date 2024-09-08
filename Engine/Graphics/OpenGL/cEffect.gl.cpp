// Includes
//=========

#include "../cEffect.h"
#include "../cShader.h"
#include "../sContext.h"
#include "../cRenderState.h"

#include <Engine/Asserts/Asserts.h>
#include <Engine/Logging/Logging.h>
#include <Engine/ScopeGuard/cScopeGuard.h>

eae6320::cResult eae6320::Graphics::cEffect::Initialize(const char* i_vertexShaderPath, const char* i_fragmentShaderPath, const uint8_t i_renderStateBits)
{
	auto result = eae6320::Results::Success;

	// Create the program
	EAE6320_ASSERT(m_programId == 0);
	m_programId = glCreateProgram();
	{
		const auto errorCode = glGetError();
		if (errorCode != GL_NO_ERROR)
		{
			result = eae6320::Results::Failure;
			EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
			eae6320::Logging::OutputError("OpenGL failed to create a program: %s", reinterpret_cast<const char*>(gluErrorString(errorCode)));
			return result;
		}
		else if (m_programId == 0)
		{
			result = eae6320::Results::Failure;
			EAE6320_ASSERT(false);
			eae6320::Logging::OutputError("OpenGL failed to create a program");
			return result;
		}
	}

	eae6320::cScopeGuard scopeGuard_program([&result, this]
	{
		if (!result && m_programId != 0)
		{
			glDeleteProgram(m_programId);
			const auto errorCode = glGetError();
			if (errorCode != GL_NO_ERROR)
			{
				EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
				eae6320::Logging::OutputError("OpenGL failed to delete the program: %s", reinterpret_cast<const char*>(gluErrorString(errorCode)));
			}
			m_programId = 0;
		}
	});

	// Load and Attach vertex shader
	GLuint vertexShaderId = 0;
	{
		cShader* vertexShader;
		if (!(result = eae6320::Graphics::cShader::Load(i_vertexShaderPath, vertexShader, eae6320::Graphics::eShaderType::Vertex)))
		{
			EAE6320_ASSERTF(false, "Can't initialize effect without vertex shader");
			glDeleteProgram(m_programId);
			return result;
		}
		vertexShaderId = vertexShader->m_shaderId;
		glAttachShader(m_programId, vertexShaderId);
		vertexShader->DecrementReferenceCount();  // Shader object is no longer needed
	}

	// Load and Attach fragment shader
	GLuint fragmentShaderId = 0;
	{
		cShader* fragmentShader;
		if (!(result = eae6320::Graphics::cShader::Load(i_fragmentShaderPath, fragmentShader, eae6320::Graphics::eShaderType::Fragment)))
		{
			EAE6320_ASSERTF(false, "Can't initialize effect without fragment shader");
			glDeleteShader(vertexShaderId);
			glDeleteProgram(m_programId);
			return result;
		}
		fragmentShaderId = fragmentShader->m_shaderId;
		glAttachShader(m_programId, fragmentShaderId);
		fragmentShader->DecrementReferenceCount();  // Shader object is no longer needed
	}

	// Link the program
	{
		glLinkProgram(m_programId);
		const auto errorCode = glGetError();
		if (errorCode == GL_NO_ERROR)
		{
			// Get link info
			// (this won't be used unless linking fails
			// but it can be useful to look at when debugging)
			std::string linkInfo;
			{
				GLint infoSize;
				glGetProgramiv(m_programId, GL_INFO_LOG_LENGTH, &infoSize);
				const auto errorCode = glGetError();
				if (errorCode == GL_NO_ERROR)
				{
					if (infoSize > 0)
					{
						auto* const info = new (std::nothrow) GLchar[infoSize];
						if (info)
						{
							eae6320::cScopeGuard scopeGuard_info([info]
								{
									delete[] info;
								});
							constexpr GLsizei* const dontReturnLength = nullptr;
							glGetProgramInfoLog(m_programId, static_cast<GLsizei>(infoSize), dontReturnLength, info);
							const auto errorCode = glGetError();
							if (errorCode == GL_NO_ERROR)
							{
								linkInfo = info;
							}
							else
							{
								result = eae6320::Results::Failure;
								EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
								eae6320::Logging::OutputError("OpenGL failed to get link info of the program: %s",
									reinterpret_cast<const char*>(gluErrorString(errorCode)));
								return result;
							}
						}
						else
						{
							result = eae6320::Results::OutOfMemory;
							EAE6320_ASSERTF(false, "Couldn't allocate memory for the program link info");
							eae6320::Logging::OutputError("Failed to allocate memory for the program link info");
							return result;
						}
					}
				}
				else
				{
					result = eae6320::Results::Failure;
					EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
					eae6320::Logging::OutputError("OpenGL failed to get the length of the program link info: %s",
						reinterpret_cast<const char*>(gluErrorString(errorCode)));
					return result;
				}
			}
			// Check to see if there were link errors
			GLint didLinkingSucceed;
			{
				glGetProgramiv(m_programId, GL_LINK_STATUS, &didLinkingSucceed);
				const auto errorCode = glGetError();
				if (errorCode == GL_NO_ERROR)
				{
					if (didLinkingSucceed == GL_FALSE)
					{
						result = eae6320::Results::Failure;
						EAE6320_ASSERTF(false, linkInfo.c_str());
						eae6320::Logging::OutputError("The program failed to link: %s",
							linkInfo.c_str());
						return result;
					}
				}
				else
				{
					result = eae6320::Results::Failure;
					EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
					eae6320::Logging::OutputError("OpenGL failed to find out if linking of the program succeeded: %s",
						reinterpret_cast<const char*>(gluErrorString(errorCode)));
					return result;
				}
			}
		}
		else
		{
			result = eae6320::Results::Failure;
			EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
			eae6320::Logging::OutputError("OpenGL failed to link the program: %s",
				reinterpret_cast<const char*>(gluErrorString(errorCode)));
			return result;
		}
	}

	// Delete the shaders as they are no longer needed after the program is linked
	glDeleteShader(vertexShaderId);
	glDeleteShader(fragmentShaderId);

	// Initialize Render State
	if (!(result = m_renderState.Initialize(i_renderStateBits)))
	{
		EAE6320_ASSERTF(false, "Can't initialize shading data without render state");
		glDeleteProgram(m_programId);
		m_programId = 0;
		return result;
	}

	return result;
}

void eae6320::Graphics::cEffect::Bind() const
{
	{
		EAE6320_ASSERT(m_programId != 0);
		glUseProgram(m_programId);
		EAE6320_ASSERT(glGetError() == GL_NO_ERROR);
	}
	// Render state
	{
		m_renderState.Bind();
	}
}

eae6320::cResult eae6320::Graphics::cEffect::CleanUp()
{
	auto result = eae6320::Results::Success;

	if (m_programId != 0)
	{
		glDeleteProgram(m_programId);
		const auto errorCode = glGetError();
		if (errorCode != GL_NO_ERROR)
		{
			if (result)
			{
				result = eae6320::Results::Failure;
			}
			EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
			eae6320::Logging::OutputError("OpenGL failed to delete the program: %s",
				reinterpret_cast<const char*>(gluErrorString(errorCode)));
		}
		m_programId = 0;
	}
	
	// TODO: Clean up Render State

	return result;
}