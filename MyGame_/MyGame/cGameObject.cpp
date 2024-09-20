#include "cGameObject.h"

void eae6320::cGameObject::Update(const float i_elapsedSecondCount_sinceLastUpdate)
{
    // Update the physics state
    if (m_rigidBodyState.acceleration == Math::sVector(0.0f, 0.0f, 0.0f))
    {
        if (m_rigidBodyState.velocity.GetLength() > 0) {
            m_rigidBodyState.acceleration = m_rigidBodyState.velocity.GetNormalized() * -1.0f;
        }
    }
    else if (m_rigidBodyState.velocity.GetLength() > m_maxVelocity)
    {
        m_rigidBodyState.acceleration = m_rigidBodyState.velocity.GetNormalized() * m_rigidBodyState.velocity.GetLength();
    }
    m_rigidBodyState.Update(i_elapsedSecondCount_sinceLastUpdate);
        
}