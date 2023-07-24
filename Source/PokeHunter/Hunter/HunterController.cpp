// Fill out your copyright notice in the Description page of Project Settings.

#include "HunterController.h"
#include <Net/UnrealNetwork.h>
#include <PokeHunter/Base/BaseInstance.h>
#include <PokeHunter/Hunter/Hunter.h>
#include <PokeHunter/Base/HunterState.h>

void AHunterController::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void AHunterController::BeginPlay()
{
	Super::BeginPlay();

	PlayerCameraManager->ViewPitchMax = 45.f;
	PlayerCameraManager->ViewPitchMin = -30.f;
}

void AHunterController::SetCharacterMaterialFromGameInstance(APawn* ClientPawn)
{
	UBaseInstance* gameinstance = GetGameInstance<UBaseInstance>();
	if (gameinstance) {
		int32 MaterialIndex = gameinstance->mySkin;
		UE_LOG(LogTemp, Warning, TEXT("[DBG] AHunterController::SetCharacterMaterialFromGameInstance() -> MaterialIndex : %d"), MaterialIndex);
		Server_SetCharacterMaterialInfo(ClientPawn, MaterialIndex);
	}
}

void AHunterController::Server_SetCharacterMaterialInfo_Implementation(APawn* ClientPawn, int32 NewMaterialInfo)
{
	UBaseInstance* gameinstance = GetGameInstance<UBaseInstance>();
	if (gameinstance) {
		gameinstance->mySkin = NewMaterialInfo;
	}

	if (ClientPawn)
	{
		AHunter* hunter = Cast<AHunter>(ClientPawn);
		if (hunter != nullptr) {
			// hunter->SetNewMaterialIndex(NewMaterialInfo);
			UE_LOG(LogTemp, Warning, TEXT("[DBG] AHunterController::Server_SetCharacterMaterialInfo_Implementation() -> NewMaterialInfo : %d"), NewMaterialInfo);
			// hunter->ClientChangeMaterialIndex(NewMaterialInfo);
		}
	}
}

void AHunterController::ClientConnectToServer_Implementation(int32 NewMaterialInfo)
{
	UE_LOG(LogTemp, Warning, TEXT("[DBG] AHunterController::ClientConnectToServer_Implementation() -> NewMaterialInfo : %d"), NewMaterialInfo);
}