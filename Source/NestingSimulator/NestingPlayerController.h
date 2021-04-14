// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "NestingPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class NESTINGSIMULATOR_API ANestingPlayerController : public APlayerController
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay() override;

public:
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "UI")
		TSubclassOf<class USimulationWidgetBase> SimulationWidgetClass;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "UI")
		class USimulationWidgetBase* SimulationWidgetObject;
};
