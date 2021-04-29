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
	// ������ͻ��� UGameplayStatics::BeginDeferredActorSpawnFromClass
	//ACubeBoard* board;
	//if (board) {
	//	CubeBoardArray.Add(board);

	//	UGameplayStatics::FinishSpawningActor(); // �������� ����
	//}

	ANestingGS* GS = Cast<ANestingGS>(UGameplayStatics::GetGameState(GetWorld()));
	if (GS) {
		GS->SetCurrentBoardSize(BoardSizeX, BoardSizeY, BoardDistance);
		GS->CurrentBoardCount = CubeBoardArray.Num();
	}


}

void ACubeNesting::ArrangeCubes()
{
	/* Sort : ���� ����, UnWeightBais �ɼ� �ϳ��� true�� ���� ����, �ƴϸ� ũ�� �� */

	// Filer

	// �׷� - Ÿ�� �ɼ� false�� �� �׷�

	// �÷��� - �÷� �ɼǿ� ���缭..
}

void ACubeNesting::NestOneArray(TArray<FCubeListTableRow>, int StartBoardIndex)
{
}

