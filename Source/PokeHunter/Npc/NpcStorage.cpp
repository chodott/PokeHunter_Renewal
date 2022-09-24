// Fill out your copyright notice in the Description page of Project Settings.


#include "NpcStorage.h"
#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"

// Sets default values
ANpcStorage::ANpcStorage()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//¸Þ½¬
	SubStaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SubStaticMesh"));
	SubStaticMesh->SetupAttachment(StaticMesh);


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

void ANpcStorage::interact_Implementation()
{
	if (bActive) bActive = 0;
	else bActive = 1;
	SetActorTickEnabled(1);
	UE_LOG(LogTemp, Warning, TEXT("Interact"))
}
