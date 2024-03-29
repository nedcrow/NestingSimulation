// Fill out your copyright notice in the Description page of Project Settings.


#include "BoxNesting.h"
#include "BoxDataTable.h"
#include "../TileBase.h"
#include "../NestingGS.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Engine/DataTable.h"
#include "Kismet/GameplayStatics.h"


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
#if WITH_EDITOR	
	NestBoxes();
#endif
}

void ABoxNesting::BeginPlay()
{
	Super::BeginPlay();	
	NestBoxes();
}


void ABoxNesting::NestBoxes()
{
	CleanUpGarbageBoxes();
	ResetUnit();

	if (BoxDataTable != nullptr)
	{
		ArrangeBoxes();
		if (bCanTypeGrouping) {
			for (auto boxGroup : BoxGroups) {
				CurrentTypeGroupIndex = boxGroup->TypeIndex;
				NestOneArray(boxGroup->Boxes, BoardStructArr.Num());				
			}
		}
		else {
			NestOneArray(BoxArray, 0);
		}
	}
	DrawFilteredBox();
}

void ABoxNesting::CleanUpGarbageBoxes() {
	// Actors
	UGameplayStatics::GetAllActorsOfClassWithTag(GetWorld(), ATileBase::StaticClass(), "Tile", GarbageBoxActorArr);
	for (int i = GarbageBoxActorArr.Num() - 1; i >= 0; i--) {
		if (!GarbageBoxActorArr[i]->IsPendingKill()) GarbageBoxActorArr[i]->Destroy();
	}
	GarbageBoxActorArr.Empty();
	BoxActorArr.Empty();

	// ISM
	BoxISM->ClearInstances();
	BoardISM->ClearInstances();

	// Struct & ETC	
	BoardStructArr.Empty();
	FilteredBoxIndexes.Empty();
	BoxColorArray.Empty();
}

/* must, this function have call first.*/
void ABoxNesting::ResetUnit()
{
	ANestingGS* GS = Cast<ANestingGS>(UGameplayStatics::GetGameState(GetWorld()));
	if(GS) Unit = GS->Unit;
}

void ABoxNesting::AddBoard() {
	CreateBoard(BoardSizeX, BoardSizeY);
	ANestingGS* GS = Cast<ANestingGS>(UGameplayStatics::GetGameState(GetWorld()));
	if (GS) {
		GS->SetCurrentBoardSize(BoardSizeX, BoardSizeY, BoardDistance);
		GS->CurrentBoardCount=BoardStructArr.Num();
	}
}

