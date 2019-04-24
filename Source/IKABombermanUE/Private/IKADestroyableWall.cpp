// Fill out your copyright notice in the Description page of Project Settings.

/*
--- Bomberman Test UE4 -------
--- Author: David Escalona ---
--- Date: 13/11/2017 ---------
*/
#include "IKADestroyableWall.h"
#include "IKAPickup.h"
#include "IKABombUtils.h"


void AIKADestroyableWall::DestroyObj()
{
	//spawn pickups
	if (PickupsToSpawn.Num() > 0)
	{
		//check probability
		if (UIKABombUtils::CheckProbability(PickupChance, nullptr, false))
		{
			//get a random pickup from the array (populated in blueprints)
			int index = FMath::RandRange(0, PickupsToSpawn.Num() - 1);
			GetWorld()->SpawnActor<AIKAPickup>(PickupsToSpawn[index].Get(), GetActorTransform());
		}
	}

	//remove tile
	FHitResult Hit;
	FCollisionQueryParams TraceParams;
	TraceParams.AddIgnoredActor(this);

	//line trace from the top of the tile in order to get it the instanced mesh which represent the logic destructible and remove it
	if (GetWorld()->LineTraceSingleByChannel(Hit, GetActorLocation() + GetActorUpVector() * 200.f, GetActorLocation() - GetActorUpVector() * 200.f, TRACE_DESTROYABLE, TraceParams))
	{
		if (m_MapReference)
		{
			m_MapReference->RemoveMapDestructibleTileMesh(Hit.Item);
		}
	}

	Super::DestroyObj();
}

