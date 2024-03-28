// Fill out your copyright notice in the Description page of Project Settings.
#pragma once
#include "PlayAreaPlane.h"
#include "PlaceableActor.h"
#include "Engine/EngineTypes.h"
#include "CustomGameState.h"
#include "GameFramework/Pawn.h"
#include "CustomARPawn.generated.h"

class UCameraComponent;

/// <summary>
/// Class to handle player interaction and gameplay states.
/// </summary>

UCLASS()
class UE5_AR_API ACustomARPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ACustomARPawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void OnScreenTouch(const ETouchIndex::Type FingerIndex, const FVector ScreenPos);

	// Happens between touch and release based on isFingerDown
	virtual void OnHold();

	virtual void OnScreenRelease(const ETouchIndex::Type FingerIndex, const FVector ScreenPos);

	virtual void MoveSelectedActor();


	// Manages game states
	virtual void StateSwitch();


	// Initialisers
	virtual void MainMenuInit();

	virtual void SetUpInit();

	virtual void PlayingInit();

	virtual void PauseInit();

	virtual void LoseInit();


	// Functions called by timers
	virtual void SpawnBuildings();

	virtual void IncrementScore();

	// Tracking variables
	bool isFingerDown = false;

	FTimerHandle BuildingTimer;
	FTimerHandle ScoreTimer;

	StateEnum CurrentState = StateEnum::MainMenu;


	// Components and important references
	UPROPERTY(Category = "CustomARClasses", VisibleAnywhere, BlueprintReadWrite)
	UCameraComponent* Camera;

	UPROPERTY(Category = "CustomARClasses", VisibleAnywhere, BlueprintReadWrite)
	UMaterialInterface* RedMat;

	UPROPERTY(Category = "myCategory", VisibleAnywhere, BlueprintReadWrite)
	APlaceableActor* SelectedActor;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Called to test raytrace
	virtual bool WorldHitTest(FVector2D screenPos, FHitResult& hitResult);

	UPROPERTY(Category = "myCategory", VisibleAnywhere, BlueprintReadWrite)
		USceneComponent* SceneComponent;

	UPROPERTY(Category = "myCategory", VisibleAnywhere, BlueprintReadWrite)
		UCameraComponent* CameraComponent;
};
