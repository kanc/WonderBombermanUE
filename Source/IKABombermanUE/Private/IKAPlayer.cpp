// Fill out your copyright notice in the Description page of Project Settings.

/*
--- Bomberman Test UE4 -------
--- Author: David Escalona ---
--- Date: 13/11/2017 ---------
*/

#include "IKAPlayer.h"
#include "Runtime/Engine/Classes/GameFramework/CharacterMovementComponent.h"
#include "Runtime/Engine/Classes/Engine/LocalPlayer.h"
#include "IKABomb.h"
#include "IKABombGameInst.h"
#include "IKAGameModeBase.h"


// Sets default values
AIKAPlayer::AIKAPlayer()
{
 	PrimaryActorTick.bCanEverTick = true;

	LightComp = CreateDefaultSubobject<UPointLightComponent>(TEXT("Light"));
	LightComp->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

	LightComp->SetRelativeLocation(FVector(0.f, 0.f, 140.f));
	LightComp->AttenuationRadius = 400.f;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	bUseControllerRotationYaw = false;

	InitialBombs = 1;
	InitialLives = 1;
	m_iExtraBlastSize = 0;
	m_bRemoteBombs = false;
}

// Called when the game starts or when spawned
void AIKAPlayer::BeginPlay()
{
	Super::BeginPlay();
	
	LightComp->SetVisibility(false);

	//set responses
	GetCapsuleComponent()->SetCollisionObjectType(COLLISION_PLAYER); 
	GetCapsuleComponent()->SetCollisionResponseToChannel(COLLISION_PICKUP, ECollisionResponse::ECR_Overlap);
	GetCapsuleComponent()->SetCollisionResponseToChannel(TRACE_BOMB, ECR_Overlap);

	//get gamemode
	m_GM = Cast<AIKAGameModeBase>(GetWorld()->GetAuthGameMode());
	//set gameinstance
	m_GI = Cast<UIKABombGameInst>(UGameplayStatics::GetGameInstance(GetWorld()));

	//set initial params
	m_bAcceptInput = true;	
	m_bIsWinner = false;
	m_iMaxBombs = InitialBombs;
	m_iBombsLeft = InitialBombs;
	m_iCurrentLives = InitialLives;
	m_fInitialSpeed = GetCharacterMovement()->MaxWalkSpeed;

	m_PlayerMats = GetMesh()->GetMaterials();	
	m_AddonMesh = GetComponentsByClass(UStaticMeshComponent::StaticClass());

	if (m_AddonMesh.Num() > 0)
	{
		if (Cast<UStaticMeshComponent>(m_AddonMesh[0]))
		{
			m_AddonsMats = Cast<UStaticMeshComponent>(m_AddonMesh[0])->GetMaterials();
		}
	}

}

// Called every frame
void AIKAPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AIKAPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	APlayerController* pc = Cast<APlayerController>(GetController());

	if (pc)
	{
		//bind inputs to the correct player
		switch (pc->GetLocalPlayer()->GetControllerId())
		{
			case 0:
			{
				PlayerInputComponent->BindAxis("P1_Forward", this, &AIKAPlayer::MoveForward);
				PlayerInputComponent->BindAxis("P1_Right", this, &AIKAPlayer::MoveRight);
				PlayerInputComponent->BindAction("P1_Bomb", IE_Pressed, this, &AIKAPlayer::DropBomb);
				PlayerInputComponent->BindAction("PauseGame", IE_Pressed, this, &AIKAPlayer::PauseGame).bExecuteWhenPaused = true;
				break;
			}
			case 1:
			{
				PlayerInputComponent->BindAxis("P2_Forward", this, &AIKAPlayer::MoveForward);
				PlayerInputComponent->BindAxis("P2_Right", this, &AIKAPlayer::MoveRight);
				PlayerInputComponent->BindAction("P2_Bomb", IE_Pressed, this, &AIKAPlayer::DropBomb);
				break;
			}
		}
	}
}

void AIKAPlayer::PauseGame()
{
	if (!m_GM) return;

	if (m_GM->GetGameplayState() == EGameplayState::EPlaying) 
	{
		UGameplayStatics::SetGamePaused(GetWorld(), true);
		PauseGameBPEvent(true);
		m_GM->SetGameplayState(EGameplayState::EPaused);
	}
	else if (m_GM->GetGameplayState() == EGameplayState::EPaused)
	{
		UGameplayStatics::SetGamePaused(GetWorld(), false);
		PauseGameBPEvent(false);
		m_GM->SetGameplayState(EGameplayState::EPlaying);
	}
}


void AIKAPlayer::BombExplodes_Implementation(AActor* bomb)
{
	//increment bombs count and update ui
	m_iBombsLeft = FMath::Clamp(m_iBombsLeft + 1, 0, m_iMaxBombs);
	OnUpdatePlayerUI.Broadcast(EPlayerProperty::EBombsLeft, m_iBombsLeft);
}

