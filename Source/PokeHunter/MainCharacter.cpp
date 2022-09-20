//// Fill out your copyright notice in the Description page of Project Settings.
//
//
//#include "MainCharacter.h"
//#include "GameFramework/SpringArmComponent.h"
//#include "Camera/CameraComponent.h"
//
//// Sets default values
//AMainCharacter::AMainCharacter()
//{
// 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
//	PrimaryActorTick.bCanEverTick = true;
//
//	//Ä«¸Þ¶ó
//	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
//	CameraBoom->SetupAttachment(GetRootComponent());
//	CameraBoom->TargetOffset = FVector(0, 0, GetDefaultHalfHeight());
//	CameraBoom->TargetArmLength = 500.f;
//	CameraBoom->bUsePawnControlRotation = true;
//
//}
//
//// Called when the game starts or when spawned
//void AMainCharacter::BeginPlay()
//{
//	Super::BeginPlay();
//	
//}
//
//// Called every frame
//void AMainCharacter::Tick(float DeltaTime)
//{
//	Super::Tick(DeltaTime);
//
//}
//
//// Called to bind functionality to input
//void AMainCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
//{
//	Super::SetupPlayerInputComponent(PlayerInputComponent);
//
//}
//
//void AMainCharacter::AddControllerPitchInput(float Val)
//{
//
//}
//
//void AMainCharacter::AddControllerYawInput(float Val)
//{
//
//}
//
//void AMainCharacter::MoveForward(float Val)
//{
//
//}
//
//void AMainCharacter::MoveRight(float Val)
//{
//
//}
