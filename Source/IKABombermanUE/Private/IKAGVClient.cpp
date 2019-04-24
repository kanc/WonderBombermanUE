// Fill out your copyright notice in the Description page of Project Settings.

/*
--- Bomberman Test UE4 -------
--- Author: David Escalona ---
--- Date: 13/11/2017 ---------
*/

#include "IKAGVClient.h"
#include "Engine.h"

bool UIKAGVClient::InputKey(const FInputKeyEventArgs& EventArgs)
{
	
	if (IgnoreInput() || EventArgs.IsGamepad() || EventArgs.Key.IsMouseButton())
	{
		return Super::InputKey(EventArgs);
	}
	else
	{
		// Propagate keyboard events to all players
		UEngine* const Engine = GetOuterUEngine();
		int32 const NumPlayers = Engine ? Engine->GetNumGamePlayers(this) : 0;
		bool bRetVal = Super::InputKey(EventArgs);

		for (int32 i = 1; i < NumPlayers; i++)
		{
			FInputKeyEventArgs test(EventArgs);
			test.ControllerId = i;

			bRetVal = Super::InputKey(test);
		}

		return bRetVal;
	}
}

