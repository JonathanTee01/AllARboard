// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ARTypes.h"
#include "GameFramework/Actor.h"
#include "HelloARManager.generated.h"


class UARSessionConfig;
class AARPlaneActor;
class UARPlaneGeometry;

/// <summary>
/// Slightly adapted from the given class. Functions were added to allow for extra utilities
/// </summary>

UCLASS()
class UE5_AR_API AHelloARManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHelloARManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//Create a default Scene Component
	UPROPERTY(Category = "SceneComp", VisibleAnywhere, BlueprintReadWrite)
		USceneComponent* SceneComponent;

	// Toggles the creation of any new planes
	virtual void ShouldCreateNewPlanes(bool shouldCreate);

	// Toggles the visibility of planes. Currently not used but could prove useful in future development
	virtual void SetPlaneVisibility(bool isVisible);

	virtual void StartARCoreSession();

	virtual void ResetARCoreSession();

protected:
	
	// Updates the plane actors on every frame as long as the AR Session is running
	void UpdatePlaneActors();

	AARPlaneActor* SpawnPlaneActor();
	FColor GetPlaneColor(int Index);
	// Configuration file for AR Session
	UARSessionConfig* Config;

	//Base plane actor for geometry detection
	AARPlaneActor* PlaneActor;


	//Map of geometry planes
	TMap<UARPlaneGeometry*, AARPlaneActor*> PlaneActors;

	//Index for plane colours adn array of colours
	int PlaneIndex = 0;
	TArray<FColor> PlaneColors;

	bool createNewPlanes;
};
