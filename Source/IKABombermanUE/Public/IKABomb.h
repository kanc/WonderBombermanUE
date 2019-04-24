// Fill out your copyright notice in the Description page of Project Settings.

/*
--- Bomberman Test UE4 -------
--- Author: David Escalona ---
--- Date: 13/11/2017 ---------
*/

#pragma once

#include "CoreMinimal.h"
#include "IKADestructibleObj.h"
#include "IKABomb.generated.h"

/**
 * 
 */
UCLASS()
class IKABOMBERMANUE_API AIKABomb : public AIKADestructibleObj
{
	GENERATED_BODY()

public:

	AIKABomb();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Bomb")
	void EnableBombEvent(bool enabled);

	UFUNCTION(BlueprintCallable, Category = "Bomb")
	void BombStolen();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Bomb")
	bool IsActive();

	void EnableBomb(bool enable, bool timedBomb = false);
	void SetPoolReference(class UIKABombFactory* factory);
	void SetBombData(AActor* owner, float TileSize);
	void AddBlastSize(int extraLong);	
	void ExplodeBomb();

	virtual void DestroyObj() override;
	virtual void BeginPlay() override;
	virtual void Tick(float Delta) override;

protected:

	UPROPERTY(EditDefaultsOnly, Category ="Components")
	UStaticMeshComponent* BombMesh;

	UPROPERTY(EditDefaultsOnly, Category = "FX")
	UParticleSystem* BombBlastFX;

	UPROPERTY(EditDefaultsOnly, Category = "FX")
	USoundCue* BlastSound;

	UPROPERTY(EditDefaultsOnly, Category = "Bomb")
	float TimeToExplode;

	UPROPERTY(EditDefaultsOnly, Category = "Bomb")
	int InitialBlastLong;

	UPROPERTY(EditDefaultsOnly, Category = "Bomb")
	float ScaleIncreaseSpeed;

	UPROPERTY(EditDefaultsOnly, Category = "Bomb")
	float ScaleRange;

	void TraceBombBlast(FVector direction);

private:

	bool					m_bExploding;
	bool					m_bIsActive;
	class UIKABombFactory*	m_PoolRef;
	int						m_iCurrentBlastLong;
	float					m_fTileSize;
	float					m_fSine;
	AActor*					m_BombOwner;
	FTimerHandle			m_THExplode;
	FVector					m_vInitialScale;	
	
};
