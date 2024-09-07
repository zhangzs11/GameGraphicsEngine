// Includes
//=========

#include "../cView.h"
#include "../sContext.h"

#include <Engine/Asserts/Asserts.h>
#include <Engine/Logging/Logging.h>

eae6320::cResult eae6320::Graphics::cView::Initialize(const sInitializationParameters& i_initializationParameters)
{
    auto result = eae6320::Results::Success;
    
    //glViewport(0, 0, static_cast<GLsizei>(i_resolutionWidth), static_cast<GLsizei>(i_resolutionHeight));
    //EAE6320_ASSERT(glGetError() == GL_NO_ERROR);

    return result;
}

void eae6320::Graphics::cView::Clear(const float i_clearColor[4])
{
	{
		glClearColor(i_clearColor[0], i_clearColor[1], i_clearColor[2], i_clearColor[3]);
		EAE6320_ASSERT(glGetError() == GL_NO_ERROR);
	}

	{
		constexpr GLbitfield clearColor = GL_COLOR_BUFFER_BIT;
		glClear(clearColor);
		EAE6320_ASSERT(glGetError() == GL_NO_ERROR);
	}

	{
		glDepthMask(GL_TRUE);
		EAE6320_ASSERT(glGetError() == GL_NO_ERROR);
		constexpr GLclampd clearToFarDepth = 1.0;
		glClearDepth(clearToFarDepth);
		EAE6320_ASSERT(glGetError() == GL_NO_ERROR);
	}
	{
		constexpr GLbitfield clearDepth = GL_DEPTH_BUFFER_BIT;
		glClear(clearDepth);
		EAE6320_ASSERT(glGetError() == GL_NO_ERROR);
	}
}

void eae6320::Graphics::cView::Bind()
{

}

eae6320::cResult eae6320::Graphics::cView::CleanUp()
{
	return eae6320::Results::Success;
}