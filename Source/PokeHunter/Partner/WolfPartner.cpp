// Fill out your copyright notice in the Description page of Project Settings.


#include "WolfPartner.h"
#include "PartnerProjectile.h"
#include "PartnerAnimInstance.h"
#include "Components/StaticMeshComponent.h"
#include "PokeHunter/Enemy/Enemy.h"
#include "Kismet/GameplayStatics.h"

AWolfPartner::AWolfPartner()
{
	PrimaryActorTick.bCanEverTick = true;

	//얼음창 클래스 설정
	static ConstructorHelpers::FClassFinder<APartnerProjectile>TempIceShardClass(TEXT("/Game/Partner/Blueprint/PartnerProjectile/BP_IceShard"));
	if (TempIceShardClass.Succeeded())
	{
		IceShardClass = TempIceShardClass.Class;
	}

	//스킬 범위 컴포넌트
	StormCollision = CreateDefaultSubobject<UStaticMeshComponent>(FName("StormCollision"));
	StormCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	BreathCollision = CreateDefaultSubobject<UStaticMeshComponent>(FName("BreathCollision"));
	BreathCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	BreathCollision->SetupAttachment(GetMesh(), FName("HeadSocket"));
	BreathCollision->SetVisibility(false);

}


void AWolfPartner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	if (bBreathe)
	{
		TArray<AActor*> OverlapActors;
		BreathCollision->GetOverlappingActors(OverlapActors, AEnemy::StaticClass());
		for (auto Enemy : OverlapActors)
		{
			UGameplayStatics::ApplyDamage(Enemy, BreathDamage, GetController(), this, UDamageType::StaticClass());
		}

		float ElapsedTime = GetWorld()->TimeSeconds - BreathStartTime;
		if (ElapsedTime > BreathTime)
		{
			bBreathe = false;
			BreathCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			BreathCollision->SetVisibility(false);
		}
		
	}

	if (bOnStorm)
	{
		TArray<AActor*> OverlapActors;
		BreathCollision->GetOverlappingActors(OverlapActors, AEnemy::StaticClass());
		for (auto Enemy : OverlapActors)
		{
			UGameplayStatics::ApplyDamage(Enemy, StormDamage, GetController(), this, UDamageType::StaticClass());
		}
	}
}

void AWolfPartner::BeginPlay()
{
	Super::BeginPlay();

	//StormCollision->OnComponentBeginOverlap.AddDynamic(this, &AWolfPartner::IntoStorm);
	//StormCollision->OnComponentEndOverlap.AddDynamic(this, &AWolfPartner::OutStorm);
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
		break;

	case ESkillID::IceBreath:
		CurState = EPartnerState::IceBreath;
		break;
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

void AWolfPartner::IceBreathe()
{
	BreathStartTime = GetWorld()->GetTimeSeconds();
	bBreathe = true;
	BreathCollision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	BreathCollision->SetVisibility(true);
	PartnerAnim->PlayCombatMontage(TEXT("Attack"));
}

void AWolfPartner::MakeIceShard()
{
	FVector InitialPos = GetMesh()->GetSocketLocation(FName("Head")) + GetActorForwardVector() * 300.f;
	FVector EndPos = Target->GetActorLocation();
	FVector DirectionVec = EndPos - GetActorLocation();
	DirectionVec.Normalize();

	ServerSpawnProjectile(this, IceShardClass, InitialPos, EndPos);
}

void AWolfPartner::MakeStorm()
{
	PartnerAnim->PlayCombatMontage(TEXT("Attack"));
	//StormCollision.set
}

void AWolfPartner::IntoStorm(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	
}

void AWolfPartner::OutStorm(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

}
