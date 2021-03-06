// Fill out your copyright notice in the Description page of Project Settings.


#include "SimulationWidgetBase.h"
#include "../BoxNesting/BoxNesting.h"
#include "CheckBoxWidgetBase.h"
#include "Components/Button.h"
#include "Components/CheckBox.h"
#include "Components/ComboBoxString.h"
#include "Components/EditableText.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "../Utility.h"

void USimulationWidgetBase::NativeConstruct() {
	Super::NativeConstruct();
	NestingTypeDropDown = Cast<UComboBoxString>(GetWidgetFromName(TEXT("NestingTypeDropDown")));
	AutoFilter = Cast<UCheckBoxWidgetBase>(GetWidgetFromName(TEXT("AutoFilter")));
	CanBoxRotating = Cast<UCheckBoxWidgetBase>(GetWidgetFromName(TEXT("CanBoxRotating")));
	CanColoring_BoxArea = Cast<UCheckBoxWidgetBase>(GetWidgetFromName(TEXT("CanColoring_BoxArea")));
	CanGrouping_BoxType = Cast<UCheckBoxWidgetBase>(GetWidgetFromName(TEXT("CanGrouping_BoxType")));
	AlignDropDown = Cast<UComboBoxString>(GetWidgetFromName(TEXT("AlignDropDown")));
	NestButton = Cast<UButton>(GetWidgetFromName(TEXT("NestButton")));
	BoardSizeX = Cast<UEditableText>(GetWidgetFromName(TEXT("BoardSizeX")));
	BoardSizeY = Cast<UEditableText>(GetWidgetFromName(TEXT("BoardSizeY")));
	BoardPaddingX = Cast<UEditableText>(GetWidgetFromName(TEXT("BoardPaddingX")));
	BoardPaddingY = Cast<UEditableText>(GetWidgetFromName(TEXT("BoardPaddingY")));
	BoardMargin = Cast<UEditableText>(GetWidgetFromName(TEXT("BoardMargin")));
	BoxMargin = Cast<UEditableText>(GetWidgetFromName(TEXT("BoxMargin")));
	BoxMin = Cast<UEditableText>(GetWidgetFromName(TEXT("BoxMin")));
	BoxMax = Cast<UEditableText>(GetWidgetFromName(TEXT("BoxMax")));
	AutoCheckBox = Cast<UCheckBox>(GetWidgetFromName(TEXT("AutoCheckBox")));

	// CheckBoxes OnCheckStateChanged
	if (AutoFilter->ChildCheckBox)				AutoFilter->ChildCheckBox->OnCheckStateChanged.AddDynamic(this, &USimulationWidgetBase::OnCheckedAutoFilter);
	if (CanBoxRotating->ChildCheckBox)			CanBoxRotating->ChildCheckBox->OnCheckStateChanged.AddDynamic(this, &USimulationWidgetBase::OnCheckedBoxRotating);
	if (CanColoring_BoxArea->ChildCheckBox)		CanColoring_BoxArea->ChildCheckBox->OnCheckStateChanged.AddDynamic(this, &USimulationWidgetBase::OnCheckedBoxArea);
	if (CanGrouping_BoxType->ChildCheckBox)		CanGrouping_BoxType->ChildCheckBox->OnCheckStateChanged.AddDynamic(this, &USimulationWidgetBase::OnCheckedBoxGroup);

	// DropDown
	if(AlignDropDown)							AlignDropDown->OnSelectionChanged.AddDynamic(this, &USimulationWidgetBase::OnChangedAlign);

	// EditableText OnTextCommitted
	if (BoardSizeX)								BoardSizeX->OnTextCommitted.AddDynamic(this, &USimulationWidgetBase::OnChangedBoardSizeX);
	if (BoardSizeY)								BoardSizeY->OnTextCommitted.AddDynamic(this, &USimulationWidgetBase::OnChangedBoardSizeY);
	if (BoardPaddingX)							BoardPaddingX->OnTextCommitted.AddDynamic(this, &USimulationWidgetBase::OnChangedBoardPaddingX);
	if (BoardPaddingY)							BoardPaddingY->OnTextCommitted.AddDynamic(this, &USimulationWidgetBase::OnChangedBoardPaddingY);
	if (BoardMargin)							BoardMargin->OnTextCommitted.AddDynamic(this, &USimulationWidgetBase::OnChangedBoardMargin);
	if (BoxMin)									BoxMin->OnTextCommitted.AddDynamic(this, &USimulationWidgetBase::OnChangedBoxMin);
	if (BoxMax)									BoxMax->OnTextCommitted.AddDynamic(this, &USimulationWidgetBase::OnChangedBoxMax);
	if (BoxMargin)								BoxMargin->OnTextCommitted.AddDynamic(this, &USimulationWidgetBase::OnChangedBoxMargin);

	// Button
	if (NestButton)								NestButton->OnClicked.AddDynamic(this, &USimulationWidgetBase::OnClickedNestButton);

	// Spawnd Actor in world
	BoxNesting = Cast<ABoxNesting>(UGameplayStatics::GetActorOfClass(GetWorld(), ABoxNesting::StaticClass()));

	SetUMGProperties();
}

