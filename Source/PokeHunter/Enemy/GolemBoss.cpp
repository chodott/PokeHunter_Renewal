// Fill out your copyright notice in the Description page of Project Settings.


#include "GolemBoss.h"
#include "Components/BoxComponent.h"
#include "PokeHunter/Base/HitBoxComponent.h"
#include "PokeHunter/Item/Item.h"
#include "PokeHunter/Hunter/Hunter.h"
#include "EnemyAnimInstance.h"
#include "EnemyProjectile.h"
#include "Kismet/GameplayStatics.h"

AGolemBoss::AGolemBoss()
{

	AttackRange = 1000.f;

	FName PartName = FName("HeadSocket");
	HeadHitBox = CreateDefaultSubobject<UHitBoxComponent>(FName("HeadHitBox"));
	HeadHitBox->SetupAttachment(GetMesh(), PartName);
	HeadHitBox->SetCollisionProfileName(FName("EnemyHitBox"));
	

	PartName = FName("Body");
	BodyHitBox = CreateDefaultSubobject<UHitBoxComponent>(FName("BodyHitBox"));
	BodyHitBox->SetupAttachment(GetMesh(), PartName);
	BodyHitBox->SetCollisionProfileName(FName("EnemyHitBox"));
	

	PartName = FName("RightHand");
	RightHandHitBox = CreateDefaultSubobject<UHitBoxComponent>(FName("RightHandHitBox"));
	RightHandHitBox->SetupAttachment(GetMesh(), PartName);
	RightHandHitBox->SetCollisionProfileName(FName("EnemyHitBox"));
	

	PartName = FName("RightArm");
	RightArmHitBox = CreateDefaultSubobject<UHitBoxComponent>(FName("RightArmHitBox"));
	RightArmHitBox->SetupAttachment(GetMesh(), PartName);
	RightArmHitBox->SetCollisionProfileName(FName("EnemyHitBox"));
	RightArmHitBox->SetChild(RightHandHitBox);
	

	PartName = FName("RightShoulder");
	RightShoulderHitBox = CreateDefaultSubobject<UHitBoxComponent>(FName("RightShoulderHitBox"));
	RightShoulderHitBox->SetupAttachment(GetMesh(), PartName);
	RightShoulderHitBox->SetCollisionProfileName(FName("EnemyHitBox"));
	RightShoulderHitBox->SetChild(RightArmHitBox);
	

	PartName = FName("LeftHand");
	LeftHandHitBox = CreateDefaultSubobject<UHitBoxComponent>(FName("LeftHandHitBox"));
	LeftHandHitBox->SetupAttachment(GetMesh(), PartName);
	LeftHandHitBox->SetCollisionProfileName(FName("EnemyHitBox"));
	

	PartName = FName("LeftArm");
	LeftArmHitBox = CreateDefaultSubobject<UHitBoxComponent>(FName("LeftArmHitBox"));
	LeftArmHitBox->SetupAttachment(GetMesh(), PartName);
	LeftArmHitBox->SetCollisionProfileName(FName("EnemyHitBox"));
	LeftArmHitBox->SetChild(LeftHandHitBox);
	

	PartName = FName("RightLeg");
	RightLegHitBox = CreateDefaultSubobject<UHitBoxComponent>(FName("RightLegHitBox"));
	RightLegHitBox->SetupAttachment(GetMesh(), PartName);
	RightLegHitBox->SetCollisionProfileName(FName("EnemyHitBox"));
	

	PartName = FName("LeftLeg");
	LeftLegHitBox = CreateDefaultSubobject<UHitBoxComponent>(FName("LeftLegHitBox"));
	LeftLegHitBox->SetupAttachment(GetMesh(), PartName);
	LeftLegHitBox->SetCollisionProfileName(FName("EnemyHitBox"));
	LeftLegHitBox->SetIsReplicated(true);


	//Earthquake Collision
	EarthquakeCollision = CreateDefaultSubobject<UStaticMeshComponent>(FName("EarthquakeCollision"));
	EarthquakeCollision->SetupAttachment(GetRootComponent());
	EarthquakeCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);



}

void AGolemBoss::Tick(float DeltaTime)
{
	APawn::Tick(DeltaTime);

	for (auto Hitbox : HitBoxMap)
	{
		if (Hitbox.Value)
		{
			if (Hitbox.Value->CheckBurning(DeltaTime))
			{
				float DamageAmount = 1;
				HP -= DamageAmount;
				OnDamage.Broadcast(DamageAmount, Hitbox.Value->GetComponentLocation());
			}
		}
	}
}

void AGolemBoss::BeginPlay()
{
	Super::BeginPlay();

	isDie = false;
}

void AGolemBoss::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	HitBoxMap.Add(FName("HeadSocket"), HeadHitBox);
	HitBoxMap.Add(FName("Body"), BodyHitBox);
	HitBoxMap.Add(FName("RightHand"), RightHandHitBox);
	HitBoxMap.Add(FName("RightArm"), RightArmHitBox);
	HitBoxMap.Add(FName("RightShoulder"), RightShoulderHitBox);
	HitBoxMap.Add(FName("LeftHand"), LeftHandHitBox);
	HitBoxMap.Add(FName("LeftArm"), LeftArmHitBox);
	HitBoxMap.Add(FName("RightLeg"), RightLegHitBox);
	HitBoxMap.Add(FName("LeftLeg"), LeftLegHitBox);

	//for (auto& Hitbox : HitBoxMap)
	//{
	//	Hitbox.Value->OnComponentBeginOverlap.AddDynamic(this, &AGolemBoss::OnOverlapBegin);
	//	// Hitbox.Value->BurningTime = BurningTime;
	//}
}

