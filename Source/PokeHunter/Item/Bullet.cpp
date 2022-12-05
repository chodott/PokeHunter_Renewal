// Fill out your copyright notice in the Description page of Project Settings.


#include "Bullet.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "PokeHunter/Hunter/Hunter.h"

ABullet::ABullet()
{

	//Collision
	StaticMesh->SetCollisionProfileName("Bullet");

	//Movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->InitialSpeed = 100.f;
	ProjectileMovement->MaxSpeed = 100.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = true;

};

void ABullet::UseItem(AHunter* ItemOwner)
{
	Hunter = ItemOwner;
	ProjectileMovement->Velocity = GetActorForwardVector() * ProjectileMovement->InitialSpeed;
}