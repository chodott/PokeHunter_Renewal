// Fill out your copyright notice in the Description page of Project Settings.

#include "HunterController.h"

void AHunterController::BeginPlay()
{
	Super::BeginPlay();
	//SetInputMode(FInputModeGameAndUI());

	PlayerCameraManager->ViewPitchMax = 45.f;
	PlayerCameraManager->ViewPitchMin = -30.f;
	
}

void AHunterController::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}
