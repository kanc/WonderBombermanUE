// Fill out your copyright notice in the Description page of Project Settings.

/*
--- Bomberman Test UE4 -------
--- Author: David Escalona ---
--- Date: 13/11/2017 ---------
*/

#include "IKAGameModeBase.h"
#include "IKABombFactory.h"
#include "IKABombGameInst.h"
#include "IKABomb.h"

AIKAGameModeBase::AIKAGameModeBase()
{
	bStartPlayersAsSpectators = true;
	StartDelay = 3.f;
	GameTime = 120.f;
}

void AIKAGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	UIKABombGameInst* gi = Cast<UIKABombGameInst>(UGameplayStatics::GetGameInstance(GetWorld()));

	//get time from game instance (instanced from UI or randomly got)
	if (gi)
	{
		GameTime = gi->GameTime;
	}

	m_State = EGameplayState::EStarting;

	if (StartDelay > 0)
	{
		//trigger event to show ready message in blueprints
		GetWorldTimerManager().SetTimer(GameTimerHandle, this, &AIKAGameModeBase::ReadyMessage, StartDelay / 2);
	}
	else
	{
		//not enough delay time, go to play
		GameStarted();
	}
}

void AIKAGameModeBase::GameStarted_Implementation()
{
	m_State = EGameplayState::EPlaying;
	GetWorldTimerManager().SetTimer(GameTimerHandle, this, &AIKAGameModeBase::TimeOver, GameTime);
}

void AIKAGameModeBase::ReadyMessage_Implementation()
{
	GetWorldTimerManager().SetTimer(GameTimerHandle, this, &AIKAGameModeBase::GameStarted, StartDelay / 2);
}

//manage the bombs factory from gamemode
AIKABomb* AIKAGameModeBase::CreateBomb(AActor* BombOwner)
{
	//not created, do it
	if (!m_BombFactory)
	{
		m_BombFactory = NewObject<UIKABombFactory>();
		m_BombFactory->Init(BombTemplate, MapReference->GetActorTransform(), GetWorld());
	}

	//get a bomb from factory
	AIKABomb* bomb = m_BombFactory->GetBomb();

	//set data needed
	if (bomb)
	{
		bomb->SetBombData(BombOwner, MapReference->GetMapTileSize());
	}

	return  bomb;
}

void AIKAGameModeBase::SetMapReference(AIKAMapManager* map)
{
	MapReference = map;
	MapReady();
}

FVector AIKAGameModeBase::GetMapTilePosition(int instance, ETileType type)
{
	return (MapReference) ? MapReference->GetTilePosition(instance, type) : FVector::ZeroVector;
}

float AIKAGameModeBase::GetMapTileSize()
{
	return (MapReference) ? MapReference->GetMapTileSize() : 0;	
}

void AIKAGameModeBase::SetGameplayState(EGameplayState state) 
{
	m_State = state; 
}

EGameplayState AIKAGameModeBase::GetGameplayState() 
{
	return m_State; 
}