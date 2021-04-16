// Fill out your copyright notice in the Description page of Project Settings.


#include "NestingGS.h"

void ANestingGS::SetCurrentBoardSize(int X, int Y, int Alpha)
{
	CurrentBoardSize = FVector2D(X, Y);
	CurrentBoardDistance = Alpha;
}

/* Return x_y size is centimeter */
FVector2D ANestingGS::GetSizeOfBoard()
{
	return FVector2D(CurrentBoardSize.X * Unit, CurrentBoardSize.Y * Unit);
}

int ANestingGS::GetAreaOfBoards() {
	return (CurrentBoardSize.X + CurrentBoardDistance) * CurrentBoardSize.Y * CurrentBoardCount * Unit;
}
