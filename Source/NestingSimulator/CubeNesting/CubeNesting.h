// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CubeDataTable.h"
#include "CubeBoard.h"
#include "CubeNesting.generated.h"

UENUM(BlueprintType)
enum class ECubeAlign : uint8 {
	E_Vertical = 0	UMETA(DisplayName = "Vertical"),
	E_Horizontal = 1	UMETA(DisplayName = "Horizontal"),
	E_UnWeightBais = 2	UMETA(DisplayName = "UnWeightBais"),
};

UENUM(BlueprintType)
enum class ECubeColoring : uint8 {
	E_Volume = 0	UMETA(DisplayName = "Volume"),
	E_Mass = 1	UMETA(DisplayName = "Mass"),
	E_Type = 2	UMETA(DisplayName = "Type"),
};

USTRUCT(Atomic, BlueprintType)
struct FCubeGroup {
	GENERATED_BODY()

public:
	int TypeIndex;
	TArray<FCubeListTableRow*> Boxes;
	TArray<int32> FilteredIndexes;
};

UCLASS()
class NESTINGSIMULATOR_API ACubeNesting : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACubeNesting();
	class USphereComponent* Sphere;

protected:
	virtual void PostRegisterAllComponents() override;
	virtual void BeginPlay() override;
	float Unit = 100;
	int CurrentTypeGroupIndex;
	int BeforeStartPoint[3] = { 0,0,0 };
	int BeforeCubeBoardIndex;
	FVector BeforeCubeSize;
	TArray<ACubeBoard*> CubeBoardArray;
	TArray<FVector> CubeColorArray;
	TArray<FCubeGroup*> CubeGroups;

public:	

	void NestCubes();
	void CleanUpGarbageCubes();
	void ResetUnit();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data_Common")
		ECubeColoring eColoring = ECubeColoring::E_Type;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data_Common")
		ECubeAlign eAlign = ECubeAlign::E_Vertical;

	/* True 설정 시 정렬은 평행조건을 따름. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data_Common")
		bool bCanWeightBais = false;


	/*이 프로미터가 활성화되면 걸러진 상자들을 따로 배치한다.*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data_Common")
		bool bCanAutoFiltering = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data_Common")
		bool bCanCubeRotatingRoll = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data_Common")
		bool bCanCubeRotatingPitch = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data_Common")
		bool bCanCubeRotatingYaw = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data_Common")
		bool bCanTypeGrouping = false;

#pragma region Board
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data_Box")
		TSubclassOf<class ACubeBoard> BoardClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data_Board")
		int32 BoardSizeX = 10;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data_Board")
		int32 BoardSizeY = 10;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data_Board")
		int32 BoardSizeZ = 10;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data_Board")
		int32 PaddingX = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data_Board")
		int32 PaddingY = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data_Board")
		int32 PaddingZ = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data_Board")
		int32 BoardDistance = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data_Board")
		int32 BoxDistance = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data_Board")
		int32 MaxWeight = 1;
#pragma endregion


#pragma region Cube
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data_Box")
		class UDataTable* CubeDataTable;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data_Box")
		TSubclassOf<class ATileBase> CubeClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data_Box")
		TArray<ATileBase*> CubeActorArr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data_Box")
		TArray<AActor*> GarbageBoxActorArr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data_Box")
		TArray<int> FilteredCubeIndexes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data_Box")
		int32 FilterSizeMin = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data_Box")
		int32 FilterSizeMax = 10;
#pragma endregion


private:
	void AddBoard();
	void ArrangeCubes();
	void NestOneArray(TArray<FCubeListTableRow>, int StartBoardIndex);
	void DrawFilteredCube(); 
};
