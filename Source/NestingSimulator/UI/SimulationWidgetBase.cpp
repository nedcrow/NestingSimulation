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
	CanColoring_BoxGroup = Cast<UCheckBoxWidgetBase>(GetWidgetFromName(TEXT("CanColoring_BoxGroup")));
	NestButton = Cast<UButton>(GetWidgetFromName(TEXT("NestButton")));
	BoardSizeX = Cast<UEditableText>(GetWidgetFromName(TEXT("BoardSizeX")));
	BoardSizeY = Cast<UEditableText>(GetWidgetFromName(TEXT("BoardSizeY")));
	BoardPaddingX = Cast<UEditableText>(GetWidgetFromName(TEXT("BoardPaddingX")));
	BoardPaddingY = Cast<UEditableText>(GetWidgetFromName(TEXT("BoardPaddingY")));
	BoardMargin = Cast<UEditableText>(GetWidgetFromName(TEXT("BoardMargin")));
	BoardMargin = Cast<UEditableText>(GetWidgetFromName(TEXT("BoardMargin")));
	AutoCheckBox = Cast<UCheckBox>(GetWidgetFromName(TEXT("AutoCheckBox")));

	// CheckBoxes OnCheckStateChanged
	if (AutoFilter->ChildCheckBox)				AutoFilter->ChildCheckBox->OnCheckStateChanged.AddDynamic(this, &USimulationWidgetBase::OnCheckedAutoFilter);
	if (CanBoxRotating->ChildCheckBox)			CanBoxRotating->ChildCheckBox->OnCheckStateChanged.AddDynamic(this, &USimulationWidgetBase::OnCheckedBoxRotating);
	if (CanColoring_BoxArea->ChildCheckBox)		CanColoring_BoxArea->ChildCheckBox->OnCheckStateChanged.AddDynamic(this, &USimulationWidgetBase::OnCheckedBoxArea);
	if (CanColoring_BoxGroup->ChildCheckBox)	CanColoring_BoxGroup->ChildCheckBox->OnCheckStateChanged.AddDynamic(this, &USimulationWidgetBase::OnCheckedBoxGroup);

	// EditableText OnTextCommitted
	if (BoardSizeX)								BoardSizeX->OnTextCommitted.AddDynamic(this, &USimulationWidgetBase::OnChangedBoardSizeX);
	if (BoardSizeY)								BoardSizeY->OnTextCommitted.AddDynamic(this, &USimulationWidgetBase::OnChangedBoardSizeY);
	if (BoardPaddingX)							BoardPaddingX->OnTextCommitted.AddDynamic(this, &USimulationWidgetBase::OnChangedBoardPaddingX);
	if (BoardPaddingY)							BoardPaddingY->OnTextCommitted.AddDynamic(this, &USimulationWidgetBase::OnChangedBoardPaddingY);
	if (BoardMargin)							BoardMargin->OnTextCommitted.AddDynamic(this, &USimulationWidgetBase::OnChangedBoardMargin);
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
	CanColoring_BoxGroup->ChildCheckBox->SetCheckedState(BoxNesting->bCanTypeGrouping ? ECheckBoxState::Checked : ECheckBoxState::Unchecked);
	BoardSizeX->SetText(FText::AsNumber(BoxNesting->BoardSizeX));
	BoardSizeY->SetText(FText::AsNumber(BoxNesting->BoardSizeY));
	BoardPaddingX->SetText(FText::AsNumber(BoxNesting->PaddingX));
	BoardPaddingY->SetText(FText::AsNumber(BoxNesting->PaddingY));
	BoardMargin->SetText(FText::AsNumber(BoxNesting->BoardDistance));
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

void USimulationWidgetBase::OnChangedBoardSizeX(const FText& Text, ETextCommit::Type Method)
{
	if (Method == ETextCommit::OnEnter) {
		int value = UUtility::GetIntFromFText(Text);
		BoxNesting->BoardSizeX = isnan((double)value) ? BoxNesting->BoardSizeX : value;
		BoardSizeX->SetText(FText::AsNumber(BoxNesting->BoardSizeX));
		if (AutoCheckBox && AutoCheckBox->IsChecked()) BoxNesting->NestBoxes();
	}
}

void USimulationWidgetBase::OnChangedBoardSizeY(const FText& Text, ETextCommit::Type Method)
{
	if (Method == ETextCommit::OnEnter) {
		int value = UUtility::GetIntFromFText(Text);
		BoxNesting->BoardSizeY = isnan((double)value) ? BoxNesting->BoardSizeY : value;
		BoardSizeY->SetText(FText::AsNumber(BoxNesting->BoardSizeY));
		if (AutoCheckBox && AutoCheckBox->IsChecked()) BoxNesting->NestBoxes();
	}
}

void USimulationWidgetBase::OnChangedBoardPaddingX(const FText& Text, ETextCommit::Type Method)
{
	if (Method == ETextCommit::OnEnter) {
		int value = UUtility::GetIntFromFText(Text);
		BoxNesting->PaddingX = isnan((double)value) ? BoxNesting->PaddingX : value;
		BoardPaddingX->SetText(FText::AsNumber(BoxNesting->PaddingX));
		if (AutoCheckBox && AutoCheckBox->IsChecked()) BoxNesting->NestBoxes();
	}
}

void USimulationWidgetBase::OnChangedBoardPaddingY(const FText& Text, ETextCommit::Type Method)
{
	if (Method == ETextCommit::OnEnter) {
		int value = UUtility::GetIntFromFText(Text);
		BoxNesting->PaddingY = isnan((double)value) ? BoxNesting->PaddingY : value;
		BoardPaddingY->SetText(FText::AsNumber(BoxNesting->PaddingY));
		if (AutoCheckBox && AutoCheckBox->IsChecked()) BoxNesting->NestBoxes();
	}
}

void USimulationWidgetBase::OnChangedBoardMargin(const FText& Text, ETextCommit::Type Method)
{
	if (Method == ETextCommit::OnEnter) {
		int value = UUtility::GetIntFromFText(Text);
		BoxNesting->BoardDistance = isnan((double)value) ? BoxNesting->BoardDistance : value;
		BoardMargin->SetText(FText::AsNumber(BoxNesting->BoardDistance));
		if (AutoCheckBox && AutoCheckBox->IsChecked()) BoxNesting->NestBoxes();
	}
}

void USimulationWidgetBase::OnChangedBoxMargin(const FText& Text, ETextCommit::Type Method)
{
	if (Method == ETextCommit::OnEnter) {
		int value = UUtility::GetIntFromFText(Text);
		BoxNesting->BoxDistance = isnan((double)value) ? BoxNesting->BoxDistance : value;
		BoxMargin->SetText(FText::AsNumber(BoxNesting->BoxDistance));
		if (AutoCheckBox && AutoCheckBox->IsChecked()) BoxNesting->NestBoxes();
	}
}

void USimulationWidgetBase::OnClickedNestButton()
{
	if (BoxNesting)	BoxNesting->NestBoxes();
}