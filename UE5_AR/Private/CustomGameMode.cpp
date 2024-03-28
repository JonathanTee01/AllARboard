// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomGameMode.h"
#include "CustomARPawn.h"
#include "CustomGameState.h"
#include "CustomActor.h"
#include "HelloARManager.h"
#include "ARPin.h"
#include "ARBlueprintLibrary.h"
#include "PlayAreaPlane.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "PlaceableActor.h"

ACustomGameMode::ACustomGameMode()
{
	// Add this line to your code if you wish to use the Tick() function
	PrimaryActorTick.bCanEverTick = true;

	// Set the default pawn and gamestate to be our custom pawn and gamestate programatically
	DefaultPawnClass = ACustomARPawn::StaticClass();
	GameStateClass = ACustomGameState::StaticClass();
	
}


void ACustomGameMode::StartPlay() 
{
	SpawnInitialActors();

	// This function will transcend to call BeginPlay on all the actors 
	Super::StartPlay();

	if (UIWidget) 
	{
		UUserWidget* ui = CreateWidget<UUserWidget>(GetWorld(), UIWidget);
		ui->AddToViewport();
	}
}

int32 ACustomGameMode::GetStationsSpawnable()
{
	return GetGameState<ACustomGameState>()->StationsSpawnable;
}

int32 ACustomGameMode::GetScore()
{
	return GetGameState<ACustomGameState>()->Score;
}

// Returns all the colour charges needed for UI
void ACustomGameMode::GetColorCharges(int32& Red, int32& Green, int32& Blue)
{
	Red = GetGameState<ACustomGameState>()->RedCharges;
	Green = GetGameState<ACustomGameState>()->GreenCharges;
	Blue = GetGameState<ACustomGameState>()->BlueCharges;
}

void ACustomGameMode::SetScore(const int32 NewScore)
{
	GetGameState<ACustomGameState>()->Score = NewScore;
}

void ACustomGameMode::IncrementScore(const int32 ScoreToAdd)
{
	GetGameState<ACustomGameState>()->Score += ScoreToAdd;
}

StateEnum ACustomGameMode::GetStateEnum()
{
	return GetGameState<ACustomGameState>()->StateTracker;	
}

void ACustomGameMode::SetStateEnum(const StateEnum state)
{
	GetGameState<ACustomGameState>()->StateTracker = state;	
}

// Set everything back to default values
void ACustomGameMode::Reset()
{
	bPlaneSpawned = false;
	spawnedPlanes.Empty(); 
	workplaceSpawnTracker = 0;
	ACustomGameState* GS = GetGameState<ACustomGameState>();
	GS->StationsSpawnable = 2;
	GS->RedCharges = 0;
	GS->GreenCharges = 0;
	GS->BlueCharges = 0;
}

void ACustomGameMode::AddPlane()
{
	// Return if there is no plane to add
	if (PlayArea == NULL) return;

	// Otherwise add a plane to the array and clear the temporary reference
	bPlaneSpawned = false;
	spawnedPlanes.Add(PlayArea);
	PlayArea = NULL;
}

void ACustomGameMode::RemovePlane()
{
	bPlaneSpawned = false;

	// If a plane is being drawn then Destroy it
	if (PlayArea) 
	{
		PlayArea->Destroy();
		PlayArea = NULL;
	}
	// Otherwise delete the most recently added
	else  if (spawnedPlanes.Num() > 0)
	{
		spawnedPlanes.Last()->Destroy();
		spawnedPlanes.Pop();
	}
}

bool ACustomGameMode::CheckPlanes()
{
	// Used to verify planes have been placed
	return (spawnedPlanes.Num() > 0);	
}

void ACustomGameMode::HidePlanes()
{
	// Change the planes to a more transparent material
	for (int i = 0; i < spawnedPlanes.Num(); i++) 
	{
		spawnedPlanes[i]->UpdatePlaneMaterial(TransMat);
	}
}

bool ACustomGameMode::StartPlacing()
{
	// If able to spawn more stations
	if (GetGameState<ACustomGameState>()->StationsSpawnable > 0) placingStation = true;

	return placingStation;
}

bool ACustomGameMode::ConfirmPlacing()
{
	// If a station has been placed
	if (!placingStation && LastSpawnedStation != NULL)
	{
		// Update bPlaced. Used to stop it being moved once placed
		LastSpawnedStation->bPlaced = true;
		LastSpawnedStation->CheckNearbyBuildings(30, true);
		return true;
	}
	return false;
}

void ACustomGameMode::DenyPlacing()
{
	// If a station has been placed
	if (!placingStation && LastSpawnedStation != NULL)
	{
		LastSpawnedStation->Destroy();
		GetGameState<ACustomGameState>()->StationsSpawnable += 1;
	}
}

void ACustomGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void ACustomGameMode::SpawnInitialActors()
{
	// Spawn an instance of the HelloARManager class
	ARManager = GetWorld()->SpawnActor<AHelloARManager>();
}

