// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseCharacter.h"
#include "Net/UnrealNetwork.h"
#include "NiagaraFunctionLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
ABaseCharacter::ABaseCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//Invincible
	if (bInvincible)
	{
		CurInvincibleTime += DeltaTime;
		if(InvincibleTime <= CurInvincibleTime)
		{
			CurInvincibleTime = 0.f;
			bInvincible = false;
		}
	}

	if (bNoCollision)
	{
		CurNoCollisionTime += DeltaTime;
		if (NoCollisionTime <=CurNoCollisionTime)
		{
			CurNoCollisionTime = 0.f;
			bNoCollision = false;
			SetActorEnableCollision(true);
		}
	}
}

// Called to bind functionality to input
void ABaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ABaseCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABaseCharacter, HP);
}

void ABaseCharacter::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
}

void ABaseCharacter::MultiStartInvincibility_Implementation()
{
	bInvincible = true;
}

void ABaseCharacter::ServerStartInvincibility_Implementation()
{
	MultiStartInvincibility();
}

void ABaseCharacter::InteractEarthquake_Implementation()
{
	if (GetCharacterMovement()->IsFalling()) return;
	if (bInvincible) return;


	LaunchCharacter(FVector(0, 0, 1000), false, false);
}

void ABaseCharacter::InteractAttack_Implementation(FVector HitDirection, float DamageAmount)
{
	if (DamageAmount <= 0.f || bDamaged)
	{
		return;
	}

	if (bInvincible) return;

	if (HitDirection.Z < 0.f) HitDirection.Z *= -1;
	if (HitDirection.Z <= 0.3f) HitDirection.Z = 0.35f;

	//bDamaged = true;
	FVector TargetVec = FVector(HitDirection.X * -1, HitDirection.Y * -1, 0);
	FRotator TargetRot = TargetVec.Rotation();
	TargetRot.Pitch = 0;
	SetActorRelativeRotation(TargetRot);
	//CurState = EPlayerState::Knockback;

	LaunchCharacter(HitDirection * 1000.f, false, false);
	bNoCollision = true;
	SetActorEnableCollision(false);
}

void ABaseCharacter::InteractGrabAttack_Implementation()
{
	bNoCollision = true;
	FRotator TargetRot = FVector::ZeroVector.Rotation();
}

void ABaseCharacter::InteractWideAttack_Implementation(float DamageAmount)
{
	if (DamageAmount <= 0.f || bDamaged)
	{
		return;
	}

	if (bInvincible) return;

	//CurState = EPlayerState::Knockback;
	LaunchCharacter(FVector(0, 0, 1000), false, false);
	bNoCollision = true;
	SetActorEnableCollision(false);
}

void ABaseCharacter::InteractPotion_Implementation(float HealAmount)
{
	SetHP(GetHP() + HealAmount);
}

void ABaseCharacter::InteractHealArea_Implementation()
{
	HealPerSecondAmount += 10.f;
}

void ABaseCharacter::OutHealArea_Implementation()
{
	HealPerSecondAmount -= 10.f;
}

void ABaseCharacter::ServerPlayMontage_Implementation(FName Session)
{
	MultiPlayMontage(Session);
}

void ABaseCharacter::MultiPlayMontage_Implementation(FName Session)
{
}

void ABaseCharacter::ServerSpawnEffect_Implementation(class UNiagaraSystem* Niagara, const FVector& SpawnLoc)
{
	MultiSpawnEffect(Niagara, SpawnLoc);
}


void ABaseCharacter::MultiSpawnEffect_Implementation(class UNiagaraSystem* Niagara, const FVector& SpawnLoc)
{
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), Niagara, SpawnLoc);
}