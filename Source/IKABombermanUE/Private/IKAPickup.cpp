// Fill out your copyright notice in the Description page of Project Settings.

/*
--- Bomberman Test UE4 -------
--- Author: David Escalona ---
--- Date: 13/11/2017 ---------
*/

#include "IKAPickup.h"
#include "IKACanPickup.h"


AIKAPickup::AIKAPickup()
{
	PrimaryActorTick.bCanEverTick = true;

	PickupMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PickupMesh"));
	PickupMesh->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	
	RotatePickup = true;
	RotationValue = FVector(0.f, 50.f, 0.f);
}

void AIKAPickup::BeginPlay()
{
	Super::BeginPlay();

	PickupMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	TriggerBoxCmp->OnComponentBeginOverlap.AddDynamic(this, &AIKAPickup::OnOverlapBegin);
	TriggerBoxCmp->SetCollisionObjectType(COLLISION_PICKUP);
	TriggerBoxCmp->SetCollisionResponseToChannel(COLLISION_PLAYER, ECollisionResponse::ECR_Overlap);
}

void AIKAPickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (RotatePickup)
	{
		FVector rotInc = RotationValue * DeltaTime;		
		AddActorLocalRotation(FRotator(rotInc.X, rotInc.Y, rotInc.Z));
	}
}

void AIKAPickup::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("PICKUP OVERLAP")));

	//pick up just once
	if (!m_AlreadyPicked)
	{
		//check if actor implements interface to pickup objects
		IIKACanPickup* canPick = Cast<IIKACanPickup>(OtherActor);
		if (canPick)
		{
			m_AlreadyPicked = true;

			//call blueprint implementable event
			PickupAction(OtherActor);

			//trigger FX
			if (PickedFX)
			{
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), PickedFX, GetActorTransform());
			}
			if (PickedSound)
			{
				UGameplayStatics::PlaySoundAtLocation(GetWorld(), PickedSound, GetActorLocation());
			}
			
			SetLifeSpan(0.1f);
		}
	}
}

