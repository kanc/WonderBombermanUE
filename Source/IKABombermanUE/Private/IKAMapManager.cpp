// Fill out your copyright notice in the Description page of Project Settings.

/*
--- Bomberman Test UE4 -------
--- Author: David Escalona ---
--- Date: 13/11/2017 ---------
*/

#include "IKAMapManager.h"
#include "Runtime/Engine/Classes/Engine/StaticMesh.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "IKABombUtils.h"
#include "IKAGameModeBase.h"
#include "IKABombGameInst.h"
#include "IKADestroyableWall.h"

AIKAMapManager::AIKAMapManager()
{
	PrimaryActorTick.bCanEverTick = false;

	RootCmp = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(RootCmp);

	InstFloorTilesComp = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("InstancedFloorTiles"));
	InstFloorTilesComp->AttachToComponent(RootCmp, FAttachmentTransformRules::KeepRelativeTransform);

	InstWallBoundComp = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("InstancedWallTiles"));
	InstWallBoundComp->AttachToComponent(RootCmp, FAttachmentTransformRules::KeepRelativeTransform);

	InstDestrTilesComp = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("InstancedDestructibleTiles"));
	InstDestrTilesComp->AttachToComponent(RootCmp, FAttachmentTransformRules::KeepRelativeTransform);

	InstNonDestrTilesComp = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("InstancedNonDestructibleTiles"));
	InstNonDestrTilesComp->AttachToComponent(RootCmp, FAttachmentTransformRules::KeepRelativeTransform);

	//set traces each instance mesh component needs
	InstWallBoundComp->SetCollisionResponseToChannel(TRACE_BOMB, ECollisionResponse::ECR_Block);
	InstDestrTilesComp->SetCollisionResponseToChannel(TRACE_DESTROYABLE, ECollisionResponse::ECR_Block);
	InstNonDestrTilesComp->SetCollisionResponseToChannel(TRACE_BOMB, ECollisionResponse::ECR_Block);
	InstFloorTilesComp->SetCollisionResponseToChannel(TRACE_FLOOR, ECollisionResponse::ECR_Block);

}

void AIKAMapManager::BeginPlay()
{
	Super::BeginPlay();

	if (UseGameInstanceData)
	{
		GetMapValues();
	}	

	CreateLogicMap();
	PlaceMapMeshes();
	CreateDestroyableObjects();
	SpawnAtrezzoAssets();

	//send map reference to game mode in order to start the game
	m_GM = Cast<AIKAGameModeBase>(GetWorld()->GetAuthGameMode());
	if (m_GM)
	{
		m_GM->SetMapReference(this);
	}
	
}

void AIKAMapManager::GetMapValues()
{		
	UIKABombGameInst* gi = Cast<UIKABombGameInst>(UGameplayStatics::GetGameInstance(GetWorld()));

	if (gi)
	{
		MapColumns = (gi->AutoMap) ? FMath::RandRange(20, 40) : gi->NumCols;
		MapRows = (gi->AutoMap) ? FMath::RandRange(15, 35) : gi->NumRows;
		m_fGlobalPickupChance = (gi->AutoMap) ? FMath::FRand() : gi->PickupPercentChance / 100.f;
	}

	UseSeed = false;
}

//returns the world position of a tile (instanced mesh)
FVector AIKAMapManager::GetTilePosition(int instance, ETileType type)
{
	FTransform tileTransform;

	switch (type)
	{
		case ETileType::EFloor: InstFloorTilesComp->GetInstanceTransform(instance, tileTransform, true); break;
		case ETileType::EDestructible: InstDestrTilesComp->GetInstanceTransform(instance, tileTransform, true); break;
		case ETileType::ENonDestructible: InstNonDestrTilesComp->GetInstanceTransform(instance, tileTransform, true); break;
		case ETileType::EWallBound: InstWallBoundComp->GetInstanceTransform(instance, tileTransform, true); break;
	}

	return tileTransform.GetLocation();
}

void AIKAMapManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

int AIKAMapManager::RowColPos(int row, int col)
{
	return MapColumns * row + col;
}

