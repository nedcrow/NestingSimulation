// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryWidget.generated.h"

UENUM(BlueprintType)
enum class ESaveingDirection : uint8
{
	E_Vertical = 0		UMETA(DisplayName = "Vertical"),
	E_Horizontal = 1 	UMETA(DisplayName = "Horizontal"),
};

UENUM(BlueprintType)
enum class EStartPoint : uint8
{
	E_LeftTop = 0		UMETA(DisplayName = "LeftTop"),
	E_LeftDown			UMETA(DisplayName = "LeftDown"),
	E_RightTop			UMETA(DisplayName = "RightTop"),
	E_RightDown		 	UMETA(DisplayName = "RightDown"),
};

/**
 * 
 */
UCLASS()
class NESTINGSIMULATOR_API UInventoryWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;	

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class UButton* SortButton;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Option")
	ESaveingDirection SaveingDirection;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Option")
	EStartPoint StartPoint;
};
