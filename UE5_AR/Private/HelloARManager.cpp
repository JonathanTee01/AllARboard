// Fill out your copyright notice in the Description page of Project Settings.


#include "HelloARManager.h"
#include "ARPlaneActor.h"
#include "ARPin.h"
#include "ARSessionConfig.h"
#include "ARBlueprintLibrary.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"

// Sets default values
AHelloARManager::AHelloARManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// This constructor helper is useful for a quick reference within UnrealEngine and if you're working alone. But if you're working in a team, this could be messy.
	// If the artist chooses to change the location of an art asset, this will throw errors and break the game.
	// Instead let unreal deal with this. Usually avoid this method of referencing.
	// For long term games, create a Data-Only blueprint (A blueprint without any script in it) and set references to the object using the blueprint editor.
	// This way, unreal will notify your artist if the asset is being used and what can be used to replace it.
	static ConstructorHelpers::FObjectFinder<UARSessionConfig> ConfigAsset(TEXT("ARSessionConfig'/Game/Blueprints/HelloARSessionConfig.HelloARSessionConfig'"));
	Config = ConfigAsset.Object;


	//Populate the plane colours array
	PlaneColors.Add(FColor::Blue);
	PlaneColors.Add(FColor::Red);
	PlaneColors.Add(FColor::Green);
	PlaneColors.Add(FColor::Cyan);
	PlaneColors.Add(FColor::Magenta);
	PlaneColors.Add(FColor::Emerald);
	PlaneColors.Add(FColor::Orange);
	PlaneColors.Add(FColor::Purple);
	PlaneColors.Add(FColor::Turquoise);
	PlaneColors.Add(FColor::White);
	PlaneColors.Add(FColor::Yellow);

	// Initialise the blank material
	auto blankMaterial = ConstructorHelpers::FObjectFinder<UMaterial>(TEXT("Material'/Game/Assets/Materials/ARPlane_Mat.ARPlane_Mat'"));

	// Initialise plane creation toggle
	createNewPlanes = true;
}

// Called when the game starts or when spawned
void AHelloARManager::BeginPlay()
{
	Super::BeginPlay();	
}

// Start an AR session
void AHelloARManager::StartARCoreSession() 
{
	//Start the AR Session
	UARBlueprintLibrary::StartARSession(Config);
}

// Called every frame
void AHelloARManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	switch (UARBlueprintLibrary::GetARSessionStatus().Status)
	{
	case EARSessionStatus::Running:
		
		UpdatePlaneActors();
		break;

	case EARSessionStatus::FatalError:

		ResetARCoreSession();
		UARBlueprintLibrary::StartARSession(Config);
		break;
	}
}



//Updates the geometry actors in the world
void AHelloARManager::UpdatePlaneActors()
{
	if (!createNewPlanes) return;

	//Get all world geometries and store in an array
	auto Geometries = UARBlueprintLibrary::GetAllGeometriesByClass<UARPlaneGeometry>();
	bool bFound = false;

	//Loop through all geometries
	for (auto& It : Geometries)
	{
		//Check if current plane exists 
		if (PlaneActors.Contains(It))
		{
			AARPlaneActor* CurrentPActor = *PlaneActors.Find(It);

			//Check if plane is subsumed
			if (It->GetSubsumedBy()->IsValidLowLevel())
			{
				CurrentPActor->Destroy();
				PlaneActors.Remove(It);
				break;
			}
			else
			{
				//Get tracking state switch
				switch (It->GetTrackingState())
				{
					//If tracking update
				case EARTrackingState::Tracking:
					CurrentPActor->UpdatePlanePolygonMesh();
					break;
					//If not tracking destroy the actor and remove from map of actors
				case EARTrackingState::StoppedTracking:
					CurrentPActor->Destroy();
					PlaneActors.Remove(It);
					break;
				}
			}
		}
		else 
		{
			//Get tracking state switch
			switch (It->GetTrackingState())
			{

			case EARTrackingState::Tracking:
				if (!It->GetSubsumedBy()->IsValidLowLevel())
				{
					PlaneActor = SpawnPlaneActor();
					PlaneActor->SetColor(GetPlaneColor(PlaneIndex));
					PlaneActor->ARCorePlaneObject = It;

					PlaneActors.Add(It, PlaneActor);
					PlaneActor->UpdatePlanePolygonMesh();
					PlaneIndex++;
				}
				break;
			}
		}

	}
}

//Simple spawn function for the tracked AR planes
AARPlaneActor* AHelloARManager::SpawnPlaneActor()
{
	const FActorSpawnParameters SpawnInfo;
	const FRotator MyRot(0, 0, 0);
	const FVector MyLoc(0, 0, 0);

	AARPlaneActor* CustomPlane = GetWorld()->SpawnActor<AARPlaneActor>(MyLoc, MyRot, SpawnInfo);

	return CustomPlane;
}

//Gets the colour to set the plane to when its spawned
FColor AHelloARManager::GetPlaneColor(int Index)
{
	return PlaneColors[Index % PlaneColors.Num()];
}

void AHelloARManager::ResetARCoreSession()
{

	//Get all actors in the level and destroy them as well as emptying the respective arrays
	TArray<AActor*> Planes;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AARPlaneActor::StaticClass(), Planes);

	for ( auto& It : Planes)
		It->Destroy();
	
	Planes.Empty();
	PlaneActors.Empty();

}

// Simple function to toggle the creation of new planes
void AHelloARManager::ShouldCreateNewPlanes(bool shouldCreate) 
{
	createNewPlanes = shouldCreate;
}

// Simple function to toggle plane rendering
void AHelloARManager::SetPlaneVisibility(bool isVisible) 
{
	for (auto& i : PlaneActors)
	{
		AARPlaneActor* plane = i.Value; 

		if (isVisible) 
		{
			// Set to tracking. Flip the bool as this will only last a frame before changing to tracking itself.
			plane->ARCorePlaneObject->SetTrackingState(EARTrackingState::Tracking);
			plane->bShouldRender = true;
		}

		else
		{
			// Set to not tracking. Flip the bool as this will only last a frame before changing to tracking itself.
			plane->ARCorePlaneObject->SetTrackingState(EARTrackingState::NotTracking); 
			plane->bShouldRender = false;
		}
	}
}