void ACustomGameMode::SpawnBuildings()
{
	// Every 3 times this timer triggers reset the tracker to 0
	if (workplaceSpawnTracker == 3) 
	{
		workplaceSpawnTracker = 0;
		GetGameState<ACustomGameState>()->StationsSpawnable += 1;
	}

	// Make an array to store the vertices
	TArray<FVector> vertices;

	// Spawn a workplace every 3 times starting the 1st
	if (workplaceSpawnTracker == 0) 
	{
		// Pick a random player defined play area
		int randomPlaneInt = UKismetMathLibrary::RandomInteger64InRange(0, spawnedPlanes.Num() - 1);

		// Get the vertices and location of the randomly selected plane
		TArray<FVector> verticesOnPlane = spawnedPlanes[randomPlaneInt]->GetVertices();
		FVector planeLoc = spawnedPlanes[randomPlaneInt]->GetActorLocation();

		// Pick 2 random vertices
		vertices.Empty();
		for (int i = 0; i < 2; i++)
		{
			int randomInt = UKismetMathLibrary::RandomInteger64InRange(0, verticesOnPlane.Num() - 1);

			vertices.Add(verticesOnPlane[randomInt]);
		}

		// Pick a radom number between 0 and 1 to use as a ratio
		float randomFloat = UKismetMathLibrary::RandomFloatInRange(0.f, 1.f);

		const FActorSpawnParameters SpawnInfo;
		const FRotator MyRot(0.f, UKismetMathLibrary::RandomFloatInRange(0.f, 360.f), 0.f);
		FVector spawnLocation;

		// Using the random ratio find a location on the plane between the two points
		spawnLocation = (vertices[0] * randomFloat) + (vertices[1] * (1 - randomFloat)) + planeLoc;

		GetWorld()->SpawnActor<AStaticActor>(WorkplaceToSpawn, spawnLocation, MyRot, SpawnInfo);
	}

	// Pick a random player defined play area
	int randomPlaneInt = UKismetMathLibrary::RandomInteger64InRange(0, spawnedPlanes.Num() - 1);

	// Spawn houses
	for (int o = 0; o < 4; o++) 
	{
		// Get the vertices and location of the randomly selected plane
		TArray<FVector> verticesOnPlane = spawnedPlanes[randomPlaneInt]->GetVertices();
		FVector planeLoc = spawnedPlanes[randomPlaneInt]->GetActorLocation();

		// Pick 2 random vertices
		vertices.Empty();
		for (int i = 0; i < 2; i++)
		{
			int randomInt = UKismetMathLibrary::RandomInteger64InRange(0, verticesOnPlane.Num() - 1);

			vertices.Add(verticesOnPlane[randomInt]);
		}

		// Pick a radom number between 0 and 1 to use as a ratio
		float randomFloat = UKismetMathLibrary::RandomFloatInRange(0.f, 1.f);

		const FActorSpawnParameters SpawnInfo;
		const FRotator MyRot(0.f, UKismetMathLibrary::RandomFloatInRange(0.f, 360.f), 0.f);
		FVector spawnLocation;

		// Using the random ratio find a location on the plane between the two points
		spawnLocation = (vertices[0] * randomFloat) + (vertices[1] * (1 - randomFloat)) + planeLoc;

		GetWorld()->SpawnActor<AStaticActor>(HouseToSpawn, spawnLocation, MyRot, SpawnInfo);
	}

	// Increment the tracker
	workplaceSpawnTracker++;
}

void ACustomGameMode::LineTraceSpawnStation(FVector ScreenPos)
{
	if (!placingStation) return;

	//Basic variables for functionality
	APlayerController* playerController = UGameplayStatics::GetPlayerController(this, 0);
	FVector WorldPos;
	FVector WorldDir;

	//Gets the screen touch in world space and the tracked objects from a line trace from the touch
	UGameplayStatics::DeprojectScreenToWorld(playerController, FVector2D(ScreenPos), WorldPos, WorldDir);

	// construct trace vector (from point clicked to 1000.0 units beyond in same direction)
	FVector traceEndVector = WorldDir * 2000.0;
	traceEndVector = WorldPos + traceEndVector;

	FHitResult hitResult;

	// perform line trace (Raycast)
	bool traceSuccess = GetWorld()->LineTraceSingleByChannel(hitResult, WorldPos, traceEndVector, ECollisionChannel::ECC_WorldStatic);

	if (traceSuccess)
	{
		const FActorSpawnParameters SpawnInfo;
		const FRotator MyRot(0, 0, 0);
		const FVector MyLoc(hitResult.Location.X, hitResult.Location.Y, hitResult.Location.Z + 0.01);

		LastSpawnedStation = GetWorld()->SpawnActor<APlaceableActor>(StationToSpawn, MyLoc, MyRot, SpawnInfo);
	}

	// Set variables needed for UI elements
	placingStation = false;
	GetGameState<ACustomGameState>()->StationsSpawnable -= 1;
}

void ACustomGameMode::LineTraceSpawnPlane(FVector2D ScreenPos)
{
	//Basic variables for functionality
	APlayerController* playerController = UGameplayStatics::GetPlayerController(this, 0);
	FVector WorldPos;
	FVector WorldDir;

	//Gets the screen touch in world space and the tracked objects from a line trace from the touch
	UGameplayStatics::DeprojectScreenToWorld(playerController, ScreenPos, WorldPos, WorldDir);

	// construct trace vector (from point clicked to 1000.0 units beyond in same direction)
	FVector traceEndVector = WorldDir * 1000.0;
	traceEndVector = WorldPos + traceEndVector;

	FHitResult hitResult;

	// perform line trace (Raycast)
	auto TraceResult = UARBlueprintLibrary::LineTraceTrackedObjects(FVector2D(ScreenPos), false, false, false, true);

	if (TraceResult.IsValidIndex(0))
	{

		// Get the first found object in the line trace - ignoring the rest of the array elements
		auto TrackedTF = TraceResult[0].GetLocalToWorldTransform();

		const FActorSpawnParameters SpawnInfo;
		const FRotator MyRot(0, 0, 0);
		const FVector MyLoc = TrackedTF.GetLocation();

		// If the current plane is not yet spawned
		if (!bPlaneSpawned)
		{
			PlayArea = GetWorld()->SpawnActor<APlayAreaPlane>(MyLoc, MyRot, SpawnInfo);
			bPlaneSpawned = true;
		}

		// Add a vertex to the plane
		PlayArea->AddVertex(MyLoc);			
	}
}