void AGolemBoss::Die()
{
	TArray<USceneComponent*> ChildComponents;
	GetMesh()->GetChildrenComponents(true, ChildComponents);
	for (auto Component : ChildComponents)
	{
		auto PrimitiveComponent = Cast<UPrimitiveComponent>(Component);
		PrimitiveComponent->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		PrimitiveComponent->SetSimulatePhysics(true);
		PrimitiveComponent->SetCollisionProfileName(FName("Destructible"));
		PrimitiveComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	}

	isDie = true;
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

void AGolemBoss::MultiApplyDamage_Implementation(AActor* OtherActor, float DamageAmount, FVector HitDirection, AActor* DamageCauser, const FHitResult& SweepResult)
{
	if (IEnemyInteractInterface* EnemyInteractInterface = Cast<IEnemyInteractInterface>(OtherActor))
	{
		
		EnemyInteractInterface->Execute_InteractAttack(OtherActor, HitDirection, DamageAmount);
	}

	UGameplayStatics::ApplyPointDamage(OtherActor, DamageAmount, SweepResult.Normal, SweepResult, NULL, DamageCauser, UDamageType::StaticClass());

}


void AGolemBoss::OpenLevelHome()
{
	FString LevelName = GetWorld()->GetName();
	if ("L_Field0" == LevelName && "MyHome" != LevelName) {
		FString levelName = L"/Game/Map/Lobby/MyHome";
		UGameplayStatics::OpenLevel(GetWorld(), *levelName);
	}
}


float AGolemBoss::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (HP <= 0) return 0;
	APawn::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	FVector HitLoc;
	if (DamageEvent.IsOfType(FPointDamageEvent::ClassID))
	{	//Point Damage

		const FPointDamageEvent& PointDamageEvent = static_cast<const FPointDamageEvent&>(DamageEvent);
		HitLoc = PointDamageEvent.HitInfo.Location;
		UHitBoxComponent* HitBox = Cast<UHitBoxComponent>(PointDamageEvent.HitInfo.GetComponent());
		if (HitBox)
		{
			if (HitBox->TakeDamage(DamageAmount))
			{
				FName PartName = HitBox->GetAttachSocketName();
				DestroyPart(PartName);
				HitBox->ServerDestroyPart();
				FString StringPartName = PartName.ToString();
				if (StringPartName.Contains("Left"))
				{
					CurState = EEnemyState::LeftDestroy;
					ServerPlayMontage(this, FName("LeftDestroy"));
				}
				else if(StringPartName.Contains("Right"))
				{
					CurState = EEnemyState::RightDestroy;
					ServerPlayMontage(this, FName("RightDestroy"));
				}
			}
		}

		AItem* HitItem = Cast<AItem>(DamageCauser);
		if (AHunter* Hunter = Cast<AHunter>(HitItem->ThisOwner))
		{
			Hunter->SetPartnerTarget(this);
		}
		HitItem->ServerDestroy();


		HP -= DamageAmount;
	}
	else
	{
		//�Ϲ� ������ ó��
		HP -= DamageAmount;
		HitLoc = GetActorLocation();
	}

	if (HP <= 0)
	{
		ServerPlayMontage(this, FName("Die"));
		bDied = true;
		SetActorTickEnabled(false);
		//Die();
	}

	if (bReflecting)
	{	//�ݰ� ���� ������ ����
		ReflectDamgeAmount += DamageAmount;
	}

	OnDamage.Broadcast(DamageAmount, HitLoc);

	return DamageAmount;
}

void AGolemBoss::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	/*UHitBoxComponent* OverlapHitBox = Cast<UHitBoxComponent>(OverlappedComp);
	FVector HitDirection;

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
					GrabbedCharacter->SetActorEnableCollision(false);
					GrabbedTarget = GrabbedCharacter;
				}
				return;
			}
		}

		HitDirection = OtherActor->GetActorLocation() - OverlappedComp->GetComponentLocation();
		HitDirection.Normalize();
		ServerApplyDamage(OtherActor, OverlapHitBox->Damage, HitDirection, this,  SweepResult);
		

	}*/
}

//void AGolemBoss::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
//{
//	UHitBoxComponent* HitBox = Cast<UHitBoxComponent>(HitComponent);
//	if (HitBox)
//	{
//		if (bCanGrab)
//		{
//			FName PartName = HitBox->GetAttachSocketName();
//			if (PartName == FName("LeftHand") || PartName == FName("RightHand"))
//			{
//				ACharacter* GrabbedCharacter = Cast<ACharacter>(OtherActor);
//				if (GrabbedCharacter)
//				{
//					OtherActor->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("GrabSocket"));
//					GrabbedCharacter->SetActorEnableCollision(false);
//					GrabbedTarget = GrabbedCharacter;
//				}
//				return;
//			}
//		}
//
//		ServerApplyDamage()
//}

void AGolemBoss::DestroyPart_Implementation(FName PartName)
{

}

void AGolemBoss::DeleteHitBox(FName PartName)
{
	HitBoxMap.Remove(PartName);
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
	case 4:
		ServerPlayMontage(this, FName("Block"));
		Block();
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