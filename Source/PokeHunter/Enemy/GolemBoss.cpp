// Fill out your copyright notice in the Description page of Project Settings.


#include "GolemBoss.h"
#include "Components/BoxComponent.h"
#include "PokeHunter/Base/HitBoxComponent.h"
#include "PokeHunter/Item/Item.h"
#include "PokeHunter/Hunter/Hunter.h"
#include "EnemyAnimInstance.h"
#include "EnemyProjectile.h"


AGolemBoss::AGolemBoss()
{
	UHitBoxComponent* HeadHitBox = CreateDefaultSubobject<UHitBoxComponent>(FName("HeadHitBox"));
	HeadHitBox->SetupAttachment(GetMesh(), FName("Head"));
	HeadHitBox->SetCollisionProfileName(FName("EnemyHitBox"));
	PartHitBox.AddUnique(HeadHitBox);

	UHitBoxComponent* BodyHitBox = CreateDefaultSubobject<UHitBoxComponent>(FName("BodyHitBox"));
	BodyHitBox->SetupAttachment(GetMesh(), FName("Spine"));
	BodyHitBox->SetCollisionProfileName(FName("EnemyHitBox"));
	PartHitBox.AddUnique(BodyHitBox);

	UHitBoxComponent* RightArmHitBox = CreateDefaultSubobject<UHitBoxComponent>(FName("RightArmHitBox"));
	RightArmHitBox->SetupAttachment(GetMesh(), FName("RightArm"));
	RightArmHitBox->SetCollisionProfileName(FName("EnemyHitBox"));
	PartHitBox.AddUnique(RightArmHitBox);

	UHitBoxComponent* LeftArmHitBox = CreateDefaultSubobject<UHitBoxComponent>(FName("LeftArmHitBox"));
	LeftArmHitBox->SetupAttachment(GetMesh(), FName("LeftHand"));
	LeftArmHitBox->SetCollisionProfileName(FName("EnemyHitBox"));
	PartHitBox.AddUnique(LeftArmHitBox);

	UHitBoxComponent* RightLegHitBox = CreateDefaultSubobject<UHitBoxComponent>(FName("RightLegHitBox"));
	RightLegHitBox->SetupAttachment(GetMesh(), FName("RightLeg"));
	RightLegHitBox->SetCollisionProfileName(FName("EnemyHitBox"));
	PartHitBox.AddUnique(RightLegHitBox);

	UHitBoxComponent* LeftLegHitBox = CreateDefaultSubobject<UHitBoxComponent>(FName("LeftLegHitBox"));
	LeftLegHitBox->SetupAttachment(GetMesh(), FName("LeftLeg"));
	LeftLegHitBox->SetCollisionProfileName(FName("EnemyHitBox"));
	PartHitBox.AddUnique(LeftLegHitBox);


	AttackRange = 1000.f;

	//Earthquake Collision
	EarthquakeCollision = CreateDefaultSubobject<UStaticMeshComponent>(FName("EarthquakeCollision"));
	EarthquakeCollision->SetupAttachment(GetRootComponent());
	EarthquakeCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);



}

void AGolemBoss::Tick(float DeltaTime)
{
	APawn::Tick(DeltaTime);

	for (int i = 0; i < PartHitBox.Num()-1; ++i)
	{
		if (PartHitBox[i]->CheckBurning(DeltaTime))
		{
			float DamageAmount = 1;
			HP -= DamageAmount;
			OnDamage.Broadcast(DamageAmount, PartHitBox[i]->GetComponentLocation());
		}
	}
}

void AGolemBoss::BeginPlay()
{
	Super::BeginPlay();

	for (int i = 0; i < PartHitBox.Num() - 1; ++i)
	{
		PartHitBox[i]->OnComponentBeginOverlap.AddDynamic(this, &AGolemBoss::OnOverlapBegin);
		//PartHitBox[i]->OnComponentHit.AddDynamic(this, &AGolemBoss::OnHit);
		PartHitBox[i]->BurningTime = BurningTime;
	}
}

void AGolemBoss::LongAttack()
{
	if (EnemyAnim)
	{
		CurState = EEnemyState::LongAttack;
		MultiPlayMontage(this, FName("Throw"));
		
	}
	TargetPos = Target->GetActorLocation();
}

