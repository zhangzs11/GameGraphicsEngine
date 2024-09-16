#include "cGameObject.h"

void eae6320::cGameObject::Update(const float i_elapsedSecondCount_sinceLastUpdate)
{
    // Update the physics state
    m_rigidBodyState.Update(i_elapsedSecondCount_sinceLastUpdate);
}