// Fill out your copyright notice in the Description page of Project Settings.

/*
--- Bomberman Test UE4 -------
--- Author: David Escalona Rocha ---
--- Date: 13/11/2017 ---------
*/

#pragma once

#include "IKABombermanUE.h"
#include "GameFramework/Actor.h"
#include "Runtime/Engine/Classes/Components/InstancedStaticMeshComponent.h"
#include "IKAMapManager.generated.h"

UENUM(BlueprintType)
enum class ETileType : uint8
{
	ENoTile = 0				UMETA(DisplayName = "No Tile"),
	EWallBound = 1			UMETA(DisplayName = "Wall Bound Tile"),
	EDestructible = 2		UMETA(DisplayName = "Destructible Wall"),
	ENonDestructible = 3	UMETA(DisplayName = "Non Destructible Wall"),
	EPlayerStart = 4		UMETA(DisplayName = "Player Start"),
	EFloor = 5				UMETA(DisplayName = "Floor Tile")
};

USTRUCT(BlueprintType)
struct FTileData
{
	GENERATED_BODY()

	UPROPERTY(SaveGame)
	FVector TilePosition;

	UPROPERTY(SaveGame)
	ETileType TileType;

	UPROPERTY(SaveGame)
	int MeshInstance;

	FTileData()
	{
		TilePosition = FVector::ZeroVector;
		TileType = ETileType::ENoTile;
		MeshInstance = -1;
	}

};

UCLASS()
class IKABOMBERMANUE_API AIKAMapManager : public AActor
{
	GENERATED_BODY()
	
public:	
	
	AIKAMapManager();

	virtual void OnConstruction(const FTransform & Transform) override;
	virtual void Tick(float DeltaTime) override;
	
	UFUNCTION(BlueprintCallable, Category = "Map Manager")
	FVector GetPlayerStartLocation(int Player);

	void RemoveMapDestructibleTileMesh(int instance);
	FVector GetTilePosition(int instance, ETileType type);

	FORCEINLINE float GetMapTileSize() { return m_fTileSize; }
	FORCEINLINE int GetMapCols() { return MapColumns; }
	FORCEINLINE int GetMapRows() { return MapRows; }

protected:
	
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map Manager")
	bool UseGameInstanceData = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map Manager")
	int MapColumns = 25;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map Manager")
	int MapRows = 20;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map Manager")
	bool UseSeed = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map Manager")
	float RandomSeed;

	//chance to spawn a destructible wall in the empty cells (0.0 - 1.0)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map Manager")
	float SpawnDestructibleChance = 0.6f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map Manager")
	float SpawnNonDestructibleChance = 0.85f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map Manager")
	TSubclassOf<class AIKADestroyableWall> DestructibleWallTemplate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map Manager")
	UStaticMesh* BoundWallTileMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map Manager")
	bool RandomYawRot = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map Manager")
	FVector2D BoundXScaleRange = FVector2D(1, 1);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map Manager")
	FVector2D BoundYScaleRange = FVector2D(1, 1);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map Manager")
	FVector2D BoundZScaleRange = FVector2D(0.9, 1.1);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map Manager")
	UStaticMesh* FloorTileMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map Manager")
	UStaticMesh* DestructileTileMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map Manager")
	bool DestrRandomRotation = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map Manager")
	UStaticMesh* NonDestructileTileMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map Manager")
	bool NonDestrRandomRotation = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map Manager")
	float ObjectsMinOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map Manager")
	TSubclassOf<AActor> ActorInsteadCorner;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map Manager")
	TArray<UStaticMesh*> AtrezzoMeshes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map Manager")
	FVector2D AtrezzoObjectsNum;

private:
	
	UPROPERTY(EditDefaultsOnly, Category = "Components")
	USceneComponent* RootCmp;

	UPROPERTY(EditDefaultsOnly, Category = "Components")
	UInstancedStaticMeshComponent* InstFloorTilesComp;

	UPROPERTY(EditDefaultsOnly, Category = "Components")
	UInstancedStaticMeshComponent* InstWallBoundComp;

	UPROPERTY(EditDefaultsOnly, Category = "Components")
	UInstancedStaticMeshComponent* InstDestrTilesComp;

	UPROPERTY(EditDefaultsOnly, Category = "Components")
	UInstancedStaticMeshComponent* InstNonDestrTilesComp;

	
	void		CreateLogicMap();
	void		PlaceMapMeshes();
	void		CreateDestroyableObjects();
	int			RowColPos(int row, int col);
	void		SetAroundTile(int row, int col, ETileType type);
	FTransform	GetBoundRandomTransform(FVector Loc);
	FQuat		GetRandomRotation(bool OnlyYaw = false);
	void		SpawnAtrezzoAssets();
	bool		IsCornerLocation(int row, int col);
	void		GetMapValues();

	TArray<FTileData>	m_LogicMap;
	TArray<FVector>		m_PlayersSpawnTiles;
	TArray<FVector>		m_CornerLocations;
	FRandomStream		m_RandomSeed;
	float				m_fTileSize;
	float				m_fGlobalPickupChance;
	class				AIKAGameModeBase* m_GM;
	
	
};
