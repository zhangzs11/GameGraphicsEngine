// Includes
//=========

#include <fstream>

#include "cMyGame.h"

#include <Engine/Asserts/Asserts.h>
#include <Engine/UserInput/UserInput.h>

std::ofstream logFile;

// Inherited Implementation
//=========================

// Run
//----

void eae6320::cMyGame::UpdateBasedOnInput()
{
	// Is the user pressing the ESC key?
	if ( UserInput::IsKeyPressed( UserInput::KeyCodes::Escape ) )
	{
		// Exit the application
		const auto result = Exit( EXIT_SUCCESS );
		EAE6320_ASSERT( result );
	}
	if (UserInput::IsKeyPressed(UserInput::KeyCodes::Space))
	{
		SetSimulationRate(5.0f);
	}
	else
	{                
		SetSimulationRate(1.0f);
	}
}

// Initialize / Clean Up
//----------------------

eae6320::cResult eae6320::cMyGame::Initialize()
{
	logFile.open("game_log.txt", std::ios::out | std::ios::trunc);
	if (logFile.is_open())
	{
		logFile << GetMainWindowName() << " Initialization Started\n";
	}
	return Results::Success;
}

eae6320::cResult eae6320::cMyGame::CleanUp()
{
	if (logFile.is_open())
	{
		logFile << GetMainWindowName() << " Clean Up Completed\n";
		logFile.close();
	}
	return Results::Success;
}
