// Fill out your copyright notice in the Description page of Project Settings.


#include "NestingPlayerController.h"
#include "UI/SimulationWidgetBase.h"

void ANestingPlayerController::BeginPlay() {
	Super::BeginPlay();

	if (IsLocalPlayerController()) {
		if (SimulationWidgetClass) {
			SimulationWidgetObject = CreateWidget<USimulationWidgetBase>(this, SimulationWidgetClass);
			if (SimulationWidgetObject) SimulationWidgetObject->AddToViewport();
			bShowMouseCursor = true;
		}
	}
}
