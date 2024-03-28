// Fill out your copyright notice in the Description page of Project Settings.


#include "PlaceableActor.h"
#include <CustomGameMode.h>

// Sets default values
APlaceableActor::APlaceableActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	SetRootComponent(SceneComponent);


	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	StaticMeshComponent->SetupAttachment(SceneComponent);
}

// Called when the game starts or when spawned
void APlaceableActor::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void APlaceableActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	// Making sure the actor remains on the ARPin that has been found.
	if(PinComponent)
	{
		auto TrackingState = PinComponent->GetTrackingState();
		
		switch (TrackingState)
		{
		case EARTrackingState::Tracking:
			SceneComponent->SetVisibility(true);
			SetActorTransform(PinComponent->GetLocalToWorldTransform());

			// Scale down default cube mesh - Change this for your applications.
			SetActorScale3D(FVector(0.2f, 0.2f, 0.2f));
			break;

		case EARTrackingState::NotTracking:
			PinComponent = nullptr;

			break;
		}
	}

}

void APlaceableActor::CheckNearbyBuildings(const float MaxDistToActor, const bool AddToTracker = false)
{
	// Find static actors. That includes houses and workplaces
	TArray<AActor*, FDefaultAllocator> AllBuildings;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("Static"), AllBuildings);

	// Filter all static buildings by proximity
	TArray<AStaticActor*, FDefaultAllocator> NearbyBuildings;
	for (int i = 0; i < AllBuildings.Num(); i++)
	{
		double dist = FVector::Distance(StaticMeshComponent->GetComponentLocation(), Cast<AStaticActor>(AllBuildings[i])->StaticMeshComponent->GetComponentLocation());
		if (dist < MaxDistToActor) 
		{
			NearbyBuildings.Add(Cast<AStaticActor>(AllBuildings[i]));
		}
	}

	// If tracking. Currently is nver not tracking when called but was planned for use with the train and rail tracks.
	if (AddToTracker) 
	{
		// Empty arrays to avoid duplicates
		NearbyColors.Empty();
		NearbyWorkplaces.Empty();

		// Loop and sort all nearby buildings
		for (int i = 0; i < NearbyBuildings.Num(); i++) 
		{
			if (NearbyBuildings[i]->ActorHasTag("House")) 
			{
				NearbyColors.Add(NearbyBuildings[i]->Color);
			}
			else if (NearbyBuildings[i]->ActorHasTag("Workplace")) 
			{
				NearbyWorkplaces.Add(NearbyBuildings[i]);
			}
		}
	}
}

void APlaceableActor::AddColorCharges()
{
	auto Temp = GetWorld()->GetAuthGameMode();
	auto GM = Cast<ACustomGameMode>(Temp);
	
	// For each nearby colour add a charge to the counter
	for (int i = 0; i < NearbyColors.Num(); i++) 
	{
		switch (NearbyColors[i])
		{
		case (ColorEnum::Red):
			GM->GetGameState<ACustomGameState>()->RedCharges += 1;
			break;
		case (ColorEnum::Green):
			GM->GetGameState<ACustomGameState>()->GreenCharges += 1;
			break;
		case (ColorEnum::Blue):
			GM->GetGameState<ACustomGameState>()->BlueCharges += 1;
			break;
		}
	}
}

void APlaceableActor::SupplyWorkplaces() 
{
	auto Temp = GetWorld()->GetAuthGameMode();
	auto GM = Cast<ACustomGameMode>(Temp);

	// For each nearby workplace add reduce it's timer and consume a charge
	for (int i = 0; i < NearbyWorkplaces.Num(); i++) 
	{
		if (NearbyColors.Contains(NearbyWorkplaces[i]->Color)) 
		{
			switch (NearbyWorkplaces[i]->Color)
			{
			case (ColorEnum::Red):
				if (GM->GetGameState<ACustomGameState>()->RedCharges > 0)
				{
					GM->GetGameState<ACustomGameState>()->RedCharges -= 1;
					NearbyWorkplaces[i]->ReduceTimer(20);
				}
				break;
			case (ColorEnum::Green):
				if (GM->GetGameState<ACustomGameState>()->GreenCharges > 0)
				{
					GM->GetGameState<ACustomGameState>()->GreenCharges -= 1;
					NearbyWorkplaces[i]->ReduceTimer(20);
				}
				break;
			case (ColorEnum::Blue):
				if (GM->GetGameState<ACustomGameState>()->BlueCharges > 0)
				{
					GM->GetGameState<ACustomGameState>()->BlueCharges -= 1;
					NearbyWorkplaces[i]->ReduceTimer(20);
				}
				break;
			}
		}
	}
}

