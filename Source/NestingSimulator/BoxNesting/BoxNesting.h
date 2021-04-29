// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BoxDataTable.h"
#include "BoxNesting.generated.h"

UENUM(BlueprintType)
enum class EBoxAlign : uint8 {
	E_Top = 0	UMETA(DisplayName = "AlignTop"),
	E_Left = 1	UMETA(DisplayName = "AlignLeft"),
};

USTRUCT(Atomic, BlueprintType)
struct FBoxRowStruct {
	GENERATED_BODY()

	public:
	TArray<int8> RowArr;
};

USTRUCT(Atomic, BlueprintType)
struct FBoxBoardStruct {
	GENERATED_BODY()

public:
	TArray<FBoxRowStruct> Matrix;
	int EmptyArea;
};

USTRUCT(Atomic, BlueprintType)
struct FBoxGroup {
	GENERATED_BODY()

public:
	int TypeIndex;
	TArray<FBoxListTableRow*> Boxes;
	TArray<int32> FilteredIndexes;
};


UCLASS()
class NESTINGSIMULATOR_API ABoxNesting : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABoxNesting();
	class USphereComponent* Sphere;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class UInstancedStaticMeshComponent* BoardISM;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class UInstancedStaticMeshComponent* BoxISM;

protected:
	virtual void PostRegisterAllComponents() override;
	virtual void BeginPlay() override;
	float Unit = 100;
	int CurrentTypeGroupIndex;
	int BeforeStartPoint[2] = {0,0};
	FVector2D BeforeBoxSize;
	FBoxBoardStruct BeforeBoard;
	TArray<FBoxListTableRow*> BoxArray;
	TArray<FBoxGroup*> BoxGroups;

public:	

	void NestBoxes();
	void CleanUpGarbageBoxes();
	void ResetUnit();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data_Common")
	EBoxAlign eAlign = EBoxAlign::E_Top;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data_Common")
	bool bIsColorfullBox = false;

	/*이 프로미터가 활성화되면 걸러진 상자들을 따로 배치한다.*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data_Common")
	bool bCanAutoFiltering = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data_Common")
	bool bCanBoxRotating = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data_Common")
	bool bCanTypeGrouping = false;

#pragma region Board
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data_Board")
	TArray<FBoxBoardStruct> BoardStructArr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data_Board")
	int32 BoardSizeX = 10;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data_Board")
	int32 BoardSizeY = 10;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data_Board")
	int32 PaddingX = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data_Board")
	int32 PaddingY = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data_Board")
	int32 BoardDistance = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data_Board")
	int32 BoxDistance = 1;
#pragma endregion

#pragma region Box
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data_Box")
	class UDataTable* BoxDataTable;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data_Box")
	TSubclassOf<class ATileBase> BoxClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data_Box")
	TArray<ATileBase*> BoxActorArr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data_Box")
	TArray<AActor*> GarbageBoxActorArr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data_Box")
	TArray<int> FilteredBoxIndexes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data_Box")
	int32 FilterSizeMin = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data_Box")
	int32 FilterSizeMax = 10;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Data_Box")
	TArray<FVector> BoxColorArray;

#pragma endregion

private:
	void AddBoard();
	void CreateBoard(int _BoardSizeX, int _BoardSizeY);	
	void ArrangeBoxes();
	void NestOneArray(TArray<FBoxListTableRow*> Boxes, int StartBoardIndex);
	void DrawFilteredBox();
	int GetFalseAreaInBoard(int BoxSizeX, int BoxSizeY, FBoxBoardStruct _Board, int StartPoint[2]);
};
