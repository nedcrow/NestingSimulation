// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BoxNesting.generated.h"

UENUM(BlueprintType)
enum class EBoxAlign : uint8 {
	E_Top = 0	UMETA(DisplayName = "AlignTop"),
	E_Left = 1	UMETA(DisplayName = "AlignLeft"),
};

USTRUCT(Atomic, BlueprintType)
struct FRowStruct {
	GENERATED_BODY()

	public:
	TArray<int8> RowArr;
};

USTRUCT(Atomic, BlueprintType)
struct FBoardStruct {
	GENERATED_BODY()

public:
	TArray<FRowStruct> Matrix;
	//int8 ** Matrix;
	int EmptyArea;
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
	int BeforeStartPoint[2] = {0,0};
	FVector2D BeforeBoxSize;
	FBoardStruct BeforeBoard;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

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
	TArray<FBoardStruct> BoardStructArr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data_Board")
	int BoardSizeX = 10;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data_Board")
	int BoardSizeY = 10;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data_Board")
	int PaddingX = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data_Board")
	int PaddingY = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data_Board")
	int BoardDistance = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data_Board")
	int BoxDistance = 1;
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
	int FilterSizeMin = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data_Box")
	int FilterSizeMax = 10;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Data_Box")
	TArray<FVector> AreaKindColors;

#pragma endregion

private:
	void CreateBoard(int _BoardSizeX, int _BoardSizeY, bool IsReset);
	void AddBoard();
	void NestBox();
	int GetFalseAreaInBoard(int BoxSizeX, int BoxSizeY, FBoardStruct _Board, int StartPoint[2]);
};