/* Set default value of UI as option properties on editor. */
void USimulationWidgetBase::SetUMGProperties() {
	// CheckBoxes
	AutoFilter->ChildCheckBox->SetCheckedState(BoxNesting->bCanAutoFiltering ? ECheckBoxState::Checked : ECheckBoxState::Unchecked);
	CanBoxRotating->ChildCheckBox->SetCheckedState(BoxNesting->bCanBoxRotating ? ECheckBoxState::Checked : ECheckBoxState::Unchecked);
	CanColoring_BoxArea->ChildCheckBox->SetCheckedState(BoxNesting->bIsColorfullBox ? ECheckBoxState::Checked : ECheckBoxState::Unchecked);
	CanGrouping_BoxType->ChildCheckBox->SetCheckedState(BoxNesting->bCanTypeGrouping ? ECheckBoxState::Checked : ECheckBoxState::Unchecked);
	AlignDropDown->SetSelectedIndex(BoxNesting->eAlign == EBoxAlign::E_Top ? 0 : 1);
	BoardSizeX->SetText(FText::AsNumber(BoxNesting->BoardSizeX));
	BoardSizeY->SetText(FText::AsNumber(BoxNesting->BoardSizeY));
	BoardPaddingX->SetText(FText::AsNumber(BoxNesting->PaddingX));
	BoardPaddingY->SetText(FText::AsNumber(BoxNesting->PaddingY));
	BoardMargin->SetText(FText::AsNumber(BoxNesting->BoardDistance));
	BoxMin->SetText(FText::AsNumber(BoxNesting->FilterSizeMin));
	BoxMax->SetText(FText::AsNumber(BoxNesting->FilterSizeMax));
	BoxMargin->SetText(FText::AsNumber(BoxNesting->BoxDistance));
}

void USimulationWidgetBase::OnCheckedAutoFilter(bool bIsChecked)
{
	if (BoxNesting) {
		BoxNesting->bCanAutoFiltering = bIsChecked;
		if (AutoCheckBox && AutoCheckBox->IsChecked()) BoxNesting->NestBoxes();
	}
}

void USimulationWidgetBase::OnCheckedBoxRotating(bool bIsChecked)
{
	if (BoxNesting) {
		BoxNesting->bCanBoxRotating = bIsChecked;
		if (AutoCheckBox && AutoCheckBox->IsChecked()) BoxNesting->NestBoxes();
	}
}

void USimulationWidgetBase::OnCheckedBoxArea(bool bIsChecked)
{
	if (BoxNesting) {
		BoxNesting->bIsColorfullBox = bIsChecked;
		if (AutoCheckBox && AutoCheckBox->IsChecked()) BoxNesting->NestBoxes();
	}
}

void USimulationWidgetBase::OnCheckedBoxGroup(bool bIsChecked)
{
	if (BoxNesting) {
		BoxNesting->bCanTypeGrouping = bIsChecked;
		if (AutoCheckBox && AutoCheckBox->IsChecked()) BoxNesting->NestBoxes();
	}
}

