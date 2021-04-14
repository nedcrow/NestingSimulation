// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SimulationWidgetBase.generated.h"

/**
 * 
 */
UCLASS()
class NESTINGSIMULATOR_API USimulationWidgetBase : public UUserWidget
{
	GENERATED_BODY()

protected:
		virtual void NativeConstruct() override;

public:
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Widget")
		class UComboBoxString* NestingTypeDropDown;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Widget")
		class UCheckBoxWidgetBase* AutoFilter;

	UFUNCTION()
		void OnCheckedAutoFilter(bool bIsChecked);
};
