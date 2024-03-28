// Fill out your copyright notice in the Description page of Project Settings.


#include "AREmulatorPawn.h"
#include "AREmulaterGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values
AAREmulatorPawn::AAREmulatorPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	SetRootComponent(SceneComponent);

	// Camera compentent attached to the pawn
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(SceneComponent);
}

// Called when the game starts or when spawned
void AAREmulatorPawn::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AAREmulatorPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FRotator newRotation = GetActorRotation();

	newRotation.Yaw += CameraInput.X;
	newRotation.Pitch += CameraInput.Y;

	//UKismetSystemLibrary::PrintString(this, newRotation.Pitch, true, true, FLinearColor(0, 0.66, 1, 1), 2);

	SetActorRotation(newRotation);
}

// Called to bind functionality to input
void AAREmulatorPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("Forward", this, &AAREmulatorPawn::MoveForwards);
	PlayerInputComponent->BindAxis("Strafe", this, &AAREmulatorPawn::MoveStrafe);
	PlayerInputComponent->BindAxis("Vertical", this, &AAREmulatorPawn::MoveVertical);
	PlayerInputComponent->BindAxis("Pitch", this, &AAREmulatorPawn::LookPitch);
	PlayerInputComponent->BindAxis("Yaw", this, &AAREmulatorPawn::LookYaw);
	PlayerInputComponent->BindTouch(IE_Pressed, this, &AAREmulatorPawn::OnScreenTouch);
}

void AAREmulatorPawn::MoveForwards(float scale) 
{
	FVector movement = GetActorForwardVector();

	movement *= scale * GetWorld()->GetDeltaSeconds() * speedForwards;
	AddActorWorldOffset(movement);
}

void AAREmulatorPawn::MoveStrafe(float scale) 
{
	FVector movement = GetActorRightVector();
	movement *= scale * GetWorld()->GetDeltaSeconds() * speedStrafe;
	AddActorWorldOffset(movement);
}

void AAREmulatorPawn::MoveVertical(float scale) 
{
	FVector movement = GetActorUpVector();
	movement *= scale * GetWorld()->GetDeltaSeconds() * speedVertical;
	AddActorWorldOffset(movement);
}

void AAREmulatorPawn::LookYaw(float scale)
{
	CameraInput.X = scale;
}

void AAREmulatorPawn::LookPitch(float scale) 
{
	CameraInput.Y = scale;
}

void AAREmulatorPawn::OnScreenTouch(const ETouchIndex::Type FingerIndex, const FVector ScreenPos)
{
	// Get access to gameMode to interact with the ARManager
	auto Temp = GetWorld()->GetAuthGameMode();
	auto GM = Cast<AAREmulaterGameMode>(Temp);

	// Debug to screen space
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, TEXT("ScreenTouch Reached"));

	FHitResult ActorHitResult;

	// If raytrace hits something
	if (WorldHitTest(FVector2D(ScreenPos.X, ScreenPos.Y), ActorHitResult))
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, TEXT("ScreenTouch Reached"));
		//// Debug to screen space
		//GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Purple, TEXT("HitTestSuccessful"));

		//// Get object of actor hit.
		//UClass* hitActorClass = UGameplayStatics::GetObjectClass(ActorHitResult.GetActor());

		//// If hit actor belopngs to APlaceableActor
		//if (UKismetMathLibrary::ClassIsChildOf(hitActorClass, APlaceableActor::StaticClass()))
		//{
		//	// If a previous actor is slected remove the redMat so it doesn't look selecetd
		//	if (SelectedActor != NULL)
		//	{
		//		SelectedActor->StaticMeshComponent->SetMaterial(0, defaultMat);
		//	}

		//	// Track the last selected actor
		//	SelectedActor = Cast<APlaceableActor>(ActorHitResult.GetActor());
		//	SelectedActor->StaticMeshComponent->SetMaterial(0, redMat);


		//	GM->ARManager->SetPlaneVisibility(false);

		//	GM->ARManager->ShouldCreateNewPlanes(false);
		//}

		//isFingerDown = true;
	}
	// If an actor is already selected
	//else if (SelectedActor != NULL)
	//{
	//	// Deselect the previous actor
	//	SelectedActor->StaticMeshComponent->SetMaterial(0, defaultMat);
	//	SelectedActor = NULL;

	//	GM->ARManager->SetPlaneVisibility(true);

	//	GM->ARManager->ShouldCreateNewPlanes(true);
	//}
	// If no actor is selected
	else if (GM)
	{
		// Add call to the line-trace here from the Custom Game Mode
		GM->LineTraceSpawnActor(ScreenPos);
	}
}

bool AAREmulatorPawn::WorldHitTest(FVector2D screenPos, FHitResult& hitResult)
{
	// Get player controller
	APlayerController* playerController = UGameplayStatics::GetPlayerController(this, 0);

	// Perform deprojection taking 2d clicked area and generating reference in 3d world-space.
	FVector worldPosition;
	FVector worldDirection; // Unit Vector

	bool deprojectionSuccess = UGameplayStatics::DeprojectScreenToWorld(playerController, screenPos, /*out*/ worldPosition, /*out*/ worldDirection);

	// construct trace vector (from point clicked to 1000.0 units beyond in same direction)
	FVector traceEndVector = worldDirection * 1000.0;
	traceEndVector = worldPosition + traceEndVector;

	// perform line trace (Raycast)
	bool traceSuccess = GetWorld()->LineTraceSingleByChannel(hitResult, worldPosition, traceEndVector, ECollisionChannel::ECC_WorldDynamic);

	// return if the operation was successful
	return traceSuccess;
}
