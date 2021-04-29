// Fill out your copyright notice in the Description page of Project Settings.


#include "CubeNesting.h"
#include "CubeBoard.h"
#include "CubeDataTable.h"
#include "../TileBase.h"
#include "../NestingGS.h"
#include "Components/SphereComponent.h"
#include "Engine/DataTable.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ACubeNesting::ACubeNesting()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	RootComponent = Sphere;
}

void ACubeNesting::PostRegisterAllComponents()
{
	Super::PostRegisterAllComponents();
#if WITH_EDITOR	
	NestCubes();
#endif
}

// Called when the game starts or when spawned
void ACubeNesting::BeginPlay()
{
	Super::BeginPlay();
	NestCubes();
}

void ACubeNesting::NestCubes()
{
	CleanUpGarbageCubes();
	ResetUnit();
}

void ACubeNesting::CleanUpGarbageCubes()
{
	// Actor
	// Struct & ETC
}

void ACubeNesting::ResetUnit()
{
	ANestingGS* GS = Cast<ANestingGS>(UGameplayStatics::GetGameState(GetWorld()));
	if (GS) Unit = GS->Unit;
}

void ACubeNesting::AddBoard()
{
	// 보드액터생성 UGameplayStatics::BeginDeferredActorSpawnFromClass
	//ACubeBoard* board;
	//if (board) {
	//	CubeBoardArray.Add(board);

	//	UGameplayStatics::FinishSpawningActor(); // 실질적인 생성
	//}

	ANestingGS* GS = Cast<ANestingGS>(UGameplayStatics::GetGameState(GetWorld()));
	if (GS) {
		GS->SetCurrentBoardSize(BoardSizeX, BoardSizeY, BoardDistance);
		GS->CurrentBoardCount = CubeBoardArray.Num();
	}


}

void ACubeNesting::ArrangeCubes()
{
	/* Sort : 방향 통일, UnWeightBais 옵션 하나라도 true면 무게 정렬, 아니면 크기 순 */

	// Filer

	// 그룹 - 타입 옵션 false면 한 그룹

	// 컬러값 - 컬러 옵션에 맞춰서..
}

void ACubeNesting::NestOneArray(TArray<FCubeListTableRow>, int StartBoardIndex)
{
}

