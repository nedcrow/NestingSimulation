// Fill out your copyright notice in the Description page of Project Settings.


#include "NestingPlayerController.h"
#include "UI/SimulationWidgetBase.h"

void ANestingPlayerController::BeginPlay() {
	Super::BeginPlay();

	if (SimulationWidgetClass && IsLocalPlayerController()) {
		SimulationWidgetObject = CreateWidget<USimulationWidgetBase>(this, SimulationWidgetClass);
		SimulationWidgetObject->AddToViewport();
		bShowMouseCursor = true;
	}
}
