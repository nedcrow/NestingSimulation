// Fill out your copyright notice in the Description page of Project Settings.


#include "CubeBoard.h"
#include "../NestingGS.h"
#include "Components/SphereComponent.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ACubeBoard::ACubeBoard()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	RootComponent = Sphere;

	BoardISM = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("BoardISM"));
	BoardISM->SetupAttachment(Sphere);

}

// Called when the game starts or when spawned
void ACubeBoard::BeginPlay()
{
	Super::BeginPlay();
	//SetBoardSize(FVector(20,12,10)); // Test
}

void ACubeBoard::SetBoardSize(FVector Size)
{
	BoardISM->ClearInstances();

	ANestingGS* GS = Cast<ANestingGS>(UGameplayStatics::GetGameState(GetWorld()));
	if (GS) {
		FRotator rotator;
		FVector location;
		FVector scale;
		for (auto Pivot : PivotArray) {
			rotator = FRotator(
				Pivot.X == 0 ? 0 : 90 * Pivot.X,
				0,
				Pivot.Y == 0 ? 0 : 90 * Pivot.Y * -1
			);

			location = FVector(
				Size.X * Pivot.X * GS->Unit * 0.5f,
				Size.Y * Pivot.Y * GS->Unit * 0.5f,
				Size.Z * Pivot.Z * GS->Unit * 0.5f
			);

			scale = FVector(
				(Pivot.X == 0 ? Size.X : Size.Z),
				(Pivot.Y == 0 ? Size.Y : Size.Z),
				1
			);

			BoardISM->AddInstance(FTransform(
				rotator,
				location,
				scale
			));
		}
	}	
}

void ACubeBoard::CheckCubeInMatrix(FVector StartPos, FVector CubeSize)
{
	for (int x = 0; x < StartPos.X + CubeSize.X; x++) {
		for (int y = 0; y < StartPos.Y + CubeSize.Y; y++) {
			for (int z = 0; z < StartPos.Z + CubeSize.Z; z++) {
				BoardMatrix[x].RowArr[y].SpaceArr[z] = 0;
			}
		}
	}

	EmptySpaceCount -= CubeSize.X * CubeSize.Y * CubeSize.Z;
}

void ACubeBoard::CheckPaddingInMatrix(FVector PaddingXYZ)
{

}

int ACubeBoard::GetEmptySpaceCount()
{
	return EmptySpaceCount;
}

