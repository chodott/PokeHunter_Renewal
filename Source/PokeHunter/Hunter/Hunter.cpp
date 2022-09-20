// Fill out your copyright notice in the Description page of Project Settings.


#include "Hunter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"

// Sets default values
AHunter::AHunter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//카메라
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetOffset = FVector(0, 0, GetDefaultHalfHeight());
	CameraBoom->TargetArmLength = 500.f;
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	//컨트롤러 회전 시 회전 x
	bUseControllerRotationYaw = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
}

// Called when the game starts or when spawned
void AHunter::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AHunter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AHunter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis("MoveForward", this, &AHunter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AHunter::MoveRight);
	PlayerInputComponent->BindAxis("Turn", this, &AHunter::AddControllerYawInput);
	PlayerInputComponent->BindAxis("PitchUp", this, &AHunter::AddControllerPitchInput);

	PlayerInputComponent->BindAction("RMB", IE_Pressed, this, &AHunter::RMBDown);


}

void AHunter::AddControllerPitchInput(float Val)
{
	APawn::AddControllerYawInput(Val);
}

void AHunter::AddControllerYawInput(float Val)
{
	APawn::AddControllerPitchInput(Val);
}

void AHunter::MoveForward(float Val)
{
	if (Val != 0.0f)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

		//방향 벡터
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Val);
	}
}

void AHunter::MoveRight(float Val)
{
	if (Val != 0.0f)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

		//방향 벡터
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, Val);
	}
}

void AHunter::RMBDown()
{

}