// Fill out your copyright notice in the Description page of Project Settings.


#include "BoxNesting.h"
#include "Components/SphereComponent.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Engine/DataTable.h"
//#include "UObject/ConstructorHelpers.h"
#include "BoxDataTable.h"

// Sets default values
ABoxNesting::ABoxNesting()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	RootComponent = Sphere;

	BoardISM = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("BoardISM"));
	BoardISM->SetupAttachment(Sphere);

	BoxISM = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("BoxISM"));
	BoxISM->SetupAttachment(Sphere);
	BoardISM->SetRelativeLocation(FVector().ZeroVector);
}

void ABoxNesting::PostRegisterAllComponents()
{
	Super::PostRegisterAllComponents();
	CreateBoard(BoardSizeX, BoardSizeY, true);
	NestBox();
}

void ABoxNesting::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ABoxNesting::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABoxNesting::CreateBoard(int _BoardSizeX, int _BoardSizeY, bool IsReset) {
	// Reset - 리셋하는 이유는 에디터에서 플레이 하지 않고도 확인하기 위함.
	if (IsReset) {
		BoardISM->ClearInstances();
		BoardStructArr.Empty();
		FilteredBoxIndexes.Empty();
	}

	// Board Actor - 보드길이 + 누적보드길이 + 누적보드간격
	FVector startLocation = FVector(
		(_BoardSizeX * Unit * 0.5f) + (_BoardSizeX * BoardStructArr.Num() * Unit) + (BoardDistance * BoardStructArr.Num() * Unit),
		(_BoardSizeY * Unit * 0.5f),
		.0f
	);

	BoardISM->AddInstance(FTransform(
		FRotator().ZeroRotator,
		startLocation,
		FVector(_BoardSizeX, _BoardSizeY, 1.0f)
	));

	// Board Struct - All space is 1(false) in matrix
	FRowStruct Row = FRowStruct();
	FBoardStruct Board = FBoardStruct();

	Row.RowArr.Init(1, _BoardSizeX);
	Board.Matrix.Init(Row, _BoardSizeY);

	// Board Padding
	for (int y = 0; y < Board.Matrix.Num(); y++) {
		for (int i = 0; i < PaddingX; i++) {
			Board.Matrix[y].RowArr[i] = 0;
			Board.Matrix[y].RowArr[_BoardSizeX - 1 - i] = 0;
		}
	}// PaddingX - 모든 줄의 시작과 끝 칸이 대상
	for (int i = 0; i < PaddingY; i++) {
		Board.Matrix[i].RowArr.Init(0, _BoardSizeX);
		Board.Matrix[_BoardSizeY - 1 - i].RowArr.Init(0, _BoardSizeX);
	}// PaddingY - 시작과 끝 줄의 모든 칸이 대상
	Board.EmptyArea = (_BoardSizeX - PaddingX * 2) * (_BoardSizeY - PaddingY * 2);

	BoardStructArr.Add(Board);
}

void ABoxNesting::AddBoard() {

	CreateBoard(BoardSizeX, BoardSizeY, false);
	UE_LOG(LogTemp, Warning, TEXT("Add Board"));
}

