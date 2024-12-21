#ifndef EAE6320_CGAMEOBJECT_H
#define EAE6320_CGAMEOBJECT_H

// Includes
//=========

#include <Engine/Graphics/cMesh.h>
#include <Engine/Graphics/cEffect.h>
#include <Engine/Graphics/sLight.h>
#include <Engine/Physics/sRigidBodyState.h>

// Class Declaration
//==================

namespace eae6320
{
	class cGameObject
	{
	public:
		// Update
		void Update(const float i_elapsedSecondCount_sinceLastUpdate);

		// Getters
		Physics::sRigidBodyState& GetRigidBodyState() { return m_rigidBodyState; }
		Graphics::sMaterial& GetMaterial() { return m_material; }
		Graphics::cMesh* GetMesh() const { return m_mesh; }
		Graphics::LightingEffect* GetEffect() const { return m_lightEffect; }
		Graphics::DeferredRenderingEffect_Geometry* GetDeferredEffect() const { return m_deferredEffect; }

		// Setters
		void SetMesh(Graphics::cMesh* i_mesh) { m_mesh = i_mesh; }
		void SetEffect(Graphics::LightingEffect* i_effect) { m_lightEffect = i_effect; }
		void SetDeferredRenderingEffect(Graphics::DeferredRenderingEffect_Geometry* i_effect) { m_deferredEffect = i_effect; }
		void SetMaxVelocity(float i_maxV) { m_maxVelocity = i_maxV; }
		void SetMaterial(Graphics::sMaterial i_material) { m_material = i_material;  }
		void SetPosition(Math::sVector i_position) { m_rigidBodyState.position = i_position; }
		void SetOrientation(Math::cQuaternion i_orientation) { m_rigidBodyState.orientation = i_orientation; }

	private:
		Graphics::cMesh* m_mesh;
		Graphics::LightingEffect* m_lightEffect;
		Graphics::DeferredRenderingEffect_Geometry* m_deferredEffect;
		Physics::sRigidBodyState m_rigidBodyState;
		Graphics::sMaterial m_material;
		float m_maxVelocity = 2.0f;
	};
}


#endif	// EAE6320_CGAMEOBJECT_H
