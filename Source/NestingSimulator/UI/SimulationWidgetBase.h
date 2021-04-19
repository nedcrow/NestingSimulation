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
		bool AutoNesting = false;
		void SetUMGProperties();
		UPROPERTY()
		class ABoxNesting* BoxNesting;

public:
	// NestingType
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Widget")
		class UComboBoxString* NestingTypeDropDown;

	// AutoFilter
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Widget")
		class UCheckBoxWidgetBase* AutoFilter;
	UFUNCTION()
		void OnCheckedAutoFilter(bool bIsChecked);

	// CanBoxRotating
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Widget")
		class UCheckBoxWidgetBase* CanBoxRotating;
	UFUNCTION()
		void OnCheckedBoxRotating(bool bIsChecked);

	// CanColoring_BoxArea
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Widget")
		class UCheckBoxWidgetBase* CanColoring_BoxArea;
	UFUNCTION()
		void OnCheckedBoxArea(bool bIsChecked);

	// CanColoring_BoxGroup
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Widget")
		class UCheckBoxWidgetBase* CanGrouping_BoxType;
	UFUNCTION()
		void OnCheckedBoxGroup(bool bIsChecked);

	// AlignOption
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Widget")
		class UComboBoxString* AlignDropDown;
	UFUNCTION()
	void OnChangedAlign(FString SelectedItem, ESelectInfo::Type SelectionType);

	// Board
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Widget")
		class UEditableText* BoardSizeX;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Widget")
		class UEditableText* BoardSizeY;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Widget")
		class UEditableText* BoardPaddingX;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Widget")
		class UEditableText* BoardPaddingY;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Widget")
		class UEditableText* BoardMargin;

	UFUNCTION()
		void OnChangedBoardSizeX(const FText& Text, ETextCommit::Type Method);
	UFUNCTION()
		void OnChangedBoardSizeY(const FText& Text, ETextCommit::Type Method);
	UFUNCTION()
		void OnChangedBoardPaddingX(const FText& Text, ETextCommit::Type Method);
	UFUNCTION()
		void OnChangedBoardPaddingY(const FText& Text, ETextCommit::Type Method);
	UFUNCTION()
		void OnChangedBoardMargin(const FText& Text, ETextCommit::Type Method);

	// Box
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Widget")
		class UEditableText* BoxMin;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Widget")
		class UEditableText* BoxMax;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Widget")
		class UEditableText* BoxMargin;
	UFUNCTION()
		void OnChangedBoxMin(const FText& Text, ETextCommit::Type Method);
	UFUNCTION()
		void OnChangedBoxMax(const FText& Text, ETextCommit::Type Method);
	UFUNCTION()
		void OnChangedBoxMargin(const FText& Text, ETextCommit::Type Method);

	// NestButton
    UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Widget")
		class UButton* NestButton;
	UFUNCTION()
		void OnClickedNestButton();

	// AutoButton		
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Widget")
		class UCheckBox* AutoCheckBox;
};
