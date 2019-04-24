// Fill out your copyright notice in the Description page of Project Settings.

/*
--- Bomberman Test UE4 -------
--- Author: David Escalona ---
--- Date: 13/11/2017 ---------
*/

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "IKABombGameInst.generated.h"

UENUM(BlueprintType)
enum class EMapType : uint8
{
	ENature = 0				UMETA(DisplayName = "Nature Map"),
	EIce = 1				UMETA(DisplayName = "Iced Map"),
	EDesert = 2				UMETA(DisplayName = "Desert Map"),
	ECity = 3				UMETA(DisplayName = "City Map")
};

UCLASS()
class IKABOMBERMANUE_API UIKABombGameInst : public UGameInstance
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Play options")
	bool AutoMap = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Play options")
	EMapType MapType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Play options")
	int NumCols = 25;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Play options")
	int NumRows = 20;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Play options")
	float PickupPercentChance = 0.35f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Play options")
	float GameTime = 120.f;
	
	
};
