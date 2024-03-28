// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "Camera/CameraComponent.h"
#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "AREmulatorPawn.generated.h"

UCLASS()
class UE5_AR_API AAREmulatorPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AAREmulatorPawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void MoveForwards(float scale);

	virtual void MoveStrafe(float scale);

	virtual void MoveVertical(float scale);

	virtual void LookPitch(float scale);

	virtual void LookYaw(float scale);

	virtual void OnScreenTouch(const ETouchIndex::Type FingerIndex, const FVector ScreenPos);

	virtual bool WorldHitTest(FVector2D screenPos, FHitResult& hitResult);

	float speedForwards = 100.f;

	float speedStrafe = 70.f;

	float speedVertical = 40.f;

	FVector2D CameraInput = FVector2D(0.0f, 0.0f);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(Category = "myCategory", VisibleAnywhere, BlueprintReadWrite)
		USceneComponent* SceneComponent;

	UPROPERTY(Category = "myCategory", VisibleAnywhere, BlueprintReadWrite)
		UCameraComponent* CameraComponent;

};
