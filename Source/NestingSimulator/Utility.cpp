// Fill out your copyright notice in the Description page of Project Settings.


#include "Utility.h"

int UUtility::GetIntFromFText(const FText& Text)
{
	if (Text.IsNumeric()) {
		return FCString::Atoi(*Text.ToString());
	}
	UE_LOG(LogClass, Warning, TEXT("Text is not numeric"));
    return NAN;
}
