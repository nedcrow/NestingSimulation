// Fill out your copyright notice in the Description page of Project Settings.


#include "BoxNesting.h"
#include "../TileBase.h"
#include "../NestingGS.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Engine/DataTable.h"
#include "BoxDataTable.h"
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
	UGameplayStatics::GetAllActorsOfClassWithTag(GetWorld(),ATileBase::StaticClass(),"Tile", GarbageBoxActorArr);
	for (int i = GarbageBoxActorArr.Num() - 1; i > 0; i--) {
		GarbageBoxActorArr[i]->Destroy();
	}
	GarbageBoxActorArr.Empty();
	ResetUnit();
	CreateBoard(BoardSizeX, BoardSizeY, true);
	NestBox();
#endif
}

void ABoxNesting::BeginPlay()
{
	Super::BeginPlay();
	//ResetUnit();
	//CreateBoard(BoardSizeX, BoardSizeY, true);
	//NestBox();
}

// Called every frame
void ABoxNesting::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

/* must, this function have call first.*/
void ABoxNesting::ResetUnit()
{
	ANestingGS* GS = Cast<ANestingGS>(UGameplayStatics::GetGameState(GetWorld()));
	if(GS) Unit = GS->Unit;
}

void ABoxNesting::CreateBoard(int _BoardSizeX, int _BoardSizeY, bool IsReset) {
	// Reset - �����ϴ� ������ �����Ϳ��� �÷��� ���� �ʰ� Ȯ���ϱ� ����.
	if (IsReset) {
		BoardISM->ClearInstances();
		BoardStructArr.Empty();
		FilteredBoxIndexes.Empty();
		AreaKindColors.Empty();
		for (int i = BoxActorArr.Num() - 1; i > 0; i--) {
			BoxActorArr[i]->Destroy();
		}
		BoxActorArr.Empty();
	}

	// Board Actor - ������� + ����������� + �������尣��
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
	}// PaddingX - ��� ���� ���۰� �� ĭ�� ���
	for (int i = 0; i < PaddingY; i++) {
		Board.Matrix[i].RowArr.Init(0, _BoardSizeX);
		Board.Matrix[_BoardSizeY - 1 - i].RowArr.Init(0, _BoardSizeX);
	}// PaddingY - ���۰� �� ���� ��� ĭ�� ���
	Board.EmptyArea = (_BoardSizeX - PaddingX * 2) * (_BoardSizeY - PaddingY * 2);

	BoardStructArr.Add(Board);
}

