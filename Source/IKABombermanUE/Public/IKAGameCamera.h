// Fill out your copyright notice in the Description page of Project Settings.

/*
--- Bomberman Test UE4 -------
--- Author: David Escalona ---
--- Date: 13/11/2017 ---------
*/

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "IKAGameCamera.generated.h"

UCLASS()
class IKABOMBERMANUE_API AIKAGameCamera : public AActor
{
	GENERATED_BODY()
	
public:	
	
	AIKAGameCamera();
	virtual void Tick(float DeltaTime) override;


	UFUNCTION(BlueprintCallable, Category ="IKA Game Camera")
	void StartWorking();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IKA Game Camera")
	float MinArmLenght;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IKA Game Camera")
	float MaxArmLenght;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IKA Game Camera")
	bool UseDynamicMaxArm = true;
	
	//range for (30 - 50) rows or cols, the maximum
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IKA Game Camera")
	FVector2D MaxArmLenghtRange = FVector2D(1800, 3700);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IKA Game Camera")
	float ZoomSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IKA Game Camera")
	float FollowSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IKA Game Camera")
	float HeightOffset;


protected:

	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = "Components")
	USceneComponent* RootCmp;

	UPROPERTY(EditDefaultsOnly, Category = "Components")
	class USpringArmComponent* SpringArmCmp;

	UPROPERTY(EditDefaultsOnly, Category = "Components")
	class UCameraComponent* CameraCmp;


private:
	
	void	SetAsMainCamera();
	void	GetPlayers();	
	float	GetArmLengthAndComputeTarget();

	FVector					m_TargetPoint;
	TArray<APawn*>			m_Players;
	class AIKAGameModeBase* m_GM;
	bool					m_bCameraReady;
	FTimerHandle			m_THCamera;
	
	
	
};
