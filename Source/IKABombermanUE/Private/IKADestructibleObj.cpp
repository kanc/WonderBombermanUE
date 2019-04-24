// Fill out your copyright notice in the Description page of Project Settings.

/*
--- Bomberman Test UE4 -------
--- Author: David Escalona ---
--- Date: 13/11/2017 ---------
*/
#include "IKADestructibleObj.h"
#include "IKABombermanUE.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"


AIKADestructibleObj::AIKADestructibleObj()
{
	PrimaryActorTick.bCanEverTick = false;

	TriggerBoxCmp = CreateDefaultSubobject<UBoxComponent>(TEXT("ObjDetector"));
	SetRootComponent(TriggerBoxCmp);
	
	m_fBoxSize = 40.f;
}

void AIKADestructibleObj::Tick(float Delta)
{
	Super::Tick(Delta);
}

void AIKADestructibleObj::SetData(float size, AIKAMapManager* mapRef)
{
	m_fBoxSize = size;
	m_MapReference = mapRef;
}

void AIKADestructibleObj::BeginPlay()
{
	Super::BeginPlay();

	//update size
	TriggerBoxCmp->SetBoxExtent(FVector(m_fBoxSize, m_fBoxSize, m_fBoxSize), false);

	//set collision response
	TriggerBoxCmp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TriggerBoxCmp->SetCollisionResponseToAllChannels(ECR_Ignore);
	TriggerBoxCmp->SetCollisionResponseToChannel(TRACE_BOMB, ECollisionResponse::ECR_Overlap);

}

void AIKADestructibleObj::DestroyObj()
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("OBJ DESTRUCTED")));

	//spawn FX
	if (DestroyFX)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), DestroyFX, GetActorTransform());
	}

	if (DestroySound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, DestroySound, GetActorLocation());
	}

	Destroy();
}
