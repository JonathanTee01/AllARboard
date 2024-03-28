// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "Components/Widget.h"
#include "PlayAreaPlane.h"
#include "HelloARManager.h"
#include "GameFramework/GameModeBase.h"
#include "Blueprint/UserWidget.h"
#include "StaticActor.h"
#include "PlaceableActor.h"
#include "Kismet/KismetMathLibrary.h"
#include "CustomGameMode.generated.h"
//Forward Declarations
class APlaceableActor;

/// <summary>
/// Handles spawning and managing data needed for the UI
/// </summary>

UCLASS()
class UE5_AR_API ACustomGameMode : public AGameModeBase
{
	GENERATED_BODY()
private:
	// Variable for spawn conditions
	bool bPlaneSpawned = false;
	int workplaceSpawnTracker = 0;
	TArray<APlayAreaPlane*> spawnedPlanes;

	APlaceableActor* LastSpawnedStation;

public:
	ACustomGameMode();
	virtual ~ACustomGameMode() = default;

	virtual void StartPlay() override;

	// Getters
	UFUNCTION(BlueprintCallable, Category = "Score")
		int32 GetStationsSpawnable();

	UFUNCTION(BlueprintCallable, Category = "Score")
		int32 GetScore();

	UFUNCTION(BlueprintCallable, Category = "Tracking")
		virtual StateEnum GetStateEnum();

	UFUNCTION(BlueprintCallable, Category = "Score")
		void GetColorCharges(int32& Red, int32& Green, int32& Blue);

	// Setters
	UFUNCTION(BlueprintCallable, Category="Score")
		void SetScore(const int32 NewScore);
	
	UFUNCTION(BlueprintCallable, Category = "Tracking")
		virtual void SetStateEnum(const StateEnum state);

	UFUNCTION(BlueprintCallable, Category="Score")
		void IncrementScore(const int32 ScoreToAdd);
	
	// Utilities
	UFUNCTION(BlueprintCallable, Category = "ARSetup")
		virtual void Reset();
	
	UFUNCTION(BlueprintCallable, Category = "ARSetup")
		virtual bool CheckPlanes(); 
	
	UFUNCTION(BlueprintCallable, Category = "ARSetup")
		virtual void HidePlanes(); 
	
	// Interaction with UI for placement confirmation
	UFUNCTION(BlueprintCallable, Category = "Gameplay")
		virtual bool StartPlacing(); 
	
	UFUNCTION(BlueprintCallable, Category = "Gameplay")
		virtual bool ConfirmPlacing();
	
	UFUNCTION(BlueprintCallable, Category = "Gameplay")
		virtual void DenyPlacing(); 
	
	UFUNCTION(BlueprintCallable, Category = "ARSetup")
		virtual void AddPlane();
	
	UFUNCTION(BlueprintCallable, Category = "ARSetup")
		virtual void RemovePlane(); 
	

	virtual void Tick(float DeltaSeconds) override;

	// Spawning functions
	virtual void SpawnBuildings();

	virtual void LineTraceSpawnStation(FVector ScreenPos);
	virtual void LineTraceSpawnPlane(FVector2D ScreenPos);
	
	virtual void SpawnInitialActors();

	 UPROPERTY(Category="Placeable",EditAnywhere,BlueprintReadWrite)
	 TSubclassOf<AActor> HouseToSpawn;

	 UPROPERTY(Category="Placeable",EditAnywhere,BlueprintReadWrite)
	 TSubclassOf<AActor> StationToSpawn;

	 UPROPERTY(Category="Placeable",EditAnywhere,BlueprintReadWrite)
	 bool placingStation = false;

	 UPROPERTY(Category="Placeable",EditAnywhere,BlueprintReadWrite)
	 TSubclassOf<AActor> WorkplaceToSpawn;
	 
	 UPROPERTY(Category="Placeable",EditAnywhere)
	 AHelloARManager* ARManager;

	 UPROPERTY(Category="AR Actors", EditAnywhere, BlueprintReadWrite)
	 APlayAreaPlane* PlayArea;

	 UPROPERTY(Category = "Material", EditAnywhere, BlueprintReadWrite)
	 UMaterialInterface* TransMat;

	 UPROPERTY(Category="UI Elements", EditAnywhere, BlueprintReadWrite)
	 TSubclassOf<UUserWidget> UIWidget;
};
