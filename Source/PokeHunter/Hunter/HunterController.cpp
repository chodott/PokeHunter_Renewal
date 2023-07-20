// Fill out your copyright notice in the Description page of Project Settings.

#include "HunterController.h"
#include <Net/UnrealNetwork.h>
#include <PokeHunter/Base/BaseInstance.h>

AHunterController::AHunterController()
{
	SetReplicates(true);
	bAlwaysRelevant = true;
}

void AHunterController::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void AHunterController::BeginPlay()
{
	Super::BeginPlay();
	//SetInputMode(FInputModeGameAndUI());

	PlayerCameraManager->ViewPitchMax = 45.f;
	PlayerCameraManager->ViewPitchMin = -30.f;

	baseinstance = Cast<UBaseInstance>(GetGameInstance());
}

//bool AHunterController::Server_SendMaterialInfo_Validate(int32 SelectedMaterialIndex)
//{
//	return true;
//}
//
//void AHunterController::Server_SendMaterialInfo_Implementation(int32 SelectedMaterialIndex)
//{
//	if (baseinstance)
//	{
//		baseinstance->mySkin = SelectedMaterialIndex;
//	}
//}
//
//void AHunterController::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
//{
//	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
//	DOREPLIFETIME(AHunterController, SelectedMaterialIndex);
//}
//
//void AHunterController::SelectMaterial(int32 SelectedMaterialIndex)
//{
//	if (HasAuthority())
//	{
//		Server_SendMaterialInfo(SelectedMaterialIndex);
//	}
//}
