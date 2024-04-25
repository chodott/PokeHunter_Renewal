// Fill out your copyright notice in the Description page of Project Settings.


#include "Partner.h"
#include "PartnerController.h"
#include "PartnerAnimInstance.h"
#include "PartnerProjectile.h"
#include "NiagaraFunctionLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "PokeHunter/Hunter/Hunter.h"
#include "PokeHunter/Enemy/Enemy.h"

// Sets default values
APartner::APartner()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	TeamID = FGenericTeamId(0);

	GetCharacterMovement()->bOrientRotationToMovement = true;

	baseinstace = Cast<UBaseInstance>(UGameplayStatics::GetGameInstance((GetWorld())));
}

// Called when the game starts or when spawned
void APartner::BeginPlay()
{
	Super::BeginPlay();

	PartnerAnim = Cast<UPartnerAnimInstance>(GetMesh()->GetAnimInstance());
	if (PartnerAnim)
	{
		PartnerAnim->OnMontageEnded.AddDynamic(this, &APartner::OnMontageEnded);
	}
}

// Called every frame
void APartner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void APartner::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void APartner::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APartner, Hunter);
	DOREPLIFETIME(APartner, CurState);
	DOREPLIFETIME(APartner, Target);
	DOREPLIFETIME(APartner, TargetPos);
	DOREPLIFETIME(APartner, bOrdered);
	DOREPLIFETIME(APartner, bUsingSkill);
	DOREPLIFETIME(APartner, bPosing);
	DOREPLIFETIME(APartner, bDied);
}

float APartner::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	if (bInvincible) return 0;

	if (DamageCauser != NULL && DamageCauser->IsA(ABullet::StaticClass()))
	{	//플레이어 공격 데미지 x
		return 0.f;
	}

	HP -= DamageAmount;
	Hunter->ServerPetHP(baseinstace->MyName, HP);

	if (GetHP() <= 0)
	{ //죽었을 때
		ServerPlayMontage(FName("Die"));
		bDied = true;
		SetGenericTeamId(1);
		HealPerSecondAmount = 0.f;
		AEnemy* DamageEnemy = Cast<AEnemy>(DamageCauser);
		if(DamageEnemy) DamageEnemy->LeaveTarget(this);
		return 0;
	}

	ServerPlayMontage( FName("Hit"));
	ServerStartInvincibility();

	return DamageAmount;
}

FGenericTeamId APartner::GetGenericTeamId() const
{
	return TeamID;
}


void APartner::MultiPlayMontage_Implementation(FName Section)
{
	PartnerAnim->PlayCombatMontage(Section);
}

bool APartner::CheckFalling()
{
	return GetMovementComponent()->IsFalling();
}

void APartner::SlashAttack()
{
	ServerPlayMontage(FName("SlashAttack"));
}

void APartner::SlashMove()
{
	if (Target)
	{
		FVector TargetLocation = Target->GetActorLocation();
		FVector Offset = GetActorLocation() - TargetLocation;
		FVector DirectionVec = TargetLocation - GetActorLocation();
		DirectionVec.Normalize();

		float SafeDistance = GetCapsuleComponent()->GetScaledCapsuleRadius() + Target->GetCapsuleComponent()->GetScaledCapsuleRadius();

		FVector StartLocation = GetActorLocation();
		FVector EndLocation = TargetLocation + DirectionVec * SafeDistance;
		FVector DesiredMovement = Offset + DirectionVec * SafeDistance;

		FHitResult HitResult;
		FRotator NewRotation = DirectionVec.Rotation();
		NewRotation.Pitch = 0;
		bool bHit = GetCapsuleComponent()->MoveComponent(DesiredMovement, NewRotation, true, &HitResult);

		if (bHit)
		{
			bHit = GetWorld()->LineTraceSingleByChannel(HitResult, EndLocation + FVector(0, 0, 100), EndLocation + FVector(0, 0, -1000), ECollisionChannel::ECC_Pawn);
			FVector NewLocation = HitResult.Location + FVector(0, 0, GetCapsuleComponent()->GetScaledCapsuleHalfHeight());
			if (bHit)
			{
				SetActorLocation(NewLocation);
				ServerApplyDamage(Target, SlashDamage, GetController(), this);

				//이펙트 출력
				UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), SlashHitEffect, TargetLocation);
				FVector MoveEffectSpawnLoc = StartLocation + DirectionVec * MoveEffectOffsetX;
				MoveEffectSpawnLoc.Y += 50.f;
				UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), SlashMoveEffect, MoveEffectSpawnLoc, DirectionVec.Rotation());
			}
			else
			{
				SetActorLocation(StartLocation);
			}
			//실패 구현 필요
		}

		else
		{
			SetActorLocation(EndLocation);
			ServerPlayMontage(FName("SlashAttack"));

		}
	}
}

void APartner::Howling()
{
	ServerPlayMontage(FName("Howling"));
}

void APartner::StopSkill()
{
	bOrdered = false;
	bUsingSkill = false;
	CurState = EPartnerState::MoveTarget;
	GetCharacterMovement()->MaxWalkSpeed = 600.f;
}

void APartner::UseSkill(ESkillID SkillID)
{
	
}

void APartner::SetTarget(ACharacter* setTarget)
{
	Target = setTarget;
}

void APartner::CancelOrder()
{
	bOrdered = false;
	PartnerAnim->StopAllMontages(0.5f);
	StopSkill();
}

void APartner::FollowHunter(AHunter* Master)
{
	Hunter = Master;
	bPosing = false;
	CurState = EPartnerState::MoveTarget;
}

void APartner::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	PartnerAnim->bPlaying = false;

	OnMontageEnd.Broadcast();

}

void APartner::MultiUseSkill_Implementation(ESkillID SkillID)
{
	UseSkill(SkillID);
}

void APartner::MultiSetHunter_Implementation(class AHunter* OwnerHunter)
{
	SetOwner(OwnerHunter);
	FollowHunter(OwnerHunter);
}

void APartner::MultiSetPosition_Implementation(const FVector& LocVec)
{
	SetPosition(LocVec);
}

void APartner::SetPosition(const FVector& LocVec)
{
	TargetPos = LocVec;
	bOrdered = true;
}

void APartner::ServerApplyDamage_Implementation(AActor* DamagedActor, float DamageAmount, AController* EventInstigator, AActor* DamageCauser)
{
	MultiApplyDamage(DamagedActor, DamageAmount, EventInstigator, DamageCauser);
}

void APartner::MultiApplyDamage_Implementation(AActor* DamagedActor, float DamageAmount, AController* EventInstigator, AActor* DamageCauser)
{
	UGameplayStatics::ApplyDamage(DamagedActor, DamageAmount, GetController(), DamageCauser, UDamageType::StaticClass());
}

void APartner::MultiCancelOrder_Implementation()
{
	CancelOrder();
}

void APartner::ServerStopMontage_Implementation(APartner* Partner)
{
	MultiStopMontage(Partner);
}

void APartner::MultiStopMontage_Implementation(APartner* Partner)
{
	PartnerAnim->StopAllMontages(0.2f);
}
