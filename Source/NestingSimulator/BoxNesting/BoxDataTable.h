// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/DataTable.h"
#include "BoxDataTable.generated.h"

USTRUCT(BlueprintType)
struct FBoxListTableRow : public FTableRowBase
{
    GENERATED_BODY()

public:

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString Type;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 SizeX;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 SizeY;
};

UCLASS()
class NESTINGSIMULATOR_API ABoxDataTable : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABoxDataTable();

};
