// Fill out your copyright notice in the Description page of Project Settings.


#include "NpcStorage.h"
#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Blueprint/UserWidget.h"
#include "PokeHunter/Item/Item.h"
#include "PokeHunter/Hunter/InventoryComponent.h"
#include "PokeHunter/Hunter/Hunter.h"

// Sets default values
ANpcStorage::ANpcStorage()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//메쉬
	SubStaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SubStaticMesh"));
	SubStaticMesh->SetupAttachment(StaticMesh);

	//카메라
	CameraBoom->bDoCollisionTest = false;

	//인벤토리
	Storage = CreateDefaultSubobject<UInventoryComponent>(TEXT("Storage"));


}

// Called when the game starts or when spawned
void ANpcStorage::BeginPlay()
{
	Super::BeginPlay();
	SetActorTickEnabled(0);
}

// Called every frame
void ANpcStorage::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bActive)
	{
		//SubStaticMesh->AddRelativeRotation(FRotator(0, 0, -1.0f));
		SubStaticMesh->AddLocalRotation(FRotator(0, 0, -5.0f));
		float roll = SubStaticMesh->GetRelativeRotation().Roll;
		if (roll <= -90.0f) SetActorTickEnabled(0);
	}
	else
	{
		SubStaticMesh->AddLocalRotation(FRotator(0, 0, 5.0f));
		float roll = SubStaticMesh->GetRelativeRotation().Roll;
		if (roll >= 15.0f) SetActorTickEnabled(0);

	}
}

void ANpcStorage::interact_Implementation(AHunter* Hunter)
{
	Master = Hunter;
	if (bActive)
	{
		
		
		bActive = 0;
		Cast<APlayerController>(Master->GetController())->SetViewTargetWithBlend(Master, 1.0f);
		Master->StorageUI->RemoveFromViewport();
		Master = NULL;

	}
	else
	{
		bActive = 1;
		Cast<APlayerController>(Master->GetController())->SetViewTargetWithBlend(this, 1.0f);
		GetWorldTimerManager().SetTimer(TimerHandle, this, &ANpcStorage::OpenUI, 1.0f, false, 1.0f);
	}
	SetActorTickEnabled(1);
}

void ANpcStorage::OpenUI()
{
	if (Master->StorageUI == nullptr)
	{
		Master->StorageUI = CreateWidget(GetWorld(), Master->StorageUIClass);
	}
	if (Master->StorageUI->IsInViewport())
	{
		Master->StorageUI->RemoveFromViewport();
	}
	else {
		Master->StorageUI->AddToViewport();
	}
	GetWorldTimerManager().ClearTimer(TimerHandle);
}