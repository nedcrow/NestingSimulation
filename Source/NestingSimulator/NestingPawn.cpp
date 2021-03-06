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
	SpringArm->bDoCollisionTest = false;
	SpringArm->SetRelativeRotation(FRotator(-45.0f, 0.0f, 0.0f));
	SpringArm->TargetArmLength = DefaultTargetArmLength = 3000.0f;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);

	Movement = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("Movement"));
	Movement->UpdatedComponent = RootComponent;
}

// Called when the game starts or when spawned
void ANestingPawn::BeginPlay()
{
	Super::BeginPlay();
	RootComponent->SetRelativeLocation(FVector(.0f, .0f, 300.0f));
}

// Called every frame
void ANestingPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
		
	if(bCanRotateAround)CameraRotateAround();
}

// Called to bind functionality to input
void ANestingPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &ANestingPawn::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &ANestingPawn::MoveRight);
	PlayerInputComponent->BindAxis(TEXT("TurnRight"), this, &ANestingPawn::TurnRight);
	PlayerInputComponent->BindAction(TEXT("Interaction"), IE_Released, this, &ANestingPawn::FitCameraZoom);
	PlayerInputComponent->BindAction(TEXT("ZoomIn"), IE_Released, this, &ANestingPawn::ZoomIn);
	PlayerInputComponent->BindAction(TEXT("ZoomOut"), IE_Released, this, &ANestingPawn::ZoomOut);
	PlayerInputComponent->BindAction(TEXT("CameraRotateAround"), IE_Pressed, this, &ANestingPawn::SetStartTransform);
	PlayerInputComponent->BindAction(TEXT("LeftClick"), IE_Released, this, &ANestingPawn::EndCameraRotating);
}

void ANestingPawn::MoveForward(float Value)
{
	if (Value == 0)
	{
		return;
	}

	float addSpeed = pow(SpringArm->TargetArmLength / DefaultTargetArmLength, 3);
	AddMovementInput(RootComponent->GetForwardVector(), Value * MoveSpeed * addSpeed);

	UE_LOG(LogTemp, Warning, TEXT("speed: %f"), Value * MoveSpeed * addSpeed);
}

void ANestingPawn::MoveRight(float Value)
{
	if (Value == 0)
	{
		return;
	}

	float addSpeed = SpringArm->TargetArmLength / DefaultTargetArmLength;
	AddMovementInput(RootComponent->GetRightVector(), Value * MoveSpeed * addSpeed);
	
}

void ANestingPawn::TurnRight(float Value)
{
	if (Value == 0)
	{
		return;
	}

	RootComponent->AddRelativeRotation(FRotator(
		0.0f,
		Value * TurnSpeed,
		0.0f
	));
}

void ANestingPawn::FitCameraZoom()
{
	ANestingGS* GS = Cast<ANestingGS>(UGameplayStatics::GetGameState(GetWorld()));
	if (GS) {
		float centerOfAreaX = GS->GetSizeOfBoard().X * GS->CurrentBoardCount * 0.5f;
		float inclinationOfCamera = abs((double)SpringArm->GetRelativeRotation().Pitch) / 90;
		RootComponent->SetRelativeLocation(FVector(centerOfAreaX * inclinationOfCamera, GS->GetSizeOfBoard().Y * 0.5f, 0.0f));
		SpringArm->TargetArmLength = GS->GetAreaOfBoards() * 0.01f;
	}
}

void ANestingPawn::ZoomIn()
{
	if (SpringArm->TargetArmLength - WheelSpeed <= 300) SpringArm->TargetArmLength = 300;

	SpringArm->TargetArmLength -= WheelSpeed;
}

void ANestingPawn::ZoomOut()
{
	SpringArm->TargetArmLength += WheelSpeed;
}

void ANestingPawn::CameraRotateAround()
{
	ANestingPlayerController* PC = Cast<ANestingPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (PC) {
		int32 scrX;
		int32 scrY;
		FVector2D tempMousePosition;
		
		PC->GetViewportSize(scrX,scrY);
		PC->GetLocalPlayer()->ViewportClient->GetMousePosition(tempMousePosition);

		float addRotationPitch = ((tempMousePosition.Y - StartMousePosition.Y) / scrY * MaxRotation);
		float addRotationYaw = ((tempMousePosition.X - StartMousePosition.X) / scrX * MaxRotation);

		if (addRotationPitch != 0 && addRotationYaw != 0) {
			RootComponent->SetRelativeRotation(FRotator(
				StartRootRotator.Pitch - addRotationPitch,
				StartRootRotator.Yaw + addRotationYaw,
				0.0f
			));
		}
	}
}

void ANestingPawn::EndCameraRotating()
{
	bCanRotateAround = false;
}

void ANestingPawn::SetStartTransform()
{
	ANestingPlayerController* PC = Cast<ANestingPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (PC) {
		PC->GetLocalPlayer()->ViewportClient->GetMousePosition(StartMousePosition);
		StartRootRotator = RootComponent->GetRelativeTransform().Rotator();
		bCanRotateAround = true;
	}
}
