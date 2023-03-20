// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"
#include "EnemyAnimInstance.h"
#include "EnemyProjectile.h"
#include "Net/UnrealNetwork.h"
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
	//GetCapsuleComponent()->OnComponentHit.AddDynamic(this, &AEnemy::OnHit);

	//Set TargetArray
	for (int i = 0; i < 8; ++i)
	{
		TargetArray.AddDefaulted();
	}

	TeamID = FGenericTeamId(1);
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

void AEnemy::GetActorEyesViewPoint(FVector& OutLocation, FRotator& OutRotation) const
{
	OutLocation = GetMesh()->GetSocketLocation("HeadSocket");
	OutRotation = GetMesh()->GetSocketRotation("HeadSocket");
}

float AEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);



	AItem* HitItem = Cast<AItem>(DamageCauser);
	if (HitItem)
	{
		if (HP <= 0 && CurState != EEnemyState::Die)
		{
			CurState = EEnemyState::Die;
			EnemyAnim->PlayCombatMontage(FName("Die"));
		}
		else
		{
			if (Target == NULL)
			{
				EnemyAnim->PlayCombatMontage(FName("Hit"));
				if (AHunter* Hunter = Cast<AHunter>(HitItem->ThisOwner))
				{
					Hunter->SetPartnerTarget(this);
				}
				Target = HitItem->ThisOwner;
				CurState = EEnemyState::Hit;
				bFirstHit = false;
			}
		}
		HitItem->Destroy();
	}

	HP -= DamageAmount;

	return DamageAmount;
}

bool AEnemy::IsJumping()
{
	return GetCharacterMovement()->IsFalling();
}

void AEnemy::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AEnemy, CurState);
	DOREPLIFETIME(AEnemy, Target);

}

void AEnemy::ServerPlayMontage_Implementation(AEnemy* Enemy, FName Section)
{
	MultiPlayMontage(this, Section);
}


void AEnemy::MultiPlayMontage_Implementation(AEnemy* Enemy, FName Section)
{
	EnemyAnim->PlayCombatMontage(Section);
}

void AEnemy::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	AItem* HitItem = Cast<AItem>(OtherActor);
	if (HitItem)
	{
		if (EnemyAnim != NULL && HP > 0)
		{
			HP -= 5;
			if (HP <= 0 && CurState != EEnemyState::Die)
			{
				CurState = EEnemyState::Die;
				EnemyAnim->PlayCombatMontage(FName("Die"));
			}
			else
			{
				if (Target == NULL)
				{
					EnemyAnim->PlayCombatMontage(FName("Hit"));
					if (AHunter* Hunter = Cast<AHunter>(HitItem->ThisOwner))
					{
						Hunter->SetPartnerTarget(this);
					}
					Target = HitItem->ThisOwner;
					TargetPos = Target->GetActorLocation();
					CurState = EEnemyState::Hit;
					bFirstHit = false;
				}
			}
		}
		HitItem->Destroy();
	}
}

void AEnemy::SeeNewTarget(AActor* Actor)
{
	if (bFirstMeet)
	{
		bFirstMeet = false;
		//CurState = EEnemyState::Roar;
	}
}

void AEnemy::HearSound(FVector SoundLoc)
{
	TargetPos = SoundLoc;
	CurState = EEnemyState::Attention;
}

void AEnemy::Attack(int AttackPattern)
{
	if (Target != NULL)
	{
		ServerPlayMontage(this, FName("Attack"));
		//EnemyAnim->PlayCombatMontage(TEXT("Attack"));
	}
}

void AEnemy::LongAttack()
{
	if (EnemyAnim == NULL || Target == NULL) return;
	FVector InitialPos = GetMesh()->GetSocketLocation(FName("LeftHand")) + GetActorForwardVector() * 300.f;
	FVector EndPos = Target->GetActorLocation();
	FVector DirectionVec = EndPos - GetActorLocation();
}

void AEnemy::Roar()
{
	if (EnemyAnim == NULL) return;
	ServerPlayMontage(this, FName("Roar"));
	//EnemyAnim->PlayCombatMontage(TEXT("Roar"));
}

void AEnemy::Patrol()
{
	if (EnemyAnim == NULL) return;
	ServerPlayMontage(this, FName("Patrol"));
	//EnemyAnim->PlayCombatMontage(TEXT("Patrol"));
}

void AEnemy::JumpAttack()
{
	float Distance = GetDistanceTo(Target); 
	GetCharacterMovement()->Velocity = FVector(1000, 1000, 0);
	bool bJump = GetCharacterMovement()->DoJump(false);
	if (bJump)
	{
		GetCharacterMovement()->UpdateComponentVelocity();
	}
}

void AEnemy::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (!bInterrupted)
	{
		EnemyAnim->bPlaying = false;
		if (CurState == EEnemyState::Hit) CurState = EEnemyState::Roar;
		OnMontageEnd.Broadcast();
	}
}

