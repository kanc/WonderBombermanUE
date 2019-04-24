// Fill out your copyright notice in the Description page of Project Settings.

/*
--- Bomberman Test UE4 -------
--- Author: David Escalona Rocha ---
--- Date: 13/11/2017 ---------
*/

#pragma once

#include "CoreMinimal.h"
#include "Runtime/Engine/Classes/Components/BoxComponent.h"
#include "GameFramework/Actor.h"
#include "IKAMapManager.h"
#include "Runtime/Engine/Classes/Sound/SoundCue.h"
#include "IKADestructibleObj.generated.h"

UCLASS()
class IKABOMBERMANUE_API AIKADestructibleObj : public AActor
{
	GENERATED_BODY()
	
public:
	AIKADestructibleObj();

	virtual void Tick(float Delta) override;

	UFUNCTION(BlueprintCallable, Category = "Destructible Obj")
	virtual void DestroyObj();

	void SetData(float size, AIKAMapManager* mapRef = nullptr);

protected:
	
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = "Component")
	UBoxComponent* TriggerBoxCmp;

	UPROPERTY(EditDefaultsOnly, Category = "FX")
	UParticleSystem* DestroyFX;

	UPROPERTY(EditDefaultsOnly, Category = "FX")
	USoundCue* DestroySound;

protected:
	
	float			m_fBoxSize;
	AIKAMapManager* m_MapReference;
};
