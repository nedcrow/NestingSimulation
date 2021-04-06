// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BoxNesting.generated.h"

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

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Centimeter
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	float Unit = 100;

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
	TArray<int> FilteredBoxIndexes;

	/*�� ���ι��Ͱ� Ȱ��ȭ�Ǹ� �ɷ��� ���ڵ��� ���� ��ġ�Ѵ�.*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data_Box")
	bool bCanAutoFiltering = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data_Box")
	bool bCanTypeGrouping = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data_Box")
	int FilterSizeMin = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data_Box")
	int FilterSizeMax = 10;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Data_Box")
	TArray<FVector> AreaKindColors;

#pragma endregion

	void CreateBoard(int _BoardSizeX, int _BoardSizeY, bool IsReset);
	void AddBoard();
	void NestBox();
	int GetFalseAreaInBoard(int BoxSizeX, int BoxSizeY, FBoardStruct _Board, int StartPoint[2]);
};
