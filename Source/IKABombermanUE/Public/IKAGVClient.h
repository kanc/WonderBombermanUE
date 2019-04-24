// Fill out your copyright notice in the Description page of Project Settings.

/*
--- Bomberman Test UE4 -------
--- Author: David Escalona ---
--- Date: 13/11/2017 ---------
*/

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameViewportClient.h"
#include "IKAGVClient.generated.h"

/**
 * 
 */
UCLASS()
class IKABOMBERMANUE_API UIKAGVClient : public UGameViewportClient
{
	GENERATED_BODY()

		virtual bool InputKey(const FInputKeyEventArgs& EventArgs) override;
};