void AGolemBoss::LaunchStone()
{
	FVector InitialPos = GetMesh()->GetSocketLocation(FName("LeftHand"));
	if (Target == NULL) UE_LOG(LogTemp, Warning, TEXT("Dd"));
	FVector EndPos = Target->GetActorLocation();
	FVector DirectionVec = EndPos - InitialPos;
	DirectionVec.Normalize();


	AEnemyProjectile* Stone = GetWorld()->SpawnActor<AEnemyProjectile>(ProjectileClass, InitialPos, DirectionVec.Rotation());
	Stone->FireInDirection(DirectionVec, InitialPos, EndPos);
}

void AGolemBoss::InteractFire_Implementation(UPrimitiveComponent* HitComponent)
{
	auto* HitBox = Cast<UHitBoxComponent>(HitComponent);
	if (HitBox)
	{
		HitBox->bBurning = true;
		HitBox->StartBurningTime = GetWorld()->TimeSeconds;
	}
}

float AGolemBoss::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	APawn::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);



	FVector HitLoc;
	if (DamageEvent.IsOfType(FPointDamageEvent::ClassID))
	{
		const FPointDamageEvent& PointDamageEvent = static_cast<const FPointDamageEvent&>(DamageEvent);
		HitLoc = PointDamageEvent.HitInfo.Location;
		UHitBoxComponent* HitBox = Cast<UHitBoxComponent>(PointDamageEvent.HitInfo.GetComponent());
		HitBox->TakeDamage(DamageAmount);
		HP -= DamageAmount;
	}
	else
	{
		HP -= DamageAmount;
		HitLoc = GetActorLocation();
	}

	OnDamage.Broadcast(DamageAmount, HitLoc);

	return DamageAmount;
}

void AGolemBoss::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UHitBoxComponent* OverlapHitBox = Cast<UHitBoxComponent>(OverlappedComp);
	 
	if (OverlapHitBox)
	{
		if (bCanGrab)
		{
			FName PartName = OverlapHitBox->GetAttachSocketName();
			if (PartName == FName("LeftHand") || PartName == FName("RightHand"))
			{
				ACharacter* GrabbedCharacter = Cast<ACharacter>(OtherActor);
				if (GrabbedCharacter)
				{
					OtherActor->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("GrabSocket"));
					GrabbedTarget = GrabbedCharacter;
				}
				return;
			}
		}

		if (IEnemyInteractInterface* EnemyInteractInterface = Cast<IEnemyInteractInterface>(OtherActor))
		{
			EnemyInteractInterface->Execute_InteractAttack(OtherActor, OverlappedComp->GetComponentLocation());
		}

		UGameplayStatics::ApplyPointDamage(OtherActor, OverlapHitBox->Damage, SweepResult.Normal, SweepResult, GetController(), this, UDamageType::StaticClass());

	}
}

void AGolemBoss::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	UHitBoxComponent* OverlapHitBox = Cast<UHitBoxComponent>(HitComponent);
	if (OverlapHitBox)
	{
		UGameplayStatics::ApplyPointDamage(OtherActor, OverlapHitBox->Damage, Hit.ImpactNormal, Hit, GetController(), this, UDamageType::StaticClass());

	}
}

void AGolemBoss::Attack(int AttackPattern)
{
	switch (AttackPattern)
	{
	case 0:
		if (Target != NULL)
		{
			ServerPlayMontage(this, FName("Attack"));
			//EnemyAnim->PlayCombatMontage(TEXT("Attack"));
		}
		break;
	case 1:
		ServerPlayMontage(this, FName("Attack_Punch"));
		break;

	case 2:
		ServerPlayMontage(this, FName("Attack_Grab"));
		break;

	case 3:
		ServerPlayMontage(this, FName("Attack_Bind"));
		break;
	}
}

void AGolemBoss::Earthquake()
{
	FHitResult HitResult;
	FVector LocVec = GetActorLocation();
	FVector DownVec = (-1) * GetActorUpVector();
	float HalfHeight = GetCapsuleComponent()->GetScaledCapsuleHalfHeight();

	FVector StartTrace = LocVec + DownVec * HalfHeight;

		TArray<AActor*> OverlapActors;
		EarthquakeCollision->GetOverlappingActors(OverlapActors);
		for (auto OverlapActor : OverlapActors)
		{
			IEnemyInteractInterface* ApplyActor = Cast<IEnemyInteractInterface>(OverlapActor);
			if (ApplyActor)
			{
				Execute_InteractEarthquake(OverlapActor);

			}
		}
}