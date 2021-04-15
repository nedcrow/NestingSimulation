// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CheckBoxWidgetBase.generated.h"

/**
 * 
 */
UCLASS()
class NESTINGSIMULATOR_API UCheckBoxWidgetBase : public UUserWidget
{
	GENERATED_BODY()
protected:
	virtual void NativeConstruct()override;

public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Widget")
		FName DefaultText="Text";

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Widget")
		class UTextBlock* ChildBoxText;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Widget")
		class UCheckBox* ChildCheckBox;
};
