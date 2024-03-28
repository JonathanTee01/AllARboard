// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UObject/ConstructorHelpers.h"
#include "Kismet/KismetMathLibrary.h"
#include "ARPin.h"
#include "ARBlueprintLibrary.h"
#include "CustomGameState.h"
#include "GameFramework/Actor.h"
#include "StaticActor.generated.h"

/// <summary>
/// Enumerator to track the given colour during spawning
/// </summary>

UENUM(BlueprintType)
enum class ColorEnum : uint8
{
	Red = 0 UMETA(DisplayName = "Red"),
	Green = 1 UMETA(DisplayName = "Green"),
	Blue = 2 UMETA(DispayName = "Blue")
};

/// <summary>
/// Class for actors that when spawned will not be removed or moved until reset
/// </summary>

UCLASS()
class UE5_AR_API AStaticActor : public AActor
{
	GENERATED_BODY()


public:
	// Sets default values for this actor's properties
	AStaticActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void ReduceTimer(float ReductionAmount);

	UPROPERTY(Category = "ActorComponent", VisibleAnywhere, BlueprintReadWrite)
	USceneComponent* SceneComponent;

	UPROPERTY(Category = "ActorComponent", VisibleAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* StaticMeshComponent;

	// Materials set in editor
	UPROPERTY(EditAnywhere)
	UMaterialInterface* RedMaterial;

	UPROPERTY(EditAnywhere)
	UMaterialInterface* GreenMaterial;

	UPROPERTY(EditAnywhere)
	UMaterialInterface* BlueMaterial;

	// Timer properties for the lose condition
	UPROPERTY(Category = "GameTimer", VisibleAnywhere, BlueprintReadWrite)
	float Timer = 0;

	UPROPERTY(Category = "GameTimer", VisibleAnywhere, BlueprintReadWrite)
	float MaxTimer = 30;

	ColorEnum Color;

	UARPin* PinComponent;
};
