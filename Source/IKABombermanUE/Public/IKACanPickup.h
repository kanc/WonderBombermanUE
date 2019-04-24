// Fill out your copyright notice in the Description page of Project Settings.

/*
--- Bomberman Test UE4 -------
--- Author: David Escalona ---
--- Date: 13/11/2017 ---------
*/

#pragma once

#include "CoreMinimal.h"
#include "IKACanPickup.generated.h"

// This class does not need to be modified.
UINTERFACE(Blueprintable,MinimalAPI)
class UIKACanPickup : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class IKABOMBERMANUE_API IIKACanPickup
{
	GENERATED_BODY()
	
public:
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Pickup Action")
	void AddMoreBombs(int extra = 1);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Pickup Action")
	void AddSpeedIncrement(float percentage = 0.5f);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Pickup Action")
	void AddRemoteBombs(float enabledTime = 10.f);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Pickup Action")
	void AddLongerBlasts(int TileIncrement = 1);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Pickup Action")
	void FreezePlayer(float time = 3.0f);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Pickup Action")
	void SlowdownPlayer(float percentage = 0.5f, float time = 3.0f);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Pickup Action")
	void LightOnPlayer(float time = 5.0f);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Pickup Action")
	void SwapLocation(FVector newLocation);

	
};