void ABoxNesting::CreateBoard(int _BoardSizeX, int _BoardSizeY) {
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
	FBoxRowStruct Row = FBoxRowStruct();
	FBoxBoardStruct Board = FBoxBoardStruct();

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

void ABoxNesting::ArrangeBoxes()
{
	if (BoxDataTable != nullptr)
	{
		// Sort box area
		BoxArray.Empty();
		FString Context;
		BoxDataTable->GetAllRows(Context, BoxArray);
		BoxArray.Sort([](FBoxListTableRow a, FBoxListTableRow b) {return a.SizeX * a.SizeY > b.SizeX * b.SizeY; });
		
		// filter
		int lastArea = -1;
		FVector lastColor;
		for (int i = 0; i < BoxArray.Num(); i++) {
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
		}

		// Group boxes & AreaKindColors
		BoxGroups.Empty();
		if (bCanTypeGrouping) {
			// Type 수량 만큼 Group 생성
			TArray<FString> TypeArr;
			for (auto box : BoxArray) {
				if (!TypeArr.Contains(box->Type)) {
					TypeArr.Add(box->Type);
					BoxGroups.Add(new FBoxGroup());
				}
			}

			// 각 Group(Type) 마다
			for (int i = 0; i < TypeArr.Num(); i++) {
				BoxGroups[i]->TypeIndex = i;
				for (int j = 0; j < BoxArray.Num(); j++) {
					// 맞는 타입의 Box 배당
					if (BoxArray[j]->Type == TypeArr[i]) {
						BoxGroups[i]->Boxes.Add(BoxArray[j]);
						// 필터 되었던 박스면 그룹에서의 순서를 저장
						if (FilteredBoxIndexes.Contains(j)) {
							BoxGroups[i]->FilteredIndexes.Add(BoxGroups[i]->Boxes.Num()-1);
						}
					}
				}
			}

			// Group AreaKindColors
			for (auto type : TypeArr) {
				BoxColorArray.Add(FVector(FMath::RandRange(0.1f, 1.0f), FMath::RandRange(0.1f, 1.0f), FMath::RandRange(0.1f, 1.0f)));
			}
		}
		else {
			// AreaKindColors
			for (int i = 0; i < BoxArray.Num(); i++) {
				if (BoxArray[i]->SizeX * BoxArray[i]->SizeY != lastArea) {
					lastArea = BoxArray[i]->SizeX * BoxArray[i]->SizeY;
					lastColor = FVector(FMath::RandRange(0.1f, 1.0f), FMath::RandRange(0.1f, 1.0f), FMath::RandRange(0.1f, 1.0f));
				}
				BoxColorArray.Add(lastColor);
			}
		}
			
	}
}

void ABoxNesting::NestOneArray(TArray<FBoxListTableRow*> Boxes, int StartBoardIndex)
{
	// 순차적으로 Box 자리 찾기 
	for (int i = 0; i < Boxes.Num(); i++) {
		int falseSpaceCount = 0;
		int boxSizeX = Boxes[i]->SizeX;
		int boxSizeY = Boxes[i]->SizeY;
		int startPoint[2] = { 0, 0 };
		
		// 필터에 걸리는 박스면 종료
		bool isFilted = (bCanTypeGrouping && BoxGroups[CurrentTypeGroupIndex]->FilteredIndexes.Contains(i)) ||
			(!bCanTypeGrouping && FilteredBoxIndexes.Contains(i));
		if (isFilted) goto Finished_Box;

		// Board 부족 확인
		if (StartBoardIndex >= BoardStructArr.Num()) {
			AddBoard();
		}

		// 첫 보드부터 탐색 
		for (int j = StartBoardIndex; j < BoardStructArr.Num(); j++) {
			FBoxBoardStruct board = BoardStructArr[j];

			// 마지막 보드의 공백 영역이 박스 사이즈보다 작으면 자리확인 종료
			int boxArea = Boxes[i]->SizeX * Boxes[i]->SizeY;
			bool isLastBoard = j == BoardStructArr.Num() - 1;
			if (isLastBoard && board.EmptyArea < boxArea) {
				goto Finished_Counting;
			}

			// 탐색 전 보드 내부의 시작점 갱신
			for (int boardY = PaddingY; boardY < BoardSizeY - PaddingY; boardY++) {
				for (int boardX = PaddingX; boardX < BoardSizeX - PaddingX; boardX++) {
					startPoint[0] = eAlign == EBoxAlign::E_Top ? boardX : boardY;
					startPoint[1] = eAlign == EBoxAlign::E_Top ? boardY : boardX;

					// 시작점부터 박스면적만큼 탐색
					falseSpaceCount = GetFalseAreaInBoard(boxSizeX, boxSizeY, board, startPoint);

					// 공간이 충분치 않으면 회전 후 다시 탐색
					if (falseSpaceCount != boxArea && bCanBoxRotating) {
						boxSizeX = Boxes[i]->SizeY;
						boxSizeY = Boxes[i]->SizeX;
						falseSpaceCount = GetFalseAreaInBoard(boxSizeX, boxSizeY, board, startPoint);
					}

					// 회전 후에도 보드 폭(길이)가 짧고, 박스 정사각형이 아니면, 이전 박스도 돌려도 공간이 충분한지 확인
					bool canRotateBeforeBox = (eAlign == EBoxAlign::E_Top && BeforeBoxSize.X > BeforeBoxSize.Y) ||
						(eAlign == EBoxAlign::E_Left && BeforeBoxSize.Y > BeforeBoxSize.X);
					if (falseSpaceCount < 0 && canRotateBeforeBox && bCanBoxRotating) {
						int beforeFalseCount = GetFalseAreaInBoard(BeforeBoxSize.Y, BeforeBoxSize.X, BeforeBoard, BeforeStartPoint);
						// 이전 박스 회전 성공
						if (beforeFalseCount == BeforeBoxSize.Y * BeforeBoxSize.X) {
							// Board Struct - matrix
							int loopX = BeforeStartPoint[0] + BeforeBoxSize.Y + BoxDistance;
							int loopY = BeforeStartPoint[1] + BeforeBoxSize.X + BoxDistance;
							loopX = loopX <= BoardSizeX ? loopX - BeforeStartPoint[0] : BoardSizeX - BeforeStartPoint[0]; // padding 고려해 재설정
							loopY = loopY <= BoardSizeY ? loopY - BeforeStartPoint[1] : BoardSizeY - BeforeStartPoint[1];
							for (int y = 0; y < loopY; y++) {
								for (int x = 0; x < loopX; x++) {
									BeforeBoard.Matrix[BeforeStartPoint[1] + y].RowArr[BeforeStartPoint[0] + x] = 0;
								}
							}

							// 새 박스 테스트
							int tempFalseSpaceCount = 0;
							int dist;
							int tempStartPoint[2];
							if (eAlign == EBoxAlign::E_Top) {
								dist = BeforeBoxSize.X - BeforeBoxSize.Y;
								tempStartPoint[0] = startPoint[0] - dist;
								tempStartPoint[1] = startPoint[1];
								tempFalseSpaceCount = GetFalseAreaInBoard(boxSizeX, boxSizeY, BeforeBoard, tempStartPoint);
							}
							else {
								dist = BeforeBoxSize.Y - BeforeBoxSize.X;
								tempStartPoint[0] = startPoint[0];
								tempStartPoint[1] = startPoint[1] - dist;
								tempFalseSpaceCount = GetFalseAreaInBoard(boxSizeX, boxSizeY, BeforeBoard, tempStartPoint);
							}

							// 두 박스 모두 배치 가능
							if (tempFalseSpaceCount == boxArea) {
								// Board Struct - matrix
								BoardStructArr[j] = BeforeBoard;

								// Before Box Actor
								int alpha = (BoardSizeX + BoardDistance) * Unit * j;
								FVector boxLocation = FVector(
									(BeforeStartPoint[0] * Unit) + (BeforeBoxSize.Y * Unit * 0.5f) + alpha,
									(BeforeStartPoint[1] * Unit) + (BeforeBoxSize.X * Unit * 0.5f),
									0.0f
								);
								FVector scale = FVector(BeforeBoxSize.Y, BeforeBoxSize.X, 1.0f);
								if (bIsColorfullBox) {
									BoxActorArr[BoxActorArr.Num() - 1]->SetActorRelativeLocation(boxLocation);
								}
								else {
									BoxISM->UpdateInstanceTransform(BoxISM->GetInstanceCount() - 1, FTransform(FRotator().ZeroRotator, boxLocation, scale));
								}

								// 새 박스
								falseSpaceCount = tempFalseSpaceCount;
								startPoint[0] = tempStartPoint[0];
								startPoint[1] = tempStartPoint[1];
							}
						}
					}

					// 공간 충분하면 배치 및 자리확인 종료
					if (falseSpaceCount == boxArea) {
						// Backup & fork infomation
						BeforeBoard = BoardStructArr[j];
						BeforeBoxSize = FVector2D(boxSizeX, boxSizeY);
						BeforeStartPoint[0] = startPoint[0];
						BeforeStartPoint[1] = startPoint[1];

						// Board Struct - matrix
						int loopX = startPoint[0] + boxSizeX + BoxDistance;
						int loopY = startPoint[1] + boxSizeY + BoxDistance;
						loopX = loopX <= BoardSizeX ? loopX - startPoint[0] : BoardSizeX - startPoint[0]; // padding 고려해 재설정
						loopY = loopY <= BoardSizeY ? loopY - startPoint[1] : BoardSizeY - startPoint[1];
						for (int y = 0; y < loopY; y++) {
							for (int x = 0; x < loopX; x++) {
								BoardStructArr[j].Matrix[startPoint[1] + y].RowArr[startPoint[0] + x] = 0;
							}
						}
						BoardStructArr[j].EmptyArea -= loopX * loopY;

						// Box Actor
						int alpha = (BoardSizeX + BoardDistance) * Unit * j;
						FVector boxLocation = FVector(
							(startPoint[0] * Unit) + (boxSizeX * Unit * 0.5f) + alpha,
							(startPoint[1] * Unit) + (boxSizeY * Unit * 0.5f),
							0.0f
						);
						FVector scale = FVector(boxSizeX, boxSizeY, 1.0f);
						if (bIsColorfullBox) {
							ATileBase* tempBox = GetWorld()->SpawnActor<ATileBase>(BoxClass, boxLocation, FRotator().ZeroRotator);
							if (tempBox) {
								tempBox->Type = Boxes[i]->Type;
								tempBox->SetActorRelativeScale3D(scale);
								TArray<UActorComponent*> staticMeshes = tempBox->GetComponentsByTag(UStaticMeshComponent::StaticClass(), FName("Tile"));
								if (staticMeshes.Num()) {
									UStaticMeshComponent* staticMesh = Cast<UStaticMeshComponent>(staticMeshes[0]);
									UMaterialInstanceDynamic* material = UMaterialInstanceDynamic::Create(staticMesh->GetMaterial(0), NULL);
									material->SetVectorParameterValue("Color", !bCanTypeGrouping ? BoxColorArray[i] : BoxColorArray[CurrentTypeGroupIndex]);
									staticMesh->SetMaterial(0, material);
								}
								BoxActorArr.Add(tempBox);
							}
						}
						else {
							BoxISM->AddInstance(FTransform(FRotator().ZeroRotator, boxLocation, scale));
						}

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
	}
}

void ABoxNesting::DrawFilteredBox()
{
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

/*  보드 폭(길이)가 짧고 시작점이 false(1)이면 -1 리턴, 시작점이 true면 0 리턴, 이 외에는 찾은 false만큼 리턴 */
int ABoxNesting::GetFalseAreaInBoard(int BoxSizeX, int BoxSizeY, FBoxBoardStruct _Board, int StartPoint[2])
{
	int xIndex = eAlign == EBoxAlign::E_Top ? 0 : 1;
	int yIndex = eAlign == EBoxAlign::E_Top ? 1 : 0;
	int falseCount = 0;
	if (StartPoint[0] + BoxSizeX > BoardSizeX || StartPoint[1] + BoxSizeY > BoardSizeY) { 
		if (_Board.Matrix[StartPoint[yIndex]].RowArr[StartPoint[xIndex]] == 1) {
			return -1;
		}
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

