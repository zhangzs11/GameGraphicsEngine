#ifndef EAE6320_CGAMEOBJECT_H
#define EAE6320_CGAMEOBJECT_H

// Includes
//=========

#include <Engine/Graphics/cMesh.h>
#include <Engine/Graphics/cEffect.h>
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
		Graphics::cMesh* GetMesh() const { return m_mesh; }
		Graphics::cEffect* GetEffect() const { return m_effect; }

		// Setters
		void SetMesh(Graphics::cMesh* i_mesh) { m_mesh = i_mesh; }
		void SetEffect(Graphics::cEffect* i_effect) { m_effect = i_effect; }
		void SetMaxVelocity(float i_maxV) { m_maxVelocity = i_maxV; }

	private:
		Graphics::cMesh* m_mesh;
		Graphics::cEffect* m_effect;
		Physics::sRigidBodyState m_rigidBodyState;
		float m_maxVelocity;
	};
}


#endif	// EAE6320_CGAMEOBJECT_H
