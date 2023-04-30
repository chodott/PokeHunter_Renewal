// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyBomb.h"
#include "GameFramework/ProjectileMovementComponent.h"

AEnemyBomb::AEnemyBomb()
{
	ProjectileMovement->InitialSpeed = 0.f;
	ProjectileMovement->MaxSpeed = 0.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = true;
	ProjectileMovement->ProjectileGravityScale = 1.f;

	float Roll = FMath::RandRange(0.0f, 360.0f);
	float Pitch = FMath::RandRange(0.0f, 360.0f);
	float Yaw = FMath::RandRange(0.0f, 360.0f);
	FRotator RandomRot = FRotator::MakeFromEuler(FVector(Roll, Pitch, Yaw));
	StaticMesh->AddLocalRotation(RandomRot);
	StaticMesh->SetSimulatePhysics(false);

}

void AEnemyBomb::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (bActive)
	{
		UpSpeed += -9.8f * GravityValue;
	}
	AddActorWorldOffset(FVector(0, 0, DeltaTime * UpSpeed),true);
}

void AEnemyBomb::DropBomb()
{
	bActive = true;
	ProjectileMovement->ProjectileGravityScale =1000.f;
	StaticMesh->SetSimulatePhysics(true);
}

void AEnemyBomb::InteractChargeAttack_Implementation()
{
	DropBomb();
}
