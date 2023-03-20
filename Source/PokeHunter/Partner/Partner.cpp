// Fill out your copyright notice in the Description page of Project Settings.


#include "Partner.h"
#include "PartnerController.h"
#include "PartnerAnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "PokeHunter/Hunter/Hunter.h"

// Sets default values
APartner::APartner()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	TeamID = FGenericTeamId(0);

	GetCharacterMovement()->bOrientRotationToMovement = true;

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

void APartner::ServerPlayMontage_Implementation(FName Section)
{
	MultiPlayMontage(Section);
}

void APartner::MultiPlayMontage_Implementation(FName Section)
{
	PartnerAnim->PlayCombatMontage(Section);
}

void APartner::Attack()
{
	if (Target != NULL)
	{
		//Check Target in Range
		GetCharacterMovement()->MaxWalkSpeed = 2000;
		AttackPoint = Target->GetActorLocation();
		auto AIController = Cast<AAIController>(GetController());
		AIController->MoveToLocation(AttackPoint);


		//Play Montage
		ServerPlayMontage(FName("Attack"));
		//PartnerAnim->PlayCombatMontage(TEXT("Attack"));
	}


	
}

void APartner::Howling()
{
	//Play Montage
	ServerPlayMontage(FName("Howling"));
	//PartnerAnim->PlayCombatMontage(TEXT("Howling"));
}

void APartner::StopSkill()
{
	bOrdered = false;
	bUsingSkill = false;
	CurState = EPartnerState::MoveTarget;
	GetCharacterMovement()->MaxWalkSpeed = 600.f;
}

void APartner::UseNormalSkill(ESkillID SkillID)
{
	//스킬 사용중 접근 불가 추가 필요

	if (bUsingSkill) return;
	bOrdered = true;
	bUsingSkill = true;

	switch (SkillID)
	{
	case ESkillID::Rush:

		CurState = EPartnerState::Rushing;

		break;

	case ESkillID::Howling:

		CurState = EPartnerState::Howling;

		break;

	default:
		break;
	}
}

void APartner::UseSpecialSkill(ESkillID SkillID)
{
	//스킬 사용중 접근 불가 추가 필요

	if (bUsingSkill) return;
	bOrdered = true;
	bUsingSkill = true;

	switch (SkillID)
	{
	case ESkillID::Rush:

		CurState = EPartnerState::Rushing;

		break;

	case ESkillID::Howling:

		CurState = EPartnerState::Howling;

		break;

	default:
		break;
	}
}

void APartner::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	PartnerAnim->bPlaying = false;

	OnMontageEnd.Broadcast();

}