void ABoxNesting::AddBoard() {

	CreateBoard(BoardSizeX, BoardSizeY, false);
	ANestingGS* GS = Cast<ANestingGS>(UGameplayStatics::GetGameState(GetWorld()));
	if (GS) {
		GS->SetCurrentBoardSize(BoardSizeX, BoardSizeY, BoardDistance);
		GS->SetCurrentBoardCount(BoardStructArr.Num());
	}
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

		// ���������� Box �ڸ� ã�� 
		for (int i = 0; i < BoxArray.Num(); i++) {
			int falseSpaceCount = 0;
			int boxSizeX = BoxArray[i]->SizeX;
			int boxSizeY = BoxArray[i]->SizeY;
			int startPoint[2] = { 0, 0 };

			// ���Ϳ� �ɸ��� �ڽ��� ����
			if (FilteredBoxIndexes.Contains(i)) {
				goto Finished_Box;
			}

			// ù ������� Ž�� 
			for (int j = 0; j < BoardStructArr.Num(); j++) {
				FBoardStruct board = BoardStructArr[j];

				// ������ ������ ���� ������ �ڽ� ������� ������ �ڸ�Ȯ�� ����
				int boxArea = BoxArray[i]->SizeX * BoxArray[i]->SizeY;
				bool isLastBoard = j == BoardStructArr.Num() - 1;
				if (isLastBoard && board.EmptyArea < boxArea) {
					goto Finished_Counting;
				}

				// Ž�� �� ���� ������ ������ ����
				for (int boardY = PaddingY; boardY < BoardSizeY - PaddingY; boardY++) {
					for (int boardX = PaddingX; boardX < BoardSizeX - PaddingX; boardX++) {
						startPoint[0] = eAlign == EBoxAlign::E_Top ? boardX : boardY;
						startPoint[1] = eAlign == EBoxAlign::E_Top ? boardY : boardX;

						// ���������� �ڽ�������ŭ Ž��
						falseSpaceCount = GetFalseAreaInBoard(boxSizeX, boxSizeY, board, startPoint);

						// ������ ���ġ ������ ȸ�� �� �ٽ� Ž��
						if (falseSpaceCount != boxArea && bCanBoxRotating) {
							boxSizeX = BoxArray[i]->SizeY;
							boxSizeY = BoxArray[i]->SizeX;
							falseSpaceCount = GetFalseAreaInBoard(boxSizeX, boxSizeY, board, startPoint);
						}

						// ȸ�� �Ŀ��� ���� ��(����)�� ª��, �ڽ� ���簢���� �ƴϸ�, ���� �ڽ��� ������ ������ ������� Ȯ��
						bool canRotateBeforeBox = (eAlign == EBoxAlign::E_Top && BeforeBoxSize.X > BeforeBoxSize.Y) ||
							(eAlign == EBoxAlign::E_Left && BeforeBoxSize.Y > BeforeBoxSize.X);
						if (falseSpaceCount < 0 && canRotateBeforeBox && bCanBoxRotating) {
							int beforeFalseCount = GetFalseAreaInBoard(BeforeBoxSize.Y, BeforeBoxSize.X, BeforeBoard, BeforeStartPoint);
							// ���� �ڽ� ȸ�� ����
							if (beforeFalseCount == BeforeBoxSize.Y * BeforeBoxSize.X) {
								// Board Struct - matrix
								int loopX = BeforeStartPoint[0] + BeforeBoxSize.Y + BoxDistance;
								int loopY = BeforeStartPoint[1] + BeforeBoxSize.X + BoxDistance;
								loopX = loopX <= BoardSizeX ? loopX - BeforeStartPoint[0] : BoardSizeX - BeforeStartPoint[0]; // padding ����� �缳��
								loopY = loopY <= BoardSizeY ? loopY - BeforeStartPoint[1] : BoardSizeY - BeforeStartPoint[1];
								for (int y = 0; y < loopY; y++) {
									for (int x = 0; x < loopX; x++) {
										BeforeBoard.Matrix[BeforeStartPoint[1] + y].RowArr[BeforeStartPoint[0] + x] = 0;
									}
								}

								// �� �ڽ� �׽�Ʈ
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

								// �� �ڽ� ��� ��ġ ����
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
										BoxActorArr[BoxActorArr.Num()-1]->SetActorRelativeLocation(boxLocation);
									}
									else {
										BoxISM->UpdateInstanceTransform(BoxISM->GetInstanceCount() - 1, FTransform(FRotator().ZeroRotator, boxLocation, scale));
									}

									// �� �ڽ�
									falseSpaceCount = tempFalseSpaceCount;
									startPoint[0] = tempStartPoint[0];
									startPoint[1] = tempStartPoint[1];
								}
							}
						}

						// ���� ����ϸ� ��ġ �� �ڸ�Ȯ�� ����
						if (falseSpaceCount == boxArea) {
							// Backup & fork infomation
							BeforeBoard = BoardStructArr[j];
							BeforeBoxSize = FVector2D(boxSizeX, boxSizeY);
							BeforeStartPoint[0] = startPoint[0];
							BeforeStartPoint[1] = startPoint[1];

							// Board Struct - matrix
							int loopX = startPoint[0] + boxSizeX + BoxDistance;
							int loopY = startPoint[1] + boxSizeY + BoxDistance;
							loopX = loopX <= BoardSizeX ? loopX - startPoint[0] : BoardSizeX - startPoint[0]; // padding ����� �缳��
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
									tempBox->SetActorRelativeScale3D(scale);
									TArray<UActorComponent*> staticMeshes = tempBox->GetComponentsByTag(UStaticMeshComponent::StaticClass(),FName("Tile"));
									if (staticMeshes.Num()) {
										UStaticMeshComponent* staticMesh = Cast<UStaticMeshComponent>(staticMeshes[0]);
										UMaterialInstanceDynamic* material = UMaterialInstanceDynamic::Create(staticMesh->GetMaterial(0), NULL);
										material->SetVectorParameterValue("Color", AreaKindColors[i]);
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
			// ���� �� ���Ҵµ� ��� ���� ���. ���� �ڽ� ����, ���� ���� ����
			i--;
			AddBoard();

			Finished_Box: {}
		}

		// ���Ϳ� �ɸ� �ڽ� ���� �׸���
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

/*  ���� ��(����)�� ª�� �������� false(1)�̸� -1 ����, �������� true�� 0 ����, �� �ܿ��� ã�� false��ŭ ���� */
int ABoxNesting::GetFalseAreaInBoard(int BoxSizeX, int BoxSizeY, FBoardStruct _Board, int StartPoint[2])
{
	int falseCount = 0;
	if (StartPoint[0] + BoxSizeX > BoardSizeX || StartPoint[1] + BoxSizeY > BoardSizeY) { 
		if (_Board.Matrix[StartPoint[1]].RowArr[StartPoint[0]] == 1) {
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

