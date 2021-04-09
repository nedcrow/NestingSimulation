// Fill out your copyright notice in the Description page of Project Settings.


#include "NestingPawn.h"
#include "NestingGS.h"
#include "NestingPlayerController.h"
#include "Camera/CameraComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ANestingPawn::ANestingPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	RootComponent = Sphere;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(Sphere);
	SpringArm->SetRelativeRotation(FRotator(-45.0f, 0.0f, 0.0f));
	SpringArm->TargetArmLength = 3000.0f;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);

	Movement = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("Movement"));
	Movement->UpdatedComponent = RootComponent;

	bUseControllerRotationYaw = true;
}

// Called when the game starts or when spawned
void ANestingPawn::BeginPlay()
{
	Super::BeginPlay();
	RootComponent->SetRelativeLocation(FVector(.0f, .0f, 300.0f));
	RootComponent->SetRelativeRotation(FRotator(.0f, 90.0f, .0f));
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
	PlayerInputComponent->BindAction(TEXT("ZoomIn"), IE_Released, this, &ANestingPawn::ZoomIn);
	PlayerInputComponent->BindAction(TEXT("ZoomOut"), IE_Released, this, &ANestingPawn::ZoomOut);
	PlayerInputComponent->BindAction(TEXT("CameraRotationAround"), IE_Pressed, this, &ANestingPawn::SetCurrentMousePosition);
	PlayerInputComponent->BindAction(TEXT("CameraRotationAround"), IE_Released, this, &ANestingPawn::CameraRotationAround);
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

void ANestingPawn::ZoomIn()
{
	if (SpringArm->TargetArmLength - WheelSpeed <= 300) SpringArm->TargetArmLength = 300;

	SpringArm->TargetArmLength -= WheelSpeed;
}

void ANestingPawn::ZoomOut()
{
	//if (SpringArm->TargetArmLength + WheelSpeed >= 3000) SpringArm->TargetArmLength = 3000;
	SpringArm->TargetArmLength += WheelSpeed;
}

void ANestingPawn::CameraRotationAround()
{
	ANestingPlayerController* PC = Cast<ANestingPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (PC) {
		FVector2D tempMousePosition;
		int32 x;
		int32 y;
		PC->GetViewportSize(x,y);
		PC->GetLocalPlayer()->ViewportClient->GetMousePosition(tempMousePosition);
		RootComponent->SetRelativeRotation(FRotator(
			tempMousePosition.X - CurrentMousePosition.X / x,
			y,
			0.0f
		));
	}
}

void ANestingPawn::SetCurrentMousePosition()
{
	ANestingPlayerController* PC = Cast<ANestingPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (PC) {
		PC->GetLocalPlayer()->ViewportClient->GetMousePosition(CurrentMousePosition);
	}
}
