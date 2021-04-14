// Fill out your copyright notice in the Description page of Project Settings.

#include "CheckBoxWidgetBase.h"
#include "Components/TextBlock.h"
#include "Components/CheckBox.h"

void UCheckBoxWidgetBase::NativeConstruct() {
	Super::NativeConstruct();
	ChildBoxText = Cast<UTextBlock>(GetWidgetFromName(TEXT("ChildBoxText")));
	ChildCheckBox = Cast<UCheckBox>(GetWidgetFromName(TEXT("ChildCheckBox")));
}