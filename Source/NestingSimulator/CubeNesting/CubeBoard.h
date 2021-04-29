// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CubeBoard.generated.h"

USTRUCT(Atomic, BlueprintType)
struct FCubeRowStruct {
	GENERATED_BODY()

public:
	TArray<int8> SpaceArr;
};

USTRUCT(Atomic, BlueprintType)
struct FCubeFaceStruct {
	GENERATED_BODY()

public:
	TArray<FCubeRowStruct> RowArr;
};

UCLASS()
class NESTINGSIMULATOR_API ACubeBoard : public AActor
{
	GENERATED_BODY()

	
public:	
	// Sets default values for this actor's properties
	ACubeBoard();
	class USphereComponent* Sphere;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		class UInstancedStaticMeshComponent* BoardISM;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<FCubeFaceStruct> BoardMatrix;

	void SetBoardSize(FVector Size);
	void CheckCubeInMatrix(FVector StartPos, FVector CubeSize);
	void CheckPaddingInMatrix(FVector PaddingXYZ);
	int GetEmptySpaceCount();

private:
	/* under, foward, back, left, right */
	TArray<FVector> PivotArray = {FVector(0,0,-1), FVector(1,0,0), FVector(-1,0,0), FVector(0,-1,0), FVector(0,1,0) };
	int EmptySpaceCount;
};
