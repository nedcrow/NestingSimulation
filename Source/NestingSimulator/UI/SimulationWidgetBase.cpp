// Fill out your copyright notice in the Description page of Project Settings.


#include "SimulationWidgetBase.h"
#include "../BoxNesting/BoxNesting.h"
#include "CheckBoxWidgetBase.h"
#include "Components/ComboBoxString.h"
#include "Components/TextBlock.h"
#include "Components/CheckBox.h"
#include "Kismet/GameplayStatics.h"

void USimulationWidgetBase::NativeConstruct() {
	Super::NativeConstruct();
	NestingTypeDropDown = Cast<UComboBoxString>(GetWidgetFromName(TEXT("NestingTypeDropDown")));
	AutoFilter = Cast<UCheckBoxWidgetBase>(GetWidgetFromName(TEXT("AutoFilter")));
	FTimerHandle tempHandle;
	if (AutoFilter->ChildBoxText)	AutoFilter->ChildBoxText->SetText(FText::FromName(TEXT("Auto Filter")));
	if (AutoFilter->ChildCheckBox)	AutoFilter->ChildCheckBox->OnCheckStateChanged.AddDynamic(this, &USimulationWidgetBase::OnCheckedAutoFilter);
}

void USimulationWidgetBase::OnCheckedAutoFilter(bool bIsChecked)
{
	ABoxNesting* BoxNesting = Cast<ABoxNesting>(UGameplayStatics::GetActorOfClass(GetWorld(), ABoxNesting::StaticClass()));
	if (BoxNesting) {
		BoxNesting->bCanAutoFiltering = bIsChecked;
	}
}
