// Fill out your copyright notice in the Description page of Project Settings.

/*
--- Bomberman Test UE4 -------
--- Author: David Escalona ---
--- Date: 13/11/2017 ---------
*/

#include "IKABombFactory.h"
#include "IKABomb.h"

UIKABombFactory::UIKABombFactory()
{
	m_BombTemplate = nullptr;
	m_SpawnTrans = FTransform();
	m_WorldRef = nullptr;
}

void UIKABombFactory::Init(TSubclassOf<class AIKABomb> bombTemplate, const FTransform& InitialTransform, UWorld* world)
{
	m_BombTemplate = bombTemplate;
	m_SpawnTrans = InitialTransform;
	m_WorldRef = world; //no actor class, we need world reference
}

AIKABomb* UIKABombFactory::GetBomb()
{
	if (!m_BombTemplate)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("BOMB TEMPLATE NULL - CHECK IKAGAMEMODE BP")));
		return nullptr;
	}

	AIKABomb* bomb = nullptr;

	//no bombs in the pool, create a new one
	if (m_BombsPool.IsEmpty())
	{
		bomb = m_WorldRef->SpawnActor<AIKABomb>(m_BombTemplate, m_SpawnTrans);
		bomb->SetPoolReference(this);
	}
	else
	{
		//deque a bomb from pool
		m_BombsPool.Dequeue(bomb);
	}
	return bomb;
}

void UIKABombFactory::AddBombToPool(class AIKABomb* bomb)
{
	m_BombsPool.Enqueue(bomb);
}
