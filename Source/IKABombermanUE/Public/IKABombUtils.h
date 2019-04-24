// Fill out your copyright notice in the Description page of Project Settings.

/*
--- Bomberman Test UE4 -------
--- Author: David Escalona ---
--- Date: 13/11/2017 ---------
*/

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "IKABombUtils.generated.h"

/**
 * 
 */
UCLASS()
class IKABOMBERMANUE_API UIKABombUtils : public UObject
{
	GENERATED_BODY()

public:
	
	UFUNCTION(BlueprintCallable, Category = "Utils")
	static void FindScreenEdgeLocationForWorldLocation(UObject* WorldContextObject, const FVector& InLocation, const float EdgePercent, FVector2D& OutScreenPosition, float& OutRotationAngleDegrees, bool &bIsOnScreen);

	UFUNCTION(BlueprintCallable, Category = "Utils")
	static bool CheckProbability(float chance);
	
	static bool YesChance(int percentage);
	static bool CheckProbability(float chance, const FRandomStream* seed, bool useSeed);

};
