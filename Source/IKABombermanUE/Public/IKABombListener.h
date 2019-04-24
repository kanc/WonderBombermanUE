// Fill out your copyright notice in the Description page of Project Settings.

/*
--- Bomberman Test UE4 -------
--- Author: David Escalona ---
--- Date: 13/11/2017 ---------
*/

#pragma once

#include "CoreMinimal.h"
#include "IKABombListener.generated.h"

UINTERFACE(Blueprintable,MinimalAPI)
class UIKABombListener : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class IKABOMBERMANUE_API IIKABombListener
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Bomb Listener")
	void BombExplodes(AActor* bomb);
	
};