void AIKAPlayer::SetPlayerInputEnabled(bool enabled)
{
	m_bAcceptInput = enabled;
}

void AIKAPlayer::MoveForward(float value)
{
	if (!m_bAcceptInput) return;

	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);

	const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	AddMovementInput(Direction, value);	
}

void AIKAPlayer::MoveRight(float value)
{
	if (!m_bAcceptInput) return;

	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);

	const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	AddMovementInput(Direction, value);	
}

void AIKAPlayer::DropBomb()
{
	if (m_bAcceptInput && m_GM )
	{
		//we have remote switch, dont create a new bomb, just explodes the current one
		if (m_bRemoteBombs && m_CurrentTimedBomb)
		{
			m_CurrentTimedBomb->ExplodeBomb();
			m_CurrentTimedBomb = nullptr;
			return;
		}
		//we have enough bombs
		else if (m_iBombsLeft > 0)
		{
			//get bomb from factory
			AIKABomb* bomb = m_GM->CreateBomb(this);
			if (bomb)
			{
				//set the bomb location and specs
				bomb->SetActorLocation(GetTileCenterPosition(), false, nullptr, ETeleportType::TeleportPhysics);
				bomb->AddBlastSize(m_iExtraBlastSize);
				bomb->EnableBomb(true, m_bRemoteBombs);

				//if we have the remote switch, save the current bomb
				m_CurrentTimedBomb = m_bRemoteBombs ? bomb : nullptr;
				
				//decrease bombs amount
				m_iBombsLeft = FMath::Clamp(m_iBombsLeft - 1, 0, m_iMaxBombs);

				//update ui
				OnUpdatePlayerUI.Broadcast(EPlayerProperty::EBombsLeft, m_iBombsLeft);

				//play bomb drop animation
				if (DropBombMontage)
				{
					PlayAnimMontage(DropBombMontage);
				}
			}
		}
	}
}

FVector AIKAPlayer::GetTileCenterPosition()
{
	FVector pos = GetActorLocation();
	FHitResult Hit(ForceInit);

	//ray trace to get the current player floor tile and get its worldspace position to spawn the bomb on the center
	if (GetWorld()->LineTraceSingleByChannel(Hit, GetActorLocation(), GetActorLocation() - GetActorUpVector() * 500.f, TRACE_FLOOR))
	{
		if (m_GM)
		{
			//add the tile size to avoid spawn the bomb inside the floor
			pos = m_GM->GetMapTilePosition(Hit.Item, ETileType::EFloor);
			pos.Z += m_GM->GetMapTileSize();
		}
	}

	return pos;
}

float AIKAPlayer::TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser)
{		
	const float ActualDamage = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
		
	if (ActualDamage > 0.f && IsPlayerAlive() && !m_bIsWinner)
	{
		//apply damage
		m_iCurrentLives -= ActualDamage;
		
		//we are dead
		if (m_iCurrentLives <= 0.f)
		{
			if (m_GM)
			{
				//get our current player id and trigger event to gamemode blueprint to check win conditions
				APlayerController* pc = Cast<APlayerController>(GetController());
				int playerId = UGameplayStatics::GetPlayerControllerID(pc);

				m_GM->PlayerDeadBy(playerId, DamageCauser);
			}
			PlayerDead();
		}
	}

	return ActualDamage;
}

void AIKAPlayer::DisableTimedBombs()
{
	//disable remote bombs
	m_bRemoteBombs = false;

	//update ui
	OnUpdatePlayerUI.Broadcast(EPlayerProperty::ERemoteBombs, 0);

	//if the current bomb has no exploded yet, do it now :D
	if (m_CurrentTimedBomb)
	{
		m_CurrentTimedBomb->ExplodeBomb();
		m_CurrentTimedBomb = nullptr;
	}
}

void AIKAPlayer::AddMoreBombs_Implementation(int extra)
{
	//increase bombs amount and update ui
	m_iMaxBombs += extra;
	m_iBombsLeft = FMath::Clamp(m_iBombsLeft + extra, 0, m_iMaxBombs);
	
	OnUpdatePlayerUI.Broadcast(EPlayerProperty::EBombsLeft, m_iBombsLeft);
}

void AIKAPlayer::AddSpeedIncrement_Implementation(float percentage)
{
	//increase speed over initial speed not over the accumulated one
	if (GetCharacterMovement()->MaxWalkSpeed >= m_fInitialSpeed)
	{
		GetCharacterMovement()->MaxWalkSpeed = m_fInitialSpeed + (m_fInitialSpeed * percentage);
	}
	else //current velocity is less than initial, we are affected by slowdown item
	{
		GetCharacterMovement()->MaxWalkSpeed += GetCharacterMovement()->MaxWalkSpeed * percentage;
		
		//save speed to be restored with the speed increase after the slowdown effect
		m_LastSpeed = m_fInitialSpeed + (m_fInitialSpeed * percentage);
	}
	
	int enable = (GetCharacterMovement()->MaxWalkSpeed > m_fInitialSpeed) ? 1 : 0;
	OnUpdatePlayerUI.Broadcast(EPlayerProperty::ESpeed, enable);
}

