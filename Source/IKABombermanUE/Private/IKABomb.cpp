// Fill out your copyright notice in the Description page of Project Settings.

/*
 --- Bomberman Test UE4 -------
 --- Author: David Escalona ---
 --- Date: 13/11/2017 ---------
*/

#include "IKABomb.h"
#include "IKABombFactory.h"
#include "IKABombListener.h"

AIKABomb::AIKABomb()
{
	PrimaryActorTick.bCanEverTick = true;

	BombMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BombMesh"));
	BombMesh->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);	

	TimeToExplode = 1.f;
	ScaleIncreaseSpeed = 5.f;
	ScaleRange = 0.03f;
	m_bIsActive = false;
	InitialBlastLong = 3;
	m_fTileSize = 50.f;	
	m_BombOwner = nullptr;
}

void AIKABomb::BeginPlay()
{
	Super::BeginPlay();

	//set initial params
	m_vInitialScale = BombMesh->GetComponentTransform().GetScale3D();
	m_iCurrentBlastLong = InitialBlastLong;
	m_fSine = 0.f;
	m_bExploding = false;

	BombMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	BombMesh->SetVisibility(false, true);

	TriggerBoxCmp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AIKABomb::Tick(float Delta)
{
	Super::Tick(Delta);

	//increase bomb scale (visual effect)
	if (m_bIsActive)
	{
		m_fSine += Delta;
		float value = FMath::Sin(2 * m_fSine * ScaleIncreaseSpeed) * ScaleRange;

		FVector meshScale = BombMesh->GetRelativeTransform().GetScale3D() + FVector(value, value, value);
		BombMesh->SetRelativeScale3D(meshScale);
	}
}

void AIKABomb::SetPoolReference(UIKABombFactory* factory)
{
	m_PoolRef = factory;
}

bool AIKABomb::IsActive()
{
	return m_bIsActive;
}

void AIKABomb::EnableBomb(bool enable, bool timedBomb)
{
	m_bIsActive = enable;

	//we are enabling bomb
	if (m_bIsActive)
	{
		//set visible and traceable
		BombMesh->SetVisibility(true, true);
		TriggerBoxCmp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

		//is not a timed bomb, set timer to make it explode
		if (!timedBomb)
		{
			GetWorldTimerManager().SetTimer(m_THExplode, this, &AIKABomb::ExplodeBomb, TimeToExplode);
		}
	}
	else //disable bomb
	{
		//hide geometry, disable trace, reset scale and blastlong
		BombMesh->SetVisibility(false, true);
		TriggerBoxCmp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		BombMesh->SetRelativeScale3D(m_vInitialScale);
		m_iCurrentBlastLong = InitialBlastLong;
		m_BombOwner = nullptr;

		//add to bomb pool
		if (m_PoolRef)
		{
			m_PoolRef->AddBombToPool(this);
		}
	}

	//trigger event for blueprints
	EnableBombEvent(enable);
}

void AIKABomb::SetBombData(AActor* owner, float tileSize)
{
	m_BombOwner = owner;
	m_fTileSize = tileSize;
}

void AIKABomb::AddBlastSize(int extraLong)
{
	m_iCurrentBlastLong += extraLong;
}

void AIKABomb::DestroyObj()
{
	if (m_bExploding) return;

	//disable timer
	GetWorldTimerManager().ClearTimer(m_THExplode);

	//explode bomb
	ExplodeBomb();
	
}

void AIKABomb::TraceBombBlast(FVector direction)
{
	TArray<FHitResult> Hits;

	FCollisionQueryParams TraceParams;
	TraceParams.AddIgnoredActor(this);

	//line trace using current bomb blast distance in the param direction
	GetWorld()->LineTraceMultiByChannel(Hits, GetActorLocation(), GetActorLocation() + direction * m_iCurrentBlastLong * m_fTileSize, TRACE_BOMB, TraceParams);

	//initial blast number
	int totalblast = m_iCurrentBlastLong;

	for (int i = 0; i < Hits.Num(); ++i)
	{
		//overlaping actors (destructible walls, pickups, player, other bombs)
		if (!Hits[i].bBlockingHit)
		{
			//apply damage (for players)
			UGameplayStatics::ApplyPointDamage(Hits[i].GetActor(), 1, direction, Hits[i], m_BombOwner->GetInstigatorController(), this, UDamageType::StaticClass());
			
			//if it's a destructible object, do it
			AIKADestructibleObj* destructible = Cast<AIKADestructibleObj>(Hits[i].GetActor());
			if (destructible)
			{
				destructible->DestroyObj();
			}
		}
		else //trace blocked, non-destructive wall
		{
			//calculate total blast depending distance
			totalblast = (Hits[i].ImpactPoint - GetActorLocation()).Size() / m_fTileSize;
		}
	}

	//spawn blast FX
	FVector BlasftFXLocation = GetActorLocation();
	for (int i = 0; i < totalblast; ++i)
	{
		BlasftFXLocation += direction * m_fTileSize;
		if (BombBlastFX)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BombBlastFX, FTransform(BlasftFXLocation));
		}
	}

	if (BlastSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, BlastSound, GetActorLocation());
	}	
}

void AIKABomb::BombStolen()
{
	if (m_bExploding || !m_bIsActive) return;

	//disable timers
	GetWorldTimerManager().ClearAllTimersForObject(this);

	if (m_BombOwner)
	{
		//check if owner implements the interface
		IIKABombListener* interf = Cast<IIKABombListener>(m_BombOwner);
		if (interf)
		{
			//call interface method
			interf->Execute_BombExplodes(m_BombOwner, this);
		}
	}

	//disable bomb
	EnableBomb(false);

}

void AIKABomb::ExplodeBomb()
{
	if (m_bExploding || !m_bIsActive) return;

	m_bExploding = true;

	//spawn bomb location blast
	if (BombBlastFX)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BombBlastFX, FTransform(GetActorLocation()));
	}

	//check vertical and horizontal blasts
	TraceBombBlast(GetActorForwardVector());
	TraceBombBlast(GetActorForwardVector() * - 1);
	TraceBombBlast(GetActorRightVector());
	TraceBombBlast(GetActorRightVector() * -1);

	//tell bomb's owner that it's exploded
	if (m_BombOwner)
	{
		//check if owner implements the interface
		IIKABombListener* interf = Cast<IIKABombListener>(m_BombOwner);
		if (interf)
		{
			//call interface method
			interf->Execute_BombExplodes(m_BombOwner, this);
		}
	}

	//disable bomb
	EnableBomb(false);

	m_bExploding = false;
}
