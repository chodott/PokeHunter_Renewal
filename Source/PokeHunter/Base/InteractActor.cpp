// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractActor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"

// Sets default values
AInteractActor::AInteractActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	bReplicates = true;

	SceneRootComponent = CreateDefaultSubobject<USceneComponent>("My Scene Component");
	SetRootComponent(SceneRootComponent);

	//Collision
	InteractionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("InteractSphere"));
	InteractionSphere->AddLocalOffset(FVector(0.f, 0.f, GetSimpleCollisionHalfHeight()));
	InteractionSphere->SetSphereRadius(50.f);
	InteractionSphere->SetupAttachment(GetRootComponent());


	//Mesh
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMesh->SetupAttachment(GetRootComponent());

	
}

// Called when the game starts or when spawned
void AInteractActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AInteractActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AInteractActor::Interact_Implementation(AHunter* Hunter)
{
	Master = Hunter;
}
