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
		PositionArray.Add(FVector(BaseLocation.X +100.f, BaseLocation.Y + PARTNER_GAP * (i - 1), BaseLocation.Z));
		APartner* temp = GetWorld()->SpawnActor<APartner>(PartnerClassArray[i], PositionArray[i], GetActorRotation());
		temp->TargetPos = temp->GetActorLocation();
		PartnerArray.Add(temp);
	}
	PosePos = FVector(BaseLocation.X + 300, BaseLocation.Y, BaseLocation.Z);
}

void ANpcPartnerManager::Tick(float DeltaTime)
{

}

void ANpcPartnerManager::interact_Implementation(AHunter* Hunter)
{
	Super::interact_Implementation(Hunter);

}

void ANpcPartnerManager::ChangePartner()
{
	UE_LOG(LogTemp, Warning, TEXT("Change success"));
	PartnerArray[CurrentIndex]->TargetPos = PosePos;
	PartnerArray[CurrentIndex]->bPosing = true;
}

void ANpcPartnerManager::AddIndex(int Val)
{
	if (Val < 0 && CurrentIndex == 0) return;
	else if (Val > 0 && CurrentIndex == PartnerArray.Num()-1) return;

	PartnerArray[CurrentIndex]->TargetPos = PositionArray[CurrentIndex];
	PartnerArray[CurrentIndex]->bPosing = false;
	CurrentIndex += Val;
}

void ANpcPartnerManager::SelectPartner()
{
	PartnerArray[CurrentIndex]->Hunter = Master;
	PartnerArray[CurrentIndex]->bPosing = false;
}