void AIKAMapManager::OnConstruction(const FTransform & Transform)
{
	CreateLogicMap();
	PlaceMapMeshes();
}

void AIKAMapManager::CreateLogicMap()
{
	//reset map
	m_LogicMap.Reset();
	m_LogicMap.Init(FTileData(), MapColumns * MapRows);

	m_RandomSeed.Initialize(RandomSeed);
	
	//populate logic map
	for (int col = 0; col < MapColumns; ++col)
	{
		for (int row = 0; row < MapRows; ++row)
		{
			//map bounds
			if (col == 0 || row == 0 || col == MapColumns - 1 || row == MapRows - 1)
			{
				m_LogicMap[RowColPos(row, col)].TileType = ETileType::EWallBound;
			}
			//non destructible tiles in pair cells
			else if ((col % 2 == 0) && (row % 2 == 0) && UIKABombUtils::CheckProbability(SpawnNonDestructibleChance, &m_RandomSeed, UseSeed))
			{
				m_LogicMap[RowColPos(row, col)].TileType = ETileType::ENonDestructible;
			}
			//destructible tiles in odd cells
			else if (UIKABombUtils::CheckProbability(SpawnDestructibleChance, &m_RandomSeed, UseSeed))
			{
				m_LogicMap[RowColPos(row, col)].TileType = ETileType::EDestructible;
			}
		}
	}

	FVector PlayerStart;
	FVector Player2Start;

	//get random player position
	PlayerStart.X = (UseSeed) ? m_RandomSeed.RandRange(1, MapRows / 3) : FMath::RandRange(1, MapRows / 3);
	PlayerStart.Y = (UseSeed) ? m_RandomSeed.RandRange(1, MapColumns / 3) : FMath::RandRange(1, MapColumns / 3);

	//get inverse position for player 2
	Player2Start.X = FMath::Clamp(MapRows - PlayerStart.X, 1.f, MapRows - 2.f);
	Player2Start.Y = FMath::Clamp(MapColumns - PlayerStart.Y, 1.f, MapColumns - 2.f);

	//make a free range around players start
	SetAroundTile(PlayerStart.X, PlayerStart.Y, ETileType::ENoTile);
	SetAroundTile(Player2Start.X, Player2Start.Y, ETileType::ENoTile);
	
	//set players start
	m_LogicMap[RowColPos(PlayerStart.X, PlayerStart.Y)].TileType = ETileType::EPlayerStart;
	m_LogicMap[RowColPos(Player2Start.X, Player2Start.Y)].TileType = ETileType::EPlayerStart;	

	m_PlayersSpawnTiles.Reset();
}

//free tiles around a cell
void AIKAMapManager::SetAroundTile(int row, int col, ETileType type)
{
	m_LogicMap[RowColPos(FMath::Clamp(row - 1, 1, MapRows - 2), col)].TileType = type;
	m_LogicMap[RowColPos(FMath::Clamp(row + 1, 1, MapRows - 2), col)].TileType = type;
	m_LogicMap[RowColPos(row, FMath::Clamp(col + 1, 1, MapColumns - 2))].TileType = type;
	m_LogicMap[RowColPos(row, FMath::Clamp(col - 1, 1, MapColumns - 2))].TileType = type;
}

