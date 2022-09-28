// Fill out your copyright notice in the Description page of Project Settings.


#include "NpcDoor.h"
#include "Blueprint/UserWidget.h"

ANpcDoor::ANpcDoor()
{

}

void ANpcDoor::BeginPlay()
{
	Super::BeginPlay();
	SetActorTickEnabled(0);
}

// Called every frame
void ANpcDoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


}

void ANpcDoor::interact_Implementation(AHunter* Hunter)
{
	Super::interact_Implementation(Hunter);
	GetWorldTimerManager().SetTimer(TimerHandle, this, &ANpcDoor::OpenUI, 1.0f, false, 1.0f);

}