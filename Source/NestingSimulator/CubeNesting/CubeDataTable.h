// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/DataTable.h"
#include "CubeDataTable.generated.h"

USTRUCT(BlueprintType)
struct FCubeListTableRow : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString Type;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float SizeX;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float SizeY;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float SizeZ;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Weight;
};

UCLASS()
class NESTINGSIMULATOR_API ACubeDataTable : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACubeDataTable();

};