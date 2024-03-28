// Fill out your copyright notice in the Description page of Project Settings.

#include "AREmulaterGameMode.h"
#include "AREmulatorPawn.h"
#include "CustomARPawn.h"
#include "PlayAreaPlane.h"
#include "CustomGameState.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"

AAREmulaterGameMode::AAREmulaterGameMode() 
{
	DefaultPawnClass = AAREmulatorPawn::StaticClass();
	GameStateClass = ACustomGameState::StaticClass();
}

void AAREmulaterGameMode::LineTraceSpawnActor(FVector ScreenPos)
{
	//Basic variables for functionality
	APlayerController* playerController = UGameplayStatics::GetPlayerController(this, 0);
	FVector WorldPos;
	FVector WorldDir;

	//Gets the screen touch in world space and the tracked objects from a line trace from the touch
	UGameplayStatics::DeprojectScreenToWorld(playerController, FVector2D(ScreenPos), WorldPos, WorldDir);

	// construct trace vector (from point clicked to 1000.0 units beyond in same direction)
	FVector traceEndVector = WorldDir * 1000.0;
	traceEndVector = WorldPos + traceEndVector;

	FHitResult hitResult;

	// perform line trace (Raycast)
	bool traceSuccess = GetWorld()->LineTraceSingleByChannel(hitResult, WorldPos, traceEndVector, ECollisionChannel::ECC_WorldStatic);

	if (traceSuccess)
	{
		const FActorSpawnParameters SpawnInfo;
		const FRotator MyRot(0, 0, 0);
		const FVector MyLoc(hitResult.Location.X, hitResult.Location.Y, hitResult.Location.Z + 0.01);

		if (!bPlaneSpawned) 
		{
			PlayArea = GetWorld()->SpawnActor<APlayAreaPlane>(MyLoc, MyRot, SpawnInfo);
			bPlaneSpawned = true;
		}
		PlayArea->AddVertex(MyLoc);
	}	
}
