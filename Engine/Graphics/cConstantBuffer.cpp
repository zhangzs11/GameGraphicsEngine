// Includes
//=========

#include "cConstantBuffer.h"

#include "ConstantBufferFormats.h"

#include <Engine/Asserts/Asserts.h>
#include <Engine/Logging/Logging.h>

// Interface
//==========

// Initialize / Clean Up
//----------------------

eae6320::cResult eae6320::Graphics::cConstantBuffer::Initialize( const void* const i_initialData )
{
	auto result = Results::Success;

	if ( m_type < ConstantBufferTypes::Count )
	{
		// Find the size of the type's struct
		{
			switch ( m_type )
			{
			case ConstantBufferTypes::Frame:
			{
				switch (m_effectType)
				{
				case eConstantBufferEffectType::Light:
					m_size = sizeof(ConstantBufferFormats::sLight_Frame); break;
				case eConstantBufferEffectType::Shadow:
					m_size = sizeof(ConstantBufferFormats::sShadow_Frame); break;
				case eConstantBufferEffectType::FXAA:
					m_size = sizeof(ConstantBufferFormats::sFXAA_Frame); break;
				}
				break;
			}
					

//				case ConstantBufferTypes::Material: m_size = sizeof( ConstantBufferFormats::sMaterial ); break;
			case ConstantBufferTypes::DrawCall:
			{
				switch (m_effectType)
				{
				case eConstantBufferEffectType::Light:
					m_size = sizeof(ConstantBufferFormats::sDrawCall); break;
				case eConstantBufferEffectType::Shadow:
					m_size = sizeof(ConstantBufferFormats::sDrawCall); break;
				case eConstantBufferEffectType::FXAA:
					m_size = sizeof(ConstantBufferFormats::sDrawCall); break;
				}
				break;
			}

			// This should never happen
			default:
				result = Results::Failure;
				EAE6320_ASSERTF( false, "Unrecognized constant buffer type %u", m_type );
				Logging::OutputError( "The size couldn't be calculated for a constant buffer of type %u", m_type );
				return result;
			}
			EAE6320_ASSERT( m_size > 0 );
		}
		// Initialize the platform-specific constant buffer
		{
			result = Initialize_platformSpecific( i_initialData );
			EAE6320_ASSERT( result );
		}
	}
	else
	{
		result = Results::Failure;
		EAE6320_ASSERTF( false, "Invalid constant buffer type %u", m_type );
		Logging::OutputError( "A constant buffer is being initialized with the invalid type %u", m_type );
		return result;
	}

	return result;
}

eae6320::Graphics::cConstantBuffer::cConstantBuffer( const ConstantBufferTypes i_type, const eConstantBufferEffectType i_effectType)
	:
	m_type( i_type ), m_effectType(i_effectType)
{

}

eae6320::Graphics::cConstantBuffer::~cConstantBuffer()
{
	const auto result = CleanUp();
	EAE6320_ASSERT( result );
}
