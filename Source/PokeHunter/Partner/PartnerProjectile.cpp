// Fill out your copyright notice in the Description page of Project Settings.


#include "PartnerProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"

// Sets default values
APartnerProjectile::APartnerProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->InitialSpeed = 300.f;
	ProjectileMovement->MaxSpeed = 300.f;
	ProjectileMovement->bShouldBounce = false;
}

// Called when the game starts or when spawned
void APartnerProjectile::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APartnerProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APartnerProjectile::FireInDirection(const FVector& ShootDirection)
{
	ProjectileMovement->Velocity = ShootDirection * ProjectileMovement->InitialSpeed;
}

