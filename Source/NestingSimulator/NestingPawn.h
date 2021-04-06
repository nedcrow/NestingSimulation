// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "NestingPawn.generated.h"

UCLASS()
class NESTINGSIMULATOR_API ANestingPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ANestingPawn();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class USphereComponent* Sphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UCameraComponent* Camera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UFloatingPawnMovement* Movement;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Translate
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pawn")
	float TurnSpeed = 0.1f;
	void MoveForward(float Value);
	void MoveRight(float Value);
	void TurnRight(float Value);
	void ZoomIn(float Value);
};
