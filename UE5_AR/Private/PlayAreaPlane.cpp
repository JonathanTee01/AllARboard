// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayAreaPlane.h"
#include "ProceduralMeshComponent.h"
#include "Components/InstancedStaticMeshComponent.h"
#include <Kismet/KismetSystemLibrary.h>

// Sets default values
APlayAreaPlane::APlayAreaPlane()
{
	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	SetRootComponent(SceneComponent);

	PlanePolygonMeshComponent = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("PlanePolygonMesh"));
	PlanePolygonMeshComponent->SetupAttachment(SceneComponent);
	PlanePolygonMeshComponent->SetGenerateOverlapEvents(true);

	// Take material from editor
	auto MaterialAsset = ConstructorHelpers::FObjectFinder<UMaterial>(TEXT("/Game/Assets/Materials/Red_Mat.Red_Mat"));
	Material_ = MaterialAsset.Object;

	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void APlayAreaPlane::BeginPlay()
{
	Super::BeginPlay();
	PlaneMaterial = UMaterialInstanceDynamic::Create(Material_, this);
	PlaneMaterial->SetScalarParameterValue("TextureRotationAngle", FMath::RandRange(0.0f, 1.0f));
	PlanePolygonMeshComponent->SetMaterial(0, PlaneMaterial);
}

// Called every frame
void APlayAreaPlane::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APlayAreaPlane::AddVertex(FVector point) 
{
	// Get point in local space
	FVector newVertex = point - GetActorLocation();

	// Record the vertex
	Vertices.Add(newVertex);

	SortVertexOrder();

	UpdatePlanePolygonMesh();
}

// Order the vertices so they're stored anti-clockwise
void APlayAreaPlane::SortVertexOrder()
{
	TArray<FVector> shortenedArray;

	// Find and remove the subsumed point if there's more than 8
	int32 maxVertexes = 32;
	if (Vertices.Num() > maxVertexes)
	{
		// Find the centre of the vertices
		FVector center;
		for (int i = 0; i < Vertices.Num(); i++) 
		{
			center += Vertices[i];		
		}
		center /= Vertices.Num();

		// Find the point closest to the centre and remove it
		int32 closestVertexToCenter = 0;
		int32 distanceToClosestVertex = FVector::Distance(center, Vertices[0]);

		for (int i = 1; i < Vertices.Num(); i++)
		{
			int32 distanceToCurrentVertex = FVector::Distance(center, Vertices[i]);

			// If closer than current closest
			if (distanceToCurrentVertex < distanceToClosestVertex) 
			{
				closestVertexToCenter = i;
				distanceToClosestVertex = distanceToCurrentVertex;
			}
		}

		// Add all points to a new array while excluding the subsumed point
		for (int i = 0; i < Vertices.Num(); i++)
		{
			if (i != closestVertexToCenter) 
			{
				shortenedArray.Add(Vertices[i]);
			}
		}

		Vertices = shortenedArray;
	}

	// Variables to track and organise the vertices
	TArray<FVector> sortedArray;
	FVector nextVertex;

	bool vertexAdded = false;
	// Find the point with the highest Y value
	for (int i = 0; i < Vertices.Num(); i++)
	{

		if (vertexAdded) {
			// Find the top-most point
			if (Vertices[i].Y > nextVertex.Y)
			{
				nextVertex = Vertices[i];
			}
		}
		else 
		{
			nextVertex = Vertices[i];
			vertexAdded = true;
		}
	}
	sortedArray.Add(nextVertex);

	for (int j = 0; j < Vertices.Num(); j++)
	{
		vertexAdded = false;
		int32 stage = 0;

		for (int i = 0; i < Vertices.Num(); i++) 
		{
			// If current point is not in the sortedArray already
			if (!sortedArray.Contains(Vertices[i])) 
			{
				if (Vertices[i].Y <= sortedArray.Last().Y && Vertices[i].X >= sortedArray.Last().X && (stage == 0 || stage >= 1))
				{
					// If theres no vertex being considered currently
					if (!vertexAdded || stage > 1) 
					{
						vertexAdded = true;
						nextVertex = Vertices[i];
					}
					else 
					{
						if (Vertices[i].Y > nextVertex.Y) 
						{
							nextVertex = Vertices[i];
						}
					}
					stage = 1;
					continue;
				}

				else if (Vertices[i].Y <= sortedArray.Last().Y && Vertices[i].X <= sortedArray.Last().X && (stage == 0 || stage >= 2))
				{
					// If theres no vertex being considered currently
					if (!vertexAdded || stage > 2)
					{
						vertexAdded = true;
						nextVertex = Vertices[i];
					}
					else
					{
						if (Vertices[i].X > nextVertex.X)
						{
							nextVertex = Vertices[i];
						}
					}
					stage = 2;
					continue;
				}

				else if (Vertices[i].Y >= sortedArray.Last().Y && Vertices[i].X <= sortedArray.Last().X && (stage == 0 || stage >= 3))
				{
					// If theres no vertex being considered currently
					if (!vertexAdded || stage > 3)
					{
						vertexAdded = true;
						nextVertex = Vertices[i];
					}
					else
					{
						if (Vertices[i].Y < nextVertex.Y)
						{
							nextVertex = Vertices[i];
						}
					}
					stage = 3;
					continue;
				}

				else if (Vertices[i].Y >= sortedArray.Last().Y && Vertices[i].X >= sortedArray.Last().X && (stage == 0 || stage >= 4))
				{
					// If theres no vertex being considered currently
					if (!vertexAdded)
					{
						vertexAdded = true;
						nextVertex = Vertices[i];
					}
					else
					{
						if (Vertices[i].X < nextVertex.X)
						{
							nextVertex = Vertices[i];
						}
					}
					stage = 4;
					continue;
				}
			}
		}

		if (vertexAdded)
		{
			sortedArray.Add(nextVertex);
		}
	}
	Vertices = sortedArray;
}