void AIKAPlayer::AddRemoteBombs_Implementation(float enabledTime)
{
	//enable remote bombs and update ui
	m_bRemoteBombs = true;
	GetWorldTimerManager().SetTimer(m_THRemoteBombs, this, &AIKAPlayer::DisableTimedBombs, enabledTime);

	OnUpdatePlayerUI.Broadcast(EPlayerProperty::ERemoteBombs, enabledTime);
}

void AIKAPlayer::AddLongerBlasts_Implementation(int TileIncrement)
{
	//increment bombs blast distance and update ui
	m_iExtraBlastSize = TileIncrement;

	int enable = (m_iExtraBlastSize > 0) ? 1 : 0;	
	OnUpdatePlayerUI.Broadcast(EPlayerProperty::ELongBlast, enable);

}

bool AIKAPlayer::IsPlayerAlive()
{
	return m_iCurrentLives > 0;
}

void AIKAPlayer::PlayerDead()
{
	m_bAcceptInput = false;

	if (DeathSound)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), DeathSound, GetActorLocation());
	}
}

bool AIKAPlayer::IsPlayerWinner()
{
	return m_bIsWinner;
}

void AIKAPlayer::SetPlayerWinner()
{
	m_bIsWinner = true;
}

void AIKAPlayer::FreezePlayer_Implementation(float time)
{
	SetPlayerInputEnabled(false);

	GetCharacterMovement()->StopMovementImmediately();

	TArray<UActorComponent*> Meshes;

	for (int i = 0; i < m_PlayerMats.Num(); ++i)
	{
		GetMesh()->SetMaterial(i, FreezeMaterial);
	}

	if (m_AddonMesh.Num() > 0)
	{
		UStaticMeshComponent* cmp = Cast<UStaticMeshComponent>(m_AddonMesh[0]);
		if (cmp)
		{
			for (int i = 0; i < cmp->GetMaterials().Num(); ++i)
			{
				cmp->SetMaterial(i, FreezeMaterial);
			}
		}
	}

	GetWorldTimerManager().SetTimer(m_THFreeze, this, &AIKAPlayer::StopFreezing, time);
}

void AIKAPlayer::SlowdownPlayer_Implementation(float percentage, float time)
{	
	m_LastSpeed = GetCharacterMovement()->MaxWalkSpeed;
	GetCharacterMovement()->MaxWalkSpeed = m_fInitialSpeed - (m_fInitialSpeed * percentage);

	GetWorldTimerManager().SetTimer(m_THSlowdown, this, &AIKAPlayer::StopSlowdown, time);
}

void AIKAPlayer::LightOnPlayer_Implementation(float time)
{
	LightComp->SetVisibility(true);
	GetWorldTimerManager().SetTimer(m_THSlowdown, this, &AIKAPlayer::StopLighting, time);
}

void AIKAPlayer::StopFreezing()
{
	for (int i = 0; i < m_PlayerMats.Num(); ++i)
	{
		GetMesh()->SetMaterial(i, m_PlayerMats[i]);
	}

	if (m_AddonMesh.Num() > 0)
	{
		UStaticMeshComponent* cmp = Cast<UStaticMeshComponent>(m_AddonMesh[0]);
		if (cmp)
		{
			for (int i = 0; i < cmp->GetMaterials().Num(); ++i)
			{
				cmp->SetMaterial(i, m_AddonsMats[i]);
			}
		}
	}

	SetPlayerInputEnabled(true);
}

void AIKAPlayer::StopSlowdown()
{
	GetCharacterMovement()->MaxWalkSpeed = m_LastSpeed;
}

void AIKAPlayer::StopLighting()
{
	LightComp->SetVisibility(false);
}

void AIKAPlayer::SwapLocation_Implementation(FVector newLocation)
{
	SetActorLocation(newLocation);
}

void AIKAPlayer::PlayStepSound()
{
	if (m_GI)
	{
		USoundCue* sound;
		switch (m_GI->MapType)
		{
			case EMapType::ENature:  sound = StepSounds.StepNature; break;
			case EMapType::ECity:  sound = StepSounds.StepCity; break;
			case EMapType::EDesert:  sound = StepSounds.StepDesert; break;
			case EMapType::EIce:  sound = StepSounds.StepIce; break;
			default: sound = StepSounds.StepDefault; break;
		}

		if (sound)
		{
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), sound, GetActorLocation());
		}
	}
}