// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "NestingGS.generated.h"

/**
 * 
 */
UCLASS()
class NESTINGSIMULATOR_API ANestingGS : public AGameStateBase
{
	GENERATED_BODY()

protected:
	FVector2D CurrentBoardSize;
	int CurrentBoardDistance;
	int CurrentBoardCount;

public:
	// Centimeter
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	float Unit = 100;

	/* Alpha is distance of that board and diffrent board */
	void SetCurrentBoardSize(int X, int Y, int Alpha);

	void SetCurrentBoardCount(int Count);

	/* Return x_y size is centimeter */
	FVector2D GetSizeOfBoard();

	int GetAreaOfBoards();
};