void APlayAreaPlane::UpdatePlanePolygonMesh()
{
	// Update polygon mesh vertex indices, using triangle fan due to its convex.
	TArray<FVector> BoundaryVertices;
	// Obtain the boundary vertices from ARCore's plane geometry
	BoundaryVertices = Vertices;
	int BoundaryVerticesNum = BoundaryVertices.Num();

	if (BoundaryVerticesNum < 3)
	{
		PlanePolygonMeshComponent->ClearMeshSection(0);
		return;
	}

	int PolygonMeshVerticesNum = BoundaryVerticesNum;
	// Triangle number is interior(n-2 for convex polygon) plus perimeter (EdgeNum * 2);
	int TriangleNum = BoundaryVerticesNum - 2 + BoundaryVerticesNum;

	TArray<FVector> PolygonMeshVertices;
	TArray<FLinearColor> PolygonMeshVertexColors;
	TArray<int> PolygonMeshIndices;
	TArray<FVector> PolygonMeshNormals;
	TArray<FVector2D> PolygonMeshUVs;

	PolygonMeshVertices.Empty(PolygonMeshVerticesNum);
	PolygonMeshVertexColors.Empty(PolygonMeshVerticesNum);
	PolygonMeshIndices.Empty(TriangleNum * 3);
	PolygonMeshNormals.Empty(PolygonMeshVerticesNum);

	// Creating the triangle fan from the vertices obtained
	FVector PlaneNormal = FVector::UpVector;
	for (int i = 0; i < BoundaryVerticesNum; i++)
	{
		FVector BoundaryPoint = BoundaryVertices[i];
		float BoundaryToCenterDist = BoundaryPoint.Size();
		float FeatheringDist = FMath::Min(BoundaryToCenterDist, EdgeFeatheringDistance);
		FVector InteriorPoint = BoundaryPoint - BoundaryPoint.GetUnsafeNormal() * FeatheringDist;

		PolygonMeshVertices.Add(BoundaryPoint);

		PolygonMeshUVs.Add(FVector2D(BoundaryPoint.X, BoundaryPoint.Y));

		PolygonMeshNormals.Add(PlaneNormal);

		PolygonMeshVertexColors.Add(FLinearColor(0.0f, 0.f, 0.f, 0.f));
	}

	// add triangles
	for (int i = 2; i < PolygonMeshVerticesNum; i++)
	{
		PolygonMeshIndices.Add(0);
		PolygonMeshIndices.Add(i - 1);
		PolygonMeshIndices.Add(i);
	}

	// No need to fill uv and tangent;
	PlanePolygonMeshComponent->CreateMeshSection_LinearColor(0, PolygonMeshVertices, PolygonMeshIndices, PolygonMeshNormals, PolygonMeshUVs, PolygonMeshVertexColors, TArray<FProcMeshTangent>(), true);
}

void APlayAreaPlane::UpdatePlaneMaterial(UMaterialInterface* Mat)
{
	PlanePolygonMeshComponent->SetMaterial(0, Mat);
}

TArray<FVector> APlayAreaPlane::GetVertices()
{
	return Vertices;
}

