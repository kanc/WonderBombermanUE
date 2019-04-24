// Fill out your copyright notice in the Description page of Project Settings.

/*
--- Bomberman Test UE4 -------
--- Author: David Escalona ---
--- Date: 13/11/2017 ---------
*/

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "IKABombListener.h"
#include "IKACanPickup.h"
#include "Runtime/Engine/Classes/Components/PointLightComponent.h"
#include "Runtime/Engine/Classes/Sound/SoundCue.h"
#include "IKAPlayer.generated.h"

UENUM(BlueprintType)
enum class EPlayerProperty : uint8
{
	ESpeed = 0				UMETA(DisplayName = "Increase Speed"),
	EBombsLeft = 1			UMETA(DisplayName = "Bombs Left"),
	ELongBlast = 2			UMETA(DisplayName = "LongBlast"),
	ERemoteBombs = 3		UMETA(DisplayName = "Remote Bombs")
};

USTRUCT(BlueprintType)
struct FStepSound
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Step Sound")
	USoundCue* StepNature;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Step Sound")
	USoundCue* StepIce;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Step Sound")
	USoundCue* StepDesert;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Step Sound")
	USoundCue* StepCity;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Step Sound")
	USoundCue* StepDefault;

	FStepSound()
	{
		StepNature = nullptr;
		StepIce = nullptr;
		StepDesert = nullptr;
		StepCity = nullptr;
		StepDefault = nullptr;
	}

};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FUpdateUI, EPlayerProperty, PlayerProperty, int, value);

UCLASS()
class IKABOMBERMANUE_API AIKAPlayer : public ACharacter, public IIKABombListener, public IIKACanPickup
{
	GENERATED_BODY()

public:	
	AIKAPlayer();
	virtual void	Tick(float DeltaTime) override;
	virtual void	SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual float	TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser) override;
	
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Player")
	void PauseGameBPEvent(bool paused);

	UFUNCTION(BlueprintCallable, Category = "Player")
	void PauseGame();

	UFUNCTION(BlueprintCallable, Category = "Player")
	void SetPlayerInputEnabled(bool enabled);

	UFUNCTION(BlueprintCallable, Category = "Player")
	void PlayStepSound();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category ="Player")
	bool IsPlayerAlive();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Player")
	bool IsPlayerWinner();

	UFUNCTION(BlueprintCallable, Category = "Player")
	void SetPlayerWinner();
	
	// -- Bomb Listener Interface Implementation
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Bomb Listener")
	void BombExplodes(AActor* bomb);
	virtual void BombExplodes_Implementation(AActor* bomb) override;	


	// -- Pickup Actions Interface Implementation
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Pickup Action")
	void AddMoreBombs(int extra);
	virtual void AddMoreBombs_Implementation(int extra) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Pickup Action")
	void AddSpeedIncrement(float percentage);
	virtual void AddSpeedIncrement_Implementation(float percentage) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Pickup Action")
	void AddRemoteBombs(float enabledTime);
	virtual void AddRemoteBombs_Implementation(float enabledTime) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Pickup Action")
	void AddLongerBlasts(int TileIncrement);
	virtual void AddLongerBlasts_Implementation(int TileIncrement) override;	

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Pickup Action")
	void FreezePlayer(float time);
	virtual void FreezePlayer_Implementation(float time) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Pickup Action")
	void SlowdownPlayer(float percentage, float time);
	virtual void SlowdownPlayer_Implementation(float percentage, float time) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Pickup Action")
	void LightOnPlayer(float time);
	virtual void LightOnPlayer_Implementation(float time) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Pickup Action")
	void SwapLocation(FVector newLocation);
	virtual void SwapLocation_Implementation(FVector newLocation) override;

	//event to be captured on blueprints
	UPROPERTY(BlueprintAssignable, Category = "Player Event")
	FUpdateUI OnUpdatePlayerUI;

protected:	
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = "Player")
	UPointLightComponent* LightComp;

	UPROPERTY(EditDefaultsOnly, Category = "Player")
	UAnimMontage* DropBombMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Player")
	UMaterialInterface* FreezeMaterial;

	UPROPERTY(EditDefaultsOnly, Category = "Player")
	int InitialLives;

	UPROPERTY(EditDefaultsOnly, Category = "Player")
	int InitialBombs;

	UPROPERTY(EditDefaultsOnly, Category = "Player")
	USoundCue* DeathSound;

	UPROPERTY(EditDefaultsOnly, Category = "Player")
	FStepSound StepSounds;

private:
	void	MoveForward(float value);
	void	MoveRight(float value);
	void	DropBomb();	
	void	DisableTimedBombs();
	void	StopFreezing();
	void	StopSlowdown();
	void	StopLighting();
	void	PlayerDead();

	FVector GetTileCenterPosition();

	bool						m_bAcceptInput;
	bool						m_bRemoteBombs;
	bool						m_bIsWinner;
	int							m_iExtraBlastSize;	
	int							m_iCurrentLives;
	int							m_iMaxBombs;
	int							m_iBombsLeft;
	float						m_fInitialSpeed;
	float						m_LastSpeed;
	FTimerHandle				m_THRemoteBombs;
	FTimerHandle				m_THFreeze;
	FTimerHandle				m_THSlowdown;
	FTimerHandle				m_THLighting;
	class AIKAGameModeBase*		m_GM;
	class AIKABomb*				m_CurrentTimedBomb;
	TArray<UMaterialInterface*> m_PlayerMats;
	TArray<UMaterialInterface*> m_AddonsMats;
	TArray<UActorComponent*>	m_AddonMesh;
	class UIKABombGameInst*		m_GI;
			
};