void USimulationWidgetBase::OnChangedAlign(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	if (BoxNesting) {
		BoxNesting->eAlign = SelectedItem == "Vertical" ? EBoxAlign::E_Top : EBoxAlign::E_Left;
		if (AutoCheckBox && AutoCheckBox->IsChecked()) BoxNesting->NestBoxes();
	}
}

void USimulationWidgetBase::OnChangedBoardSizeX(const FText& Text, ETextCommit::Type Method)
{
	if (Method == ETextCommit::OnEnter) {
		int value = UUtility::GetIntFromFText(Text);
		BoxNesting->BoardSizeX = isnan((double)value) ? BoxNesting->BoardSizeX : value;
		if (AutoCheckBox && AutoCheckBox->IsChecked()) BoxNesting->NestBoxes();
	}
}

void USimulationWidgetBase::OnChangedBoardSizeY(const FText& Text, ETextCommit::Type Method)
{
	if (Method == ETextCommit::OnEnter) {
		int value = UUtility::GetIntFromFText(Text);
		BoxNesting->BoardSizeY = isnan((double)value) ? BoxNesting->BoardSizeY : value;
		if (AutoCheckBox && AutoCheckBox->IsChecked()) BoxNesting->NestBoxes();
	}
}

void USimulationWidgetBase::OnChangedBoardPaddingX(const FText& Text, ETextCommit::Type Method)
{
	if (Method == ETextCommit::OnEnter) {
		int value = UUtility::GetIntFromFText(Text);
		BoxNesting->PaddingX = isnan((double)value) ? BoxNesting->PaddingX : value;
		if (AutoCheckBox && AutoCheckBox->IsChecked()) BoxNesting->NestBoxes();
	}
}

void USimulationWidgetBase::OnChangedBoardPaddingY(const FText& Text, ETextCommit::Type Method)
{
	if (Method == ETextCommit::OnEnter) {
		int value = UUtility::GetIntFromFText(Text);
		BoxNesting->PaddingY = isnan((double)value) ? BoxNesting->PaddingY : value;
		if (AutoCheckBox && AutoCheckBox->IsChecked()) BoxNesting->NestBoxes();
	}
}

void USimulationWidgetBase::OnChangedBoardMargin(const FText& Text, ETextCommit::Type Method)
{
	if (Method == ETextCommit::OnEnter) {
		int value = UUtility::GetIntFromFText(Text);
		BoxNesting->BoardDistance = isnan((double)value) ? BoxNesting->BoardDistance : value;
		if (AutoCheckBox && AutoCheckBox->IsChecked()) BoxNesting->NestBoxes();
	}
}

void USimulationWidgetBase::OnChangedBoxMin(const FText& Text, ETextCommit::Type Method)
{
	if (Method == ETextCommit::OnEnter) {
		int value = UUtility::GetIntFromFText(Text);
		BoxNesting->FilterSizeMin = isnan((double)value) ? BoxNesting->BoardDistance : value;
		if (AutoCheckBox && AutoCheckBox->IsChecked()) BoxNesting->NestBoxes();
	}
}

void USimulationWidgetBase::OnChangedBoxMax(const FText& Text, ETextCommit::Type Method)
{
	if (Method == ETextCommit::OnEnter) {
		int value = UUtility::GetIntFromFText(Text);
		BoxNesting->FilterSizeMax = isnan((double)value) ? BoxNesting->BoardDistance : value;
		if (AutoCheckBox && AutoCheckBox->IsChecked()) BoxNesting->NestBoxes();
	}
}

void USimulationWidgetBase::OnChangedBoxMargin(const FText& Text, ETextCommit::Type Method)
{
	if (Method == ETextCommit::OnEnter) {
		int value = UUtility::GetIntFromFText(Text);
		BoxNesting->BoxDistance = isnan((double)value) ? BoxNesting->BoxDistance : value;
		if (AutoCheckBox && AutoCheckBox->IsChecked()) BoxNesting->NestBoxes();
	}
}

void USimulationWidgetBase::OnClickedNestButton()
{
	if (BoxNesting)	BoxNesting->NestBoxes();
}