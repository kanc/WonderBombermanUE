// Fill out your copyright notice in the Description page of Project Settings.

/*
--- Bomberman Test UE4 -------
--- Author: David Escalona ---
--- Date: 13/11/2017 ---------
*/

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Runtime/Core/Public/Containers/Queue.h"
#include "IKABombFactory.generated.h"

/**
 * 
 */
UCLASS()
class IKABOMBERMANUE_API UIKABombFactory : public UObject
{
	GENERATED_BODY()
	
public:
	UIKABombFactory();

	void			Init(TSubclassOf<class AIKABomb> bombTemplate, const FTransform& InitialTransform, UWorld* WorldRef);
	class AIKABomb* GetBomb();
	void			AddBombToPool(class AIKABomb* bomb);

private:
	TQueue <class AIKABomb*>	m_BombsPool;
	TSubclassOf<class AIKABomb> m_BombTemplate;
	FTransform					m_SpawnTrans;
	UWorld*						m_WorldRef;
	
	
};
