// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"
#include "EnemyAnimInstance.h"
#include "components/CapsuleComponent.h"
#include "PokeHunter/Item/Item.h"

// Sets default values
AEnemy::AEnemy()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	
	GetCapsuleComponent()->OnComponentHit.AddDynamic(this, &AEnemy::OnHit);
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();


	
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemy::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	AItem* HitItem = Cast<AItem>(OtherActor);
	if (HitItem)
	{
		HP -= 5;
		HitItem->Destroy();
		auto AnimInstance = Cast<UEnemyAnimInstance>(GetMesh()->GetAnimInstance());


		if (AnimInstance)
		{
			if (HP <= 0)
			{
				AnimInstance->PlayCombatMontage(FName("Die"));
			}
			else
			{
				AnimInstance->PlayCombatMontage(FName("Hit"));
			}
		}
	}
}

void AEnemy::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if(HP <= 0) this->Destroy();
}

