// Fill out your copyright notice in the Description page of Project Settings.

/*
--- Bomberman Test UE4 -------
--- Author: David Escalona ---
--- Date: 13/11/2017 ---------
*/
#include "IKABombUtils.h"
#include "Engine.h"


bool UIKABombUtils::CheckProbability(float chance, const FRandomStream* seed, bool useSeed)
{
	float offset = (useSeed) ? seed->FRandRange(0, 65535) : FMath::FRandRange(0, 65535);
	float rndTemp = (useSeed) ? seed->FRandRange(0, 2) : FMath::FRandRange(0, 2);
	float doubleChance = chance * 2;
	float p = 2 - doubleChance;

	return (rndTemp > (2 - doubleChance) && rndTemp < FMath::Clamp(FMath::Fmod(offset, p) + doubleChance, 0.f, 2.f));
}

bool UIKABombUtils::CheckProbability(float chance)
{
	float offset = FMath::FRandRange(0, 65535);
	float rndTemp = FMath::FRandRange(0, 2);
	float doubleChance = chance * 2;
	float p = 2 - doubleChance;

	return (rndTemp > (2 - doubleChance) && rndTemp < FMath::Clamp(FMath::Fmod(offset, p) + doubleChance, 0.f, 2.f));
}

bool UIKABombUtils::YesChance(int percentage)
{
	return (FMath::RandRange(1, 100 / percentage) == 1 ? true : false);
}

void UIKABombUtils::FindScreenEdgeLocationForWorldLocation(UObject* WorldContextObject, const FVector& InLocation, const float EdgePercent, FVector2D& OutScreenPosition, float& OutRotationAngleDegrees, bool &bIsOnScreen)
{
	bIsOnScreen = false;
	OutRotationAngleDegrees = 0.f;

	const FVector2D ViewportSize = FVector2D(GEngine->GameViewport->Viewport->GetSizeXY());
	const FVector2D  ViewportCenter = FVector2D(ViewportSize.X / 2, ViewportSize.Y / 2);

	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::ReturnNull);

	if (!World) return;

	APlayerController* PlayerController = (WorldContextObject ? UGameplayStatics::GetPlayerController(WorldContextObject, 0) : NULL);
	if (!PlayerController) return;

	APlayerCameraManager* ViewPointActor = (WorldContextObject ? UGameplayStatics::GetPlayerCameraManager(WorldContextObject, 0) : NULL);
	// APawn* ViewPointActor = PlayerController->GetPawn();

	if (!ViewPointActor) return;

	// FVector Location = ViewPointActor->GetActorLocation();
	FVector Location = ViewPointActor->GetCameraLocation();

	FVector Forward = ViewPointActor->GetActorForwardVector();
	FVector Offset = (InLocation - Location).GetSafeNormal();

	float DotProduct = FVector::DotProduct(Forward, Offset);
	bool bLocationIsBehindCamera = (DotProduct < 0);

	if (bLocationIsBehindCamera)
	{
		FVector DiffVector = InLocation - Location;
		FVector Inverted = DiffVector * -1.f;
		FVector NewInLocation = Location * Inverted;

		NewInLocation.Z -= 5000;

		PlayerController->ProjectWorldLocationToScreen(NewInLocation, OutScreenPosition);
		OutScreenPosition.Y = (EdgePercent * ViewportCenter.X) * 2.f;
		OutScreenPosition.X = -ViewportCenter.X - OutScreenPosition.X;
	}

	PlayerController->ProjectWorldLocationToScreen(InLocation, OutScreenPosition);//*ScreenPosition);

	if (OutScreenPosition.X >= 0.f && OutScreenPosition.X <= ViewportSize.X && OutScreenPosition.Y >= 0.f && OutScreenPosition.Y <= ViewportSize.Y)
	{
		bIsOnScreen = true;
		return;
	}

	OutScreenPosition -= ViewportCenter;

	float AngleRadians = FMath::Atan2(OutScreenPosition.Y, OutScreenPosition.X);
	AngleRadians -= FMath::DegreesToRadians(90.f);

	OutRotationAngleDegrees = FMath::RadiansToDegrees(AngleRadians) + 180.f;

	float Cos = cosf(AngleRadians);
	float Sin = -sinf(AngleRadians);

	OutScreenPosition = FVector2D(ViewportCenter.X + (Sin * 180.f), ViewportCenter.Y + Cos * 180.f);

	float m = Cos / Sin;

	FVector2D ScreenBounds = ViewportCenter * EdgePercent;

	if (Cos > 0)
	{
		OutScreenPosition = FVector2D(ScreenBounds.Y / m, ScreenBounds.Y);
	}
	else
	{
		OutScreenPosition = FVector2D(-ScreenBounds.Y / m, -ScreenBounds.Y);
	}

	if (OutScreenPosition.X > ScreenBounds.X)
	{
		OutScreenPosition = FVector2D(ScreenBounds.X, ScreenBounds.X*m);
	}
	else if (OutScreenPosition.X < -ScreenBounds.X)
	{
		OutScreenPosition = FVector2D(-ScreenBounds.X, -ScreenBounds.X*m);
	}

	OutScreenPosition += ViewportCenter;

}