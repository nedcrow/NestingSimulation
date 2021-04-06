// Fill out your copyright notice in the Description page of Project Settings.


#include "NestingPawn.h"
#include "Camera/CameraComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/FloatingPawnMovement.h"

// Sets default values
ANestingPawn::ANestingPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	RootComponent = Sphere;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(Sphere);

	Movement = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("Movement"));
	Movement->UpdatedComponent = RootComponent;

	bUseControllerRotationYaw = true;
}

// Called when the game starts or when spawned
void ANestingPawn::BeginPlay()
{
	Super::BeginPlay();
	RootComponent->SetRelativeLocation(FVector(.0f, .0f, 300.0f));
	Camera->SetRelativeRotation(FRotator(-45.0f, .0f, .0f));
}

// Called every frame
void ANestingPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ANestingPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &ANestingPawn::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &ANestingPawn::MoveRight);
	PlayerInputComponent->BindAxis(TEXT("TurnRight"), this, &ANestingPawn::TurnRight);
	PlayerInputComponent->BindAxis(TEXT("ZoomIn"), this, &ANestingPawn::ZoomIn);
}

void ANestingPawn::MoveForward(float Value)
{
	if (Value == 0)
	{
		return;
	}

	FVector ControlForwardVector2D = GetControlRotation().Vector().GetSafeNormal2D();
	AddMovementInput(ControlForwardVector2D, Value);
}

void ANestingPawn::MoveRight(float Value)
{
	if (Value == 0)
	{
		return;
	}

	FVector ControlRightVector2D = GetControlRotation().RotateVector(FVector::RightVector).GetSafeNormal2D();
	AddMovementInput(ControlRightVector2D, Value);
}

void ANestingPawn::TurnRight(float Value)
{
	if (Value == 0)
	{
		return;
	}

	AddControllerYawInput(Value * TurnSpeed);
}

void ANestingPawn::ZoomIn(float Value)
{
	if (Value == 0)
	{
		return;
	}

	FVector ControlDownVector2D = GetControlRotation().RotateVector(FVector::DownVector).GetSafeNormal2D();
	AddMovementInput(ControlDownVector2D, Value * 1000.0f);
}

