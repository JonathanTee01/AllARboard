// Fill out your copyright notice in the Description page of Project Settings.


#include "StaticActor.h"
#include "CustomGameMode.h"

// Sets default values
AStaticActor::AStaticActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	SetRootComponent(SceneComponent);

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	StaticMeshComponent->SetupAttachment(SceneComponent);
}

// Called when the game starts or when spawned
void AStaticActor::BeginPlay()
{
	Super::BeginPlay();
	int random = UKismetMathLibrary::RandomInteger64InRange(0, 2);

	if (random == 0)
	{
		// Add to RED class
		StaticMeshComponent->SetMaterial(0, RedMaterial);
		Color = ColorEnum::Red;
	}
	else if (random == 1)
	{
		// Add to GREEN class
		StaticMeshComponent->SetMaterial(0, GreenMaterial);
		Color = ColorEnum::Green;
	}
	else if (random == 2)
	{
		// Add to BLUE class
		StaticMeshComponent->SetMaterial(0, BlueMaterial);
		Color = ColorEnum::Blue;
	}
}

// Called every frame
void AStaticActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Making sure the actor remains on the ARPin that has been found.
	if (PinComponent)
	{
		auto TrackingState = PinComponent->GetTrackingState();

		switch (TrackingState)
		{
		case EARTrackingState::Tracking:
			SceneComponent->SetVisibility(true);
			SetActorTransform(PinComponent->GetLocalToWorldTransform());

			// Scale down default cube mesh - Change this for your applications.
			SetActorScale3D(FVector(0.01f, 0.01f, 0.01f));
			break;

		case EARTrackingState::NotTracking:
			PinComponent = nullptr;

			break;

		}
	}

	// If it is a workplace then steadily increase the timer
	if (ActorHasTag("Workplace")) 
	{
		auto Temp = GetWorld()->GetAuthGameMode();
		auto GM = Cast<ACustomGameMode>(Temp);
		
		if (GM->GetStateEnum() == StateEnum::Playing) 
		{
			Timer += DeltaTime;
			if (Timer >= MaxTimer) 
			{
				GM->SetStateEnum(StateEnum::Lose);
			}
		}
	}
}

// Reduce the timer by a specified amount
void AStaticActor::ReduceTimer(float ReductionAmount)
{
	Timer -= ReductionAmount;
	if (Timer < 0) Timer = 0;
}