void ABoxNesting::NestBox()
{
	if (BoxDataTable != nullptr)
	{
		// Sort box area
		TArray<FBoxListTableRow*> BoxArray;
		FString Context;
		BoxDataTable->GetAllRows(Context, BoxArray);
		BoxArray.Sort([](FBoxListTableRow a, FBoxListTableRow b) {return a.SizeX * a.SizeY > b.SizeX * b.SizeY; });

		// filter & AreaKindColors
		int lastArea = 0;
		FVector lastColor;
		for (int i = 0; i < BoxArray.Num(); i++) {
			// filter
			bool isFiltered = BoxArray[i]->SizeX > BoardSizeX - PaddingX * 2 || BoxArray[i]->SizeY > BoardSizeX - PaddingX * 2 ||
				BoxArray[i]->SizeX > BoardSizeY - PaddingY * 2 || BoxArray[i]->SizeY > BoardSizeY - PaddingY * 2 ||
				BoxArray[i]->SizeX < FilterSizeMin || BoxArray[i]->SizeY < FilterSizeMin ||
				BoxArray[i]->SizeX > FilterSizeMax || BoxArray[i]->SizeY > FilterSizeMax;

			if (isFiltered) {
				if (bCanAutoFiltering == false) {
					UE_LOG(LogTemp, Warning, TEXT("Error: box size over then filter - Box{Type: %s, SizeX: %d, SizeY: %d}"), *BoxArray[i]->Type, BoxArray[i]->SizeX, BoxArray[i]->SizeY);
					return;
				}

				FilteredBoxIndexes.Add(i);
			}

			// AreaKindColors
			if (BoxArray[i]->SizeX * BoxArray[i]->SizeY != lastArea) {
				lastArea = BoxArray[i]->SizeX * BoxArray[i]->SizeY;
				lastColor = FVector(FMath::RandRange(0.1f, 1.0f), FMath::RandRange(0.1f, 1.0f), FMath::RandRange(0.1f, 1.0f));
			}
			AreaKindColors.Add(lastColor);
		}

		// Instance
		BoxISM->ClearInstances();

		// 순차적으로 Box 자리 찾기 
		for (int i = 0; i < BoxArray.Num(); i++) {
			//bool isSucceed = false;
			int falseSpaceCount = 0;
			int boxSizeX = BoxArray[i]->SizeX;
			int boxSizeY = BoxArray[i]->SizeY;
			int startPoint[2] = { 0, 0 };

			// 필터에 걸리는 박스면 종료
			if (FilteredBoxIndexes.Contains(i)) {
				goto Finished_Box;
			}

			// 첫 보드부터 탐색 
			for (int j = 0; j < BoardStructArr.Num(); j++) {
				FBoardStruct board = BoardStructArr[j];

				// 마지막 보드의 공백 영역이 박스 사이즈보다 작으면 자리확인 종료
				int BoxArea = BoxArray[i]->SizeX * BoxArray[i]->SizeY;
				bool isLastBoard = j == BoardStructArr.Num() - 1;
				if (isLastBoard && board.EmptyArea < BoxArea) {
					goto Finished_Counting;
				}

				// 탐색 전 보드 내부의 시작점 갱신
				for (int boardY = 0; boardY < BoardSizeY; boardY++) {
					for (int boardX = 0; boardX < BoardSizeX; boardX++) {
						startPoint[0] = boardX;
						startPoint[1] = boardY;

						// 시작점부터 박스면적만큼 탐색
						falseSpaceCount = GetFalseAreaInBoard(boxSizeX, boxSizeY, board, startPoint);

						// 공간이 충분치 않으면 회전 후 다시 탐색
						if (falseSpaceCount != BoxArea) {
							boxSizeX = BoxArray[i]->SizeY;
							boxSizeY = BoxArray[i]->SizeX;
							falseSpaceCount = GetFalseAreaInBoard(boxSizeX, boxSizeY, board, startPoint);
						}

						// 공간 충분하면 배치 및 자리확인 종료
						if (falseSpaceCount == BoxArea) {
							if (i < 4) {
								UE_LOG(LogTemp, Warning, TEXT("Succeed box : %d (x: %d, y:%d)"), i, startPoint[0], startPoint[1]);
							}

							// Board Matrix - Box & BoxDistance
							int loopX = startPoint[0] + boxSizeX + BoxDistance;
							int loopY = startPoint[1] + boxSizeY + BoxDistance;
							loopX = loopX <= BoardSizeX ? loopX - startPoint[0] : BoardSizeX - startPoint[0];
							loopY = loopY <= BoardSizeY ? loopY - startPoint[1] : BoardSizeY - startPoint[1];
							for (int y = 0; y < loopY; y++) {
								for (int x = 0; x < loopX; x++) {
									BoardStructArr[j].Matrix[startPoint[1] + y].RowArr[startPoint[0] + x] = 0;
								}
							}

							// Box Actor
							int alpha = (BoardSizeX + BoardDistance) * Unit * j;
							FVector boxLocation = FVector(
								(startPoint[0] * Unit) + (boxSizeX * Unit * 0.5f) + alpha,
								(startPoint[1] * Unit) + (boxSizeY * Unit * 0.5f),
								0.0f
							);
							FVector scale = FVector(boxSizeX, boxSizeY, 1.0f);
							//BoxISM->CreateDynamicMaterialInstance(0);							
							//BoxISM->SetVectorParameterValueOnMaterials(TEXT("Color"), AreaKindColors[i]);
							BoxISM->AddInstance(FTransform(FRotator().ZeroRotator, boxLocation, scale));
							// etc
							//isSucceed = true;
							BoardStructArr[j].EmptyArea -= loopX * loopY;
							goto Finished_Box;
						}
					}
				}
			}

			Finished_Counting:
			// 보드 다 돌았는데 통과 못한 경우. 현재 박스 고정, 다음 보드 생성
			i--;
			AddBoard();

			Finished_Box: {}
			/*if (isSucceed) {
				UE_LOG(LogTemp, Warning, TEXT("Succeed box : %d"), i);
			}*/

		}

		// 필터에 걸린 박스 따로 그리기
		float sumX = 0.0f;
		for (int i = 0; i < FilteredBoxIndexes.Num(); i++) {
			float boxX = BoxArray[FilteredBoxIndexes[i]]->SizeX;
			float boxY = BoxArray[FilteredBoxIndexes[i]]->SizeY;
			FVector filteredBoxLocation = FVector(
				sumX + (boxX * Unit * 0.5f),
				-((BoxArray[FilteredBoxIndexes[0]]->SizeY * Unit * 0.5f) + (BoxDistance * Unit)),
				0.0f
			);
			FVector filteredBoxScale = FVector(boxX, boxY, 1.0f);
			sumX += (boxX + BoxDistance) * Unit;

			BoxISM->AddInstance(FTransform(FRotator().ZeroRotator, filteredBoxLocation, filteredBoxScale));
		}


	}
}

int ABoxNesting::GetFalseAreaInBoard(int BoxSizeX, int BoxSizeY, FBoardStruct _Board, int StartPoint[2])
{
	int falseCount = 0;
	if (StartPoint[0] + BoxSizeX > BoardSizeX || StartPoint[1] + BoxSizeY > BoardSizeY) { 
		return falseCount;
	}
	else {
		for (int y = 0; y < BoxSizeY; y++) {
			for (int x = 0; x < BoxSizeX; x++) {
				if (_Board.Matrix[StartPoint[1] + y].RowArr[StartPoint[0] + x] == 0) { return falseCount; }
				falseCount += 1;
			}
		}
	}

	return falseCount;
}

