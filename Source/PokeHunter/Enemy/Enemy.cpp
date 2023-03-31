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

	if (CurState == EEnemyState::Binding)
	{
		float ElapsedTime = GetWorld()->TimeSeconds - StartBindingTime;
		float TimeLeft = BindingTime - ElapsedTime;
		if (TimeLeft <= 0.0f)
		{
			CurState = EEnemyState::Chase;
		}

	}

	//독 데미지 
	if (bPoisoned)
	{
		float ElapsedTime = GetWorld()->TimeSeconds - StartPoisonedTime;
		int CurSecond = FMath::FloorToInt(ElapsedTime);
		float TimeLeft = PoisonedTime - ElapsedTime;
		if (TimeLeft <= 0.0f)
		{
			bPoisoned = false;
			CurSecond = 0;
		}
		else
		{
			if (CurSecond == PoisonSaveTime)
			{
				//
			}
			else
			{
				PoisonSaveTime = CurSecond;
				FDamageEvent PoisonDamage;
				float DamageAmount = 1.f;
				TakeDamage(DamageAmount, PoisonDamage, NULL, NULL);

				//Damage UI Print Event
				OnDamage.Broadcast(DamageAmount, GetActorLocation());
			}
		}
	}
	//화상 데미지
	if (bBurning)
	{
		float ElapsedTime = GetWorld()->TimeSeconds - StartBurningTime;
		int CurSecond = FMath::FloorToInt(ElapsedTime);
		float TimeLeft = BurningTime - ElapsedTime;
		if (TimeLeft <= 0.0f)
		{
			bBurning = false;
			CurSecond = 0;
		}
		else
		{
			if (CurSecond == BurningSaveTime)
			{
				//
			}
			else
			{
				BurningSaveTime = CurSecond;
				FDamageEvent BurningDamage;
				float DamageAmount = 1.f;
				TakeDamage(DamageAmount, BurningDamage, NULL, NULL);

				//Damage UI Print Event
				OnDamage.Broadcast(DamageAmount, GetActorLocation());
			}
		}
	}
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

	//Hit Anim
	AItem* HitItem = Cast<AItem>(DamageCauser);
	if (HitItem)
	{
		if (HP <= 0 && CurState != EEnemyState::Die)
		{
			CurState = EEnemyState::Die;
			EnemyAnim->PlayCombatMontage(FName("Die"), true);
		}
		else
		{
			if (Target == NULL)
			{
				EnemyAnim->PlayCombatMontage(FName("Hit"), true);
				if (AHunter* Hunter = Cast<AHunter>(HitItem->ThisOwner))
				{
					Hunter->SetPartnerTarget(this);
				}
				Target = HitItem->ThisOwner;
				CurState = EEnemyState::Hit;
				bFirstHit = false;
			}
		}
		//HitItem->Destroy();
	}

	//Damage and UI
	FVector HitLoc;
	if (DamageEvent.IsOfType(FPointDamageEvent::ClassID))
	{
		const FPointDamageEvent& PointDamageEvent = static_cast<const FPointDamageEvent&>(DamageEvent);
		HitLoc = PointDamageEvent.HitInfo.Location;
	}
	else
	{
		HitLoc = GetActorLocation();
	}
	HP -= DamageAmount;
	OnDamage.Broadcast(DamageAmount, HitLoc);

	

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
	EnemyAnim->PlayCombatMontage(Section, true);
}

void AEnemy::ServerStartBinding_Implementation()
{
	MultiStartBinding_Implementation();
}

void AEnemy::MultiStartBinding_Implementation()
{
	CurState = EEnemyState::Binding;
	StartBindingTime = GetWorld()->TimeSeconds;
}

void AEnemy::StartBinding()
{
	ServerStartBinding();
}

void AEnemy::StartPoison()
{
	bPoisoned = true;
	StartPoisonedTime = GetWorld()->TimeSeconds;
}

void AEnemy::SeeNewTarget(AActor* Actor)
{
	TargetArray.AddUnique(Actor);
	
	
	if (Target == NULL)
	{
		SetTarget(Actor);
		CurState = EEnemyState::Chase;
	}
	
}

void AEnemy::HearSound(FVector SoundLoc, AActor* AgroActor)
{
	TargetPos = SoundLoc;
	AgroTarget = AgroActor;
	bWaitingAgro = true;
}

void AEnemy::Attack(int AttackPattern)
{
	if (Target != NULL)
	{
		CurState = EEnemyState::NormalAttack;
		ServerPlayMontage(this, FName("Attack"));
		//EnemyAnim->PlayCombatMontage(TEXT("Attack"));
	}
}

void AEnemy::LongAttack()
{
	if (EnemyAnim == NULL || Target == NULL) return;
	CurState = EEnemyState::LongAttack;
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
	CurState = EEnemyState::JumpAttack;
	float Distance = GetDistanceTo(Target);
	FVector LookVec = Target->GetActorLocation() - GetActorLocation();
	LookVec.Normalize();
	LookVec.Z = 0.3f;
	FVector Velocity = LookVec * Distance;
	GetCharacterMovement()->Launch(Velocity);
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

void AEnemy::InteractIce_Implementation()
{
	GetCharacterMovement()->MaxWalkSpeed /= 2;
}

void AEnemy::InteractFire_Implementation(UPrimitiveComponent* HitComponent)
{
	bBurning = true;
	StartBurningTime = GetWorld()->TimeSeconds;
}

void AEnemy::InteractBindTrap_Implementation()
{
	CurState = EEnemyState::Binding;
	StartBindingTime = GetWorld()->TimeSeconds;
}