void AIKAMapManager::PlaceMapMeshes()
{
	int instance = 0;

	//clear instanced static meshes
	InstFloorTilesComp->ClearInstances();
	InstDestrTilesComp->ClearInstances();
	InstNonDestrTilesComp->ClearInstances();
	InstWallBoundComp->ClearInstances();

	m_CornerLocations.Empty(4);

	if (FloorTileMesh &&  NonDestructileTileMesh && DestructileTileMesh)
	{
		//get the floor tile size, I assume all the tiles have the same uniform scale
		m_fTileSize = FloorTileMesh->GetBounds().BoxExtent.X * 2;

		//set mesh data
		InstFloorTilesComp->SetStaticMesh(FloorTileMesh);
		InstDestrTilesComp->SetStaticMesh(DestructileTileMesh);
		InstNonDestrTilesComp->SetStaticMesh(NonDestructileTileMesh);
		InstWallBoundComp->SetStaticMesh(BoundWallTileMesh);

		//define offsets and initial tile creation position
		float rowsOffset = m_fTileSize * (MapRows / 2);
		float colsOffset = -m_fTileSize * (MapColumns / 2);

		for (int col = 0; col < MapColumns; ++col)
		{
			for (int row = 0; row < MapRows; ++row)
			{
				instance = -1;

				//set tile position, initially for floor tile
				FVector tilePosition = GetActorLocation() + (GetActorForwardVector() * rowsOffset) + (GetActorRightVector() * colsOffset);

				//create floor tile 
				InstFloorTilesComp->AddInstanceWorldSpace(FTransform(GetActorRotation(), tilePosition, GetActorScale3D()));

				//locate game tile over floor tile
				FTransform TileTrans = FTransform(GetActorRotation(), tilePosition + FVector(0.f, 0.f, m_fTileSize), GetActorScale3D());

				//create the instanced mesh
				switch (m_LogicMap[RowColPos(row, col)].TileType)
				{
					case ETileType::ENonDestructible:
					{
						if (NonDestrRandomRotation) TileTrans.SetRotation(GetRandomRotation());
						instance = InstNonDestrTilesComp->AddInstanceWorldSpace(TileTrans); 
						break;
					}
					case ETileType::EWallBound:
					{
						if (IsCornerLocation(row, col) && ActorInsteadCorner)
						{
							//save corner world position
							m_CornerLocations.Add(tilePosition);
						}
						else
						{
							TileTrans = GetBoundRandomTransform(TileTrans.GetLocation());
							instance = InstWallBoundComp->AddInstanceWorldSpace(TileTrans);
						}

						break;
					}
					case ETileType::EDestructible:
					{
						if (DestrRandomRotation) TileTrans.SetRotation(GetRandomRotation());
						instance = InstDestrTilesComp->AddInstanceWorldSpace(TileTrans); break;
						break;
					}
					case ETileType::EPlayerStart: m_PlayersSpawnTiles.Add(TileTrans.GetLocation()); break;					
				}

				//update tile physical position and instance
				m_LogicMap[RowColPos(row, col)].TilePosition = TileTrans.GetLocation();
				m_LogicMap[RowColPos(row, col)].MeshInstance = instance;

				//increment row pos
				rowsOffset -= m_fTileSize;
			}
			//reset row offset and increase col offset
			rowsOffset = m_fTileSize * (MapRows / 2);
			colsOffset += m_fTileSize;
		}
	}
}

bool AIKAMapManager::IsCornerLocation(int row, int col)
{
	return ((row == 0 && col == 0) || (row == MapRows - 1 && col == MapColumns - 1) ||
		(row == 0 && col == MapColumns - 1) || (col == 0 && row == MapRows - 1) );
}

FTransform	AIKAMapManager::GetBoundRandomTransform(FVector Loc)
{		
	FVector scaleVec = FVector(1, 1, 1);

	scaleVec.X = (UseSeed) ? m_RandomSeed.FRandRange(BoundXScaleRange.X, BoundXScaleRange.Y) : FMath::FRandRange(BoundXScaleRange.X, BoundXScaleRange.Y);
	scaleVec.Y = (UseSeed) ? m_RandomSeed.FRandRange(BoundYScaleRange.X, BoundYScaleRange.Y) : FMath::FRandRange(BoundYScaleRange.X, BoundYScaleRange.Y);
	scaleVec.Z = (UseSeed) ? m_RandomSeed.FRandRange(BoundZScaleRange.X, BoundZScaleRange.Y) : FMath::FRandRange(BoundZScaleRange.X, BoundZScaleRange.Y);
	
	float yaw = 0;

	if (RandomYawRot) yaw = (UseSeed) ? m_RandomSeed.FRandRange(0, 360) : FMath::FRandRange(0, 360);

	return FTransform(FRotator(0, yaw, 0), Loc, scaleVec);
}


void AIKAMapManager::SpawnAtrezzoAssets()
{
	//spawn corner actors
	for (int i = 0; i < m_CornerLocations.Num(); ++i)
	{
		if (ActorInsteadCorner)
		{
			FTransform trans = FTransform(m_CornerLocations[i]);
			GetWorld()->SpawnActor<AActor>(ActorInsteadCorner, trans);
		}
	}

	//spawn random assets using spawnpoints
	TArray<AActor*> SpawnPoints;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATargetPoint::StaticClass(), SpawnPoints);	

	if (AtrezzoMeshes.Num() == 0) return;
	
	//shuffle spawn points
	for (int i = SpawnPoints.Num() - 1; i > 0; i--)
	{
		int index = FMath::RandRange(0, i);
		
		// Simple swap
		AActor* actor = SpawnPoints[index];
		SpawnPoints[index] = SpawnPoints[i];
		SpawnPoints[i] = actor;
	}

	float mapRadius = FMath::Max((m_fTileSize * MapColumns) + ObjectsMinOffset, (m_fTileSize * MapRows) + ObjectsMinOffset);
	mapRadius /= 2;

	int limit = FMath::Min(SpawnPoints.Num(), (UseSeed) ? m_RandomSeed.RandRange(AtrezzoObjectsNum.X, AtrezzoObjectsNum.Y) : FMath::RandRange((int)AtrezzoObjectsNum.X, (int)AtrezzoObjectsNum.Y));

	for (int i = 0; i < limit; ++i)
	{
		float distFromMap = (SpawnPoints[i]->GetActorLocation() - GetActorLocation()).Size();

		if (distFromMap > mapRadius)
		{
			int index = (UseSeed) ? m_RandomSeed.RandRange(0, AtrezzoMeshes.Num() - 1) : FMath::RandRange(0, AtrezzoMeshes.Num() - 1);
			FTransform assetTrans = FTransform(SpawnPoints[i]->GetActorLocation());
			assetTrans.SetRotation(GetRandomRotation(true));

			AStaticMeshActor* atrezzoObj = GetWorld()->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass(), assetTrans);

			if (atrezzoObj)
			{
				atrezzoObj->GetStaticMeshComponent()->SetStaticMesh(AtrezzoMeshes[index]);
			}
		}
	}

}

FQuat AIKAMapManager::GetRandomRotation(bool OnlyYaw)
{
	float X = 90 * ((UseSeed) ? m_RandomSeed.RandRange(0, 3) : FMath::RandRange(0, 3));
	float Y = 90 * ((UseSeed) ? m_RandomSeed.RandRange(0, 3) : FMath::RandRange(0, 3));
	float Z = 90 * ((UseSeed) ? m_RandomSeed.RandRange(0, 3) : FMath::RandRange(0, 3));

	return (!OnlyYaw) ? FQuat(FRotator(X, Y, Z)) : FQuat(FRotator(0, Y, 0));
}

void AIKAMapManager::CreateDestroyableObjects()
{
	//traverse Logic Map and creates actors to represent destructible walls
	for (int i = 0; i < m_LogicMap.Num(); ++i)
	{
		if (m_LogicMap[i].TileType == ETileType::EDestructible)
		{
			//get transformation from map
			FTransform DestrTrans = FTransform(GetActorRotation(), m_LogicMap[i].TilePosition, GetActorScale3D());
			AIKADestroyableWall* DestrWall = Cast<AIKADestroyableWall>(UGameplayStatics::BeginDeferredActorSpawnFromClass(this, DestructibleWallTemplate, DestrTrans));

			//set destructible data before finish spawn
			if (DestrWall)
			{
				if (UseGameInstanceData)
				{
					DestrWall->PickupChance = m_fGlobalPickupChance;
				}

				DestrWall->SetData(m_fTileSize / 2, this);
				UGameplayStatics::FinishSpawningActor(DestrWall, DestrTrans);
			}
		}
	}
}

//returns world player start position by controller id
FVector AIKAMapManager::GetPlayerStartLocation(int Player)
{
	return (Player < m_PlayersSpawnTiles.Num()) ? m_PlayersSpawnTiles[Player] : FVector::ZeroVector;
}

//delete an instanced mesh from component
void AIKAMapManager::RemoveMapDestructibleTileMesh(int instance)
{
	if (instance >= 0)
	{
		InstDestrTilesComp->RemoveInstance(instance);
	}
	
}