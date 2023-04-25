// Fill out your copyright notice in the Description page of Project Settings.


#include "Potion.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "PokeHunter/Hunter/Hunter.h"

APotion::APotion()
{
	ItemType = EItemType::Potion;
	StaticMesh->SetEnableGravity(false);
	ProjectileMovement->ProjectileGravityScale = 0.f;
	ProjectileMovement->Deactivate();
	

}

void APotion::UseItem_Implementation(APawn* ItemOwner)
{
	AHunter* Hunter = Cast<AHunter>(ItemOwner);
	if (Hunter) Hunter->HunterHP += HealAmount;
	
}


void APotion::UseItem(APawn* ItemOwner, FVector InitialPos, FVector EndPos)
{
	StaticMesh->SetSimulatePhysics(true);
	StaticMesh->SetEnableGravity(true);
	StaticMesh->SetCollisionProfileName(FName("Bullet"));
	ProjectileMovement->ProjectileGravityScale = 1.f;
	ProjectileMovement->Activate();
	Super::UseItem(ItemOwner, InitialPos, EndPos);
	
}

void APotion::ApplyAbillity(AActor* OtherActor, UPrimitiveComponent* OtherComponent)
{
	Destroy();
}

