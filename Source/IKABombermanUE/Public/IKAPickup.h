// Fill out yourcopyright notice in the Description page of Project Settings.

/*
--- Bomberman Test UE4 -------
--- Author: David Escalona ---
--- Date: 13/11/2017 ---------
*/

#pragma once


#include "CoreMinimal.h"
#include "IKADestructibleObj.h"
#include "IKAPickup.generated.h"

/**
 * 
 */
UCLASS()
class IKABOMBERMANUE_API AIKAPickup : public AIKADestructibleObj
{
	GENERATED_BODY()

public:
	
	AIKAPickup();

	virtual void BeginPlay() override;	
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Pickup Action")
	void PickupAction(AActor* actor);

protected:
	
	UPROPERTY(EditDefaultsOnly, Category = "Components")
	UStaticMeshComponent* PickupMesh;

	UPROPERTY(EditDefaultsOnly, Category = "FX")
	UParticleSystem* PickedFX;

	UPROPERTY(EditDefaultsOnly, Category = "FX")
	USoundCue* PickedSound;

	UPROPERTY(EditDefaultsOnly, Category = "Pickup")
	bool RotatePickup;
	
	UPROPERTY(EditDefaultsOnly, Category = "Pickup")
	FVector RotationValue;

private:
	bool m_AlreadyPicked;
	
};
