// Fill out your copyright notice in the Description page of Project Settings.

/*
--- Bomberman Test UE4 -------
--- Author: David Escalona ---
--- Date: 13/11/2017 ---------
*/

#include "IKAGameCamera.h"
#include "Runtime/Engine/Classes/Camera/CameraComponent.h"
#include "Runtime/Engine/Classes/GameFramework/SpringArmComponent.h"
#include "IKAPlayer.h"
#include "IKAGameModeBase.h"
#include "IKAMapManager.h"


AIKAGameCamera::AIKAGameCamera()
{
	PrimaryActorTick.bCanEverTick = true;

	RootCmp = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(RootCmp);

	SpringArmCmp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArmCmp->AttachToComponent(RootCmp, FAttachmentTransformRules::KeepRelativeTransform);

	CameraCmp = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraCmp->AttachToComponent(SpringArmCmp, FAttachmentTransformRules::KeepRelativeTransform);

	MinArmLenght = 800.f;
	MaxArmLenght = 2000.f;
	ZoomSpeed = 1.f;
	FollowSpeed = 2.f;
	HeightOffset = 200.f;

	SpringArmCmp->TargetArmLength = MaxArmLenght + MinArmLenght / 2;

}

void AIKAGameCamera::BeginPlay()
{
	Super::BeginPlay();

	m_bCameraReady = false;
	m_GM = Cast<AIKAGameModeBase>(GetWorld()->GetAuthGameMode());

	//just in case that StartWorking function were not called from GameMode when Map and Players are ready
	GetWorldTimerManager().SetTimer(m_THCamera, this, &AIKAGameCamera::StartWorking, 1.f);	
}

void AIKAGameCamera::StartWorking()
{
	if (m_bCameraReady) return;

	GetPlayers();
	SetAsMainCamera();

	if (UseDynamicMaxArm)
	{
		//FMath::Max<int>(m_GM->GetMapRef()->GetMapCols(), m_GM->GetMapRef()->GetMapRows());
		int maxSize = m_GM->GetMapRef()->GetMapRows();
		MaxArmLenght = FMath::GetMappedRangeValueClamped(FVector2D(20, 40), MaxArmLenghtRange, maxSize);
	}

	m_bCameraReady = true;
}

void AIKAGameCamera::SetAsMainCamera()
{
	APlayerController* pc = UGameplayStatics::GetPlayerController(this, 0);

	if (pc)
	{
		pc->SetViewTarget(this);
	}
}

void AIKAGameCamera::GetPlayers()
{
	if (!m_GM)
	{
		m_GM = Cast<AIKAGameModeBase>(GetWorld()->GetAuthGameMode());
	}
	
	//get current players 
	int totalPlayers = m_GM->GetNumPlayers();

	for (int i = 0; i < totalPlayers; ++i)
	{
		APawn* player = UGameplayStatics::GetPlayerPawn(GetWorld(), i);
		if (player)
		{
			m_Players.Add(player);
		}
	}

}
float AIKAGameCamera::GetArmLengthAndComputeTarget()
{
	float maxDistance = 0;

	m_TargetPoint = GetActorLocation();

	//get the long distance between players (thinking of more than two players)
	for (int i = 0; i < m_Players.Num(); ++i)
	{
		for (int j = 1; j < m_Players.Num(); ++j)
		{
			if (i != j)
			{
				float tempDist = (m_Players[i]->GetActorLocation() - m_Players[j]->GetActorLocation()).Size();

				//longest distance, compute mid point between players
				if (tempDist > maxDistance)
				{
					m_TargetPoint = (m_Players[i]->GetActorLocation() + m_Players[j]->GetActorLocation()) / 2;
					maxDistance = tempDist;
				}
			}
		}
	}

	//add and offset, just to improve behavior
	maxDistance  += HeightOffset;
	
	return FMath::Clamp(maxDistance, MinArmLenght, MaxArmLenght);
}


void AIKAGameCamera::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//move camera to mid point between players and set the spring arm length depending of distance between them
	if (m_bCameraReady)
	{
		SpringArmCmp->TargetArmLength = FMath::FInterpTo(SpringArmCmp->TargetArmLength, GetArmLengthAndComputeTarget(), DeltaTime, ZoomSpeed);
		SetActorLocation(FMath::VInterpTo(GetActorLocation(), m_TargetPoint, DeltaTime, FollowSpeed));
	}

}

