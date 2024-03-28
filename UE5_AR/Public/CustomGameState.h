// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "CustomGameState.generated.h"

/// <summary>
/// Enumerator used for tracking the game state
/// </summary>

UENUM(BlueprintType)
enum class StateEnum : uint8
{
	MainMenu = 0 UMETA(DisplayName = "Main Menu"),
	Setup = 1 UMETA(DisplayName = "Setup"),
	Playing = 2 UMETA(DispayName = "Playing"),
	Pause = 3 UMETA(DisplayName = "Pause"),
	Exit = 4 UMETA(DisplayName = "Exit"),
	Lose = 5 UMETA(DisplayName = "Lose")
};

/// <summary>
/// Tracks inportant game variables
/// </summary>

UCLASS()
class UE5_AR_API ACustomGameState : public AGameStateBase
{
	GENERATED_BODY()	

public:
	ACustomGameState();
	~ACustomGameState() = default;

	int32 Score;

	UPROPERTY(Category = "StateTracking", VisibleAnywhere, BlueprintReadWrite)
	StateEnum StateTracker;

	int32 RedCharges = 0;
	int32 GreenCharges = 0;
	int32 BlueCharges = 0;

	int32 StationsSpawnable;
};
