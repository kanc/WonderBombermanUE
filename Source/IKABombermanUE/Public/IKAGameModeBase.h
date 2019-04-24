// Fill out your copyright notice in the Description page of Project Settings.

/*
--- Bomberman Test UE4 -------
--- Author: David Escalona ---
--- Date: 13/11/2017 ---------
*/

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "IKAMapManager.h"
#include "IKAGameModeBase.generated.h"


UENUM(BlueprintType)
enum class EGameplayState : uint8
{
	EStarting = 0		UMETA(DisplayName = "Starting"),
	EPaused = 1			UMETA(DisplayName = "Paused"),
	EPlaying = 2		UMETA(DisplayName = "Playing"),
	EFinished = 3		UMETA(DisplayName = "Finished")
};

UCLASS()
class IKABOMBERMANUE_API AIKAGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:

	AIKAGameModeBase();

	virtual void BeginPlay() override;
	
	UFUNCTION(BlueprintCallable, Category = "IKA Bomberman GM")
	class AIKABomb* CreateBomb(AActor* BombOwner);

	UFUNCTION(BlueprintCallable, Category = "IKA Bomberman GM")
	void SetGameplayState(EGameplayState state);

	UFUNCTION(BlueprintCallable, Category = "IKA Bomberman GM")
	EGameplayState GetGameplayState();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "IKA Bomberman GM")
	void MapReady();	

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "IKA Bomberman GM")
	void PlayerDeadBy(int PlayerId, AActor* DeathCauser);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "IKA Bomberman GM")
	void TimeOver();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "IKA Bomberman GM")
	void ReadyMessage();	

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "IKA Bomberman GM")
	void GameStarted();

	void	SetMapReference(AIKAMapManager* map);
	FVector GetMapTilePosition(int instance, ETileType type);
	float	GetMapTileSize();

	FORCEINLINE class AIKAMapManager* const GetMapRef() { return MapReference; }

	
protected:

	UPROPERTY(EditDefaultsOnly, Category = "IKA Bomberman GM")
	TSubclassOf<class AIKABomb> BombTemplate;
	
	UPROPERTY(BlueprintReadOnly, Category = "IKA Bomberman GM")
	class AIKAMapManager*  MapReference;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "IKA Bomberman GM")
	float GameTime;

	UPROPERTY(EditDefaultsOnly, Category = "IKA Bomberman GM")
	float StartDelay;

	UPROPERTY(BlueprintReadWrite, Category = "IKA Bomberman GM")
	FTimerHandle GameTimerHandle;

	UPROPERTY(BlueprintReadWrite, Category = "IKA Bomberman GM")
	TArray<class AIKAPlayer*>	CurrentPlayers;

	//just to get a reference to avoid garbage collection
	UPROPERTY()
	class UIKABombFactory* m_BombFactory;	
			
private:

	EGameplayState m_State;
	
};
