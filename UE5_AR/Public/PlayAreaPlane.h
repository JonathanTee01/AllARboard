// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PlayAreaPlane.generated.h"

/// <summary>
/// Class for creating user specified planes
/// </summary>

UCLASS()
class UE5_AR_API APlayAreaPlane : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APlayAreaPlane();

	UPROPERTY(Category = "myCategory", VisibleAnywhere, BlueprintReadWrite)
	USceneComponent* SceneComponent;

	/* The procedural mesh component */
	UPROPERTY(Category = GoogleARCorePlaneActor, EditAnywhere, BlueprintReadWrite)
	class UProceduralMeshComponent* PlanePolygonMeshComponent;

	UPROPERTY(Category = GoogleARCorePlaneActor, EditAnywhere, BlueprintReadWrite)
	UMaterialInstanceDynamic* PlaneMaterial;

	/** The feathering distance for the polygon edge. Default to 10 cm*/
	UPROPERTY(Category = GoogleARCorePlaneActor, EditAnywhere, BlueprintReadWrite)
	float EdgeFeatheringDistance = 10.0f;

	UMaterialInterface* Material_;

	TArray<FVector> Vertices;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void SortVertexOrder();

	virtual void UpdatePlanePolygonMesh();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void AddVertex(FVector point);

	virtual TArray<FVector> GetVertices();

	virtual void UpdatePlaneMaterial(UMaterialInterface* Mat);
};
