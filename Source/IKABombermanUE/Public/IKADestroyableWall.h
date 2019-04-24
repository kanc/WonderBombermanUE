// Fill out your copyright notice in the Description page of Project Settings.

/*
--- Bomberman Test UE4 -------
--- Author: David Escalona ---
--- Date: 13/11/2017 ---------
*/

#pragma once

#include "CoreMinimal.h"
#include "IKADestructibleObj.h"
#include "IKADestroyableWall.generated.h"

/**
 * 
 */
UCLASS()
class IKABOMBERMANUE_API AIKADestroyableWall : public AIKADestructibleObj
{
	GENERATED_BODY()
	
public:
	virtual void DestroyObj() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Destroyable")
	float PickupChance = 0.3f;

protected:
	
	UPROPERTY(EditDefaultsOnly, Category = "Destroyable")
	TArray<TSubclassOf<class AIKAPickup>> PickupsToSpawn;

	
	
};
