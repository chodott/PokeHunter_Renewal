// Fill out your copyright notice in the Description page of Project Settings.


#include "NpcPartnerManager.h"
#include "../Partner/Partner.h"
#include "../Partner/PartnerController.h"

ANpcPartnerManager::ANpcPartnerManager()
{
	CameraBoom->TargetArmLength = 500.f;
}

void ANpcPartnerManager::BeginPlay()
{
	FVector BaseLocation = GetActorLocation();
	for (int i = 0; i<PartnerClassArray.Num(); ++i)
	{
		FVector location = FVector(BaseLocation.X + 100.f, BaseLocation.Y + PARTNER_GAP * (i - 1), BaseLocation.Z);
		APartner* temp = GetWorld()->SpawnActor<APartner>(PartnerClassArray[i], location, GetActorRotation());
		PartnerArray.Add(temp);
	}
	PosePos = FVector(BaseLocation.X + 150, BaseLocation.Y, BaseLocation.Z);
}

void ANpcPartnerManager::Tick(float DeltaTime)
{

}

void ANpcPartnerManager::interact_Implementation(AHunter* Hunter)
{
	Super::interact_Implementation(Hunter);

}

void ANpcPartnerManager::SelectPartner()
{
	UE_LOG(LogTemp, Warning, TEXT("select success"));
	PartnerArray[CurrentIndex]->TargetPos = PosePos;
	PartnerArray[CurrentIndex]->bPosing = true;
}

void ANpcPartnerManager::AddIndex(int Val)
{
	CurrentIndex += Val;
}
