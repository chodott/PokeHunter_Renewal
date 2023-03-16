// Fill out your copyright notice in the Description page of Project Settings.


#include "WolfPartner.h"
#include "PartnerProjectile.h"
#include "PartnerAnimInstance.h"
#include "Components/CapsuleComponent.h"

AWolfPartner::AWolfPartner()
{
	PrimaryActorTick.bCanEverTick = true;

	//얼음창 클래스 설정
	static ConstructorHelpers::FClassFinder<APartnerProjectile>TempIceShardClass(TEXT("/Game/Partner/Blueprint/PartnerProjectile/BP_IceShard"));
	if (TempIceShardClass.Succeeded())
	{
		IceShardClass = TempIceShardClass.Class;
	}

	//스톰 범위 컴포넌트 추가
	StormCollision = CreateDefaultSubobject<UCapsuleComponent>(FName("StormCollision"));
	StormCollision->SetCapsuleHalfHeight(100.f);
	StormCollision->SetCapsuleRadius(150.f);
	StormCollision->Deactivate();
}


void AWolfPartner::UseSpecialSkill(ESkillID SkillID)
{
	if (bUsingSkill) return;
	bOrdered = true;
	bUsingSkill = true;

	switch (SkillID)
	{
	case ESkillID::IceShard:
		TargetPos = Target->GetActorLocation();
		CurState = EPartnerState::IceShard;
		break;

	case ESkillID::IceStorm:
		CurState = EPartnerState::MakingStorm;

	default:
		break;
	}
}

void AWolfPartner::LaunchIceShard()
{
	if (PartnerAnim)
	{
		PartnerAnim->PlayCombatMontage(TEXT("Attack"));
	}
}

void AWolfPartner::MakeIceShard()
{
	FVector InitialPos = GetMesh()->GetSocketLocation(FName("Head")) + GetActorForwardVector() * 300.f;
	FVector EndPos = Target->GetActorLocation();
	FVector DirectionVec = EndPos - GetActorLocation();
	DirectionVec.Normalize();


	auto IceShard = GetWorld()->SpawnActor<APartnerProjectile>(IceShardClass, InitialPos, DirectionVec.Rotation());
	IceShard->FireInDirection(DirectionVec, InitialPos, EndPos);
}

void AWolfPartner::MakeStorm()
{
	PartnerAnim->PlayCombatMontage(TEXT("Attack"));
	StormCollision->Activate(true);
}
