// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"
#include "EnemyAnimInstance.h"
#include "components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "PokeHunter/Item/Item.h"
#include "PokeHunter/Hunter/Hunter.h"
#include "PokeHunter/Partner/Partner.h"

// Sets default values
AEnemy::AEnemy()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	

	GetCharacterMovement()->bOrientRotationToMovement = true;
	
	GetCapsuleComponent()->SetCapsuleRadius(90.f);
	GetCapsuleComponent()->OnComponentHit.AddDynamic(this, &AEnemy::OnHit);
	
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	EnemyAnim = Cast<UEnemyAnimInstance>(GetMesh()->GetAnimInstance());
	EnemyAnim->OnMontageEnded.AddDynamic(this, &AEnemy::OnMontageEnded);
	
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
		if (EnemyAnim != NULL && HP > 0)
		{
			HP -= 5;
			if (HP <= 0)
			{
				EnemyAnim->PlayCombatMontage(FName("Die"));
			}
			else
			{
				if (Target == NULL)
				{
					EnemyAnim->PlayCombatMontage(FName("Hit"));
					HitItem->Hunter->SetPartnerTarget(this);
					Target = HitItem->Hunter;
					bFirstHit = 0;
				}
			}
		}
		HitItem->Destroy();
	}
}

void AEnemy::Attack()
{
	if (Target != NULL)
	{
		EnemyAnim->PlayCombatMontage(TEXT("Attack"));
	}
}

void AEnemy::Roar()
{
	if (EnemyAnim != NULL)
	{
		EnemyAnim->PlayCombatMontage(TEXT("Roar"));
	}
}

void AEnemy::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{

	EnemyAnim->bPlaying = false;

	OnMontageEnd.Broadcast();
}

