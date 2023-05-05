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

	AttackRange = 1500.f;

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

	for (auto& Hitbox : HitBoxArray)
	{
		if (Hitbox.HitBoxComponent)
		{
			if (Hitbox.HitBoxComponent->CheckBurning(DeltaTime))
			{
				float DamageAmount = 1;
				HP -= DamageAmount;
				OnDamage.Broadcast(DamageAmount, Hitbox.HitBoxComponent->GetComponentLocation());
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

	HitBoxArray.Reserve(9);

	HitBoxArray.Add(FHitBoxInfo(FName("HeadSocket"), HeadHitBox));
	HitBoxArray.Add(FHitBoxInfo(FName("Body"), BodyHitBox));
	HitBoxArray.Add(FHitBoxInfo(FName("RightHand"), RightHandHitBox));
			
	HitBoxArray.Add(FHitBoxInfo(FName("RightArm"), RightArmHitBox));
	HitBoxArray.Add(FHitBoxInfo(FName("RightShoulder"), RightShoulderHitBox));
	HitBoxArray.Add(FHitBoxInfo(FName("LeftHand"), LeftHandHitBox));
				
	HitBoxArray.Add(FHitBoxInfo(FName("LeftArm"), LeftArmHitBox));
	HitBoxArray.Add(FHitBoxInfo(FName("RightLeg"), RightLegHitBox));
	HitBoxArray.Add(FHitBoxInfo(FName("LeftLeg"), LeftLegHitBox));

	for (auto& Hitbox : HitBoxArray)
	{
		Hitbox.HitBoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AGolemBoss::OnOverlapBegin);
		Hitbox.HitBoxComponent->BurningTime = BurningTime;
	}
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
}

void AGolemBoss::LongAttack()
{
	if (EnemyAnim)
	{
		CurState = EEnemyState::LongAttack;
		ServerPlayMontage(this, FName("Throw"));
		
	}
	TargetPos = Target->GetActorLocation();
}

void AGolemBoss::LaunchStone()
{
	FVector InitialPos = GetMesh()->GetSocketLocation(FName("LeftHand"));
	FVector EndPos = Target->GetActorLocation();
	FVector DirectionVec = EndPos - InitialPos;
	DirectionVec.Normalize();

	ServerSpawnProjectile(ProjectileClass, InitialPos, EndPos, DirectionVec);
}

void AGolemBoss::SpawnBombs()
{
	FVector ForwardVec = GetActorForwardVector();
	FVector UpVec = GetActorUpVector();
	float Gap = 360.f / MaxBombCnt;
	for (int i = 0; i < MaxBombCnt; ++i)
	{
		FRandomStream RandomStream(FMath::Rand());
		float MinAngle = i * Gap;
		float MaxAngle = MinAngle + Gap;
		float RandomAngle = RandomStream.FRandRange(MinAngle, MaxAngle);
		float RandomDistance = RandomStream.FRandRange(GetCapsuleComponent()->GetScaledCapsuleRadius() + 100, MaxBombRange);
		FVector RandomPos = ForwardVec.RotateAngleAxis(RandomAngle, UpVec) * RandomDistance + GetActorLocation();
		
		int RandClassNum = RandomStream.FRandRange(0, BombClassArray.Num());
		AEnemyProjectile* NewBomb = GetWorld()->SpawnActor<AEnemyProjectile>(BombClassArray[RandClassNum], RandomPos, FRotator::ZeroRotator);

		FHitResult HitResult;
		FVector TraceStart = RandomPos;
		FVector TraceEnd = RandomPos - FVector(0.f, 0.f, 500.f);
		FCollisionQueryParams TraceParams;
		TraceParams.bTraceComplex = false;
		TraceParams.bReturnPhysicalMaterial = false;
		if (GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECollisionChannel::ECC_WorldStatic, TraceParams))
		{
			FVector SpawnLocation = HitResult.ImpactPoint;
			NewBomb->SetActorLocation(SpawnLocation);
			NewBomb->ThisOwner = this;
		}
		
		FVector DirectionVec = RandomPos - GetActorLocation();
		DirectionVec.Normalize();
		ServerSpawnProjectile(BombClassArray[RandClassNum], RandomPos, FVector(0,0,0), DirectionVec);
		BombArray.Add(NewBomb);
	}
}

void AGolemBoss::SpawnCupcake()
{
	FVector SpawnLoc = Target->GetActorLocation();
	FVector EndTrace = SpawnLoc - FVector(0, 0, 200);
	FHitResult HitResult;
	bool bHit = GetWorld()->LineTraceSingleByProfile(HitResult, SpawnLoc, EndTrace, FName("bCanStand"));
	if (bHit)
	{
		SpawnLoc = EndTrace - FVector(0, 0, 500);
		AActor* ASpawnedActor = GetWorld()->SpawnActor<AActor>(CupcakeClass, SpawnLoc, FRotator::ZeroRotator);
	}
}

void AGolemBoss::CheckWideAttack()
{
	FVector CenterVec = GetActorLocation();
	float MinRadius = (WideAttackCnt + 1) * WideAttackGap - WideAttackRadiusGap;
	float MaxRadius = (WideAttackCnt + 1) * WideAttackGap + WideAttackRadiusGap;

	FCollisionShape CapsuleShape = FCollisionShape::MakeCapsule(MaxRadius, 1000.f);
	TArray<FHitResult> HitResults;

	bool bHit = GetWorld()->SweepMultiByProfile(HitResults, CenterVec, CenterVec, FQuat::Identity, FName("EnemyHitBox"), CapsuleShape);
	if (bHit)
	{
		for (const FHitResult& HitResult : HitResults)
		{
			float Distance = FVector::Dist2D(HitResult.ImpactPoint, GetActorLocation());
			if (Distance > MinRadius)
			{
				AActor* HitActor = HitResult.GetActor();
				if (HitActor->Implements<UEnemyInteractInterface>())
				{
					IEnemyInteractInterface::Execute_InteractWideAttack(HitActor, 50.f);
				}
			}
		}
	}

	WideAttackCnt++;
	if (WideAttackCnt == 3) WideAttackCnt = 0;
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

void AGolemBoss::MultiApplyPointDamage_Implementation(AActor* OtherActor, float DamageAmount, FVector HitDirection, AActor* DamageCauser, const FHitResult& SweepResult)
{
	if (IEnemyInteractInterface* EnemyInteractInterface = Cast<IEnemyInteractInterface>(OtherActor))
	{
		
		EnemyInteractInterface->Execute_InteractAttack(OtherActor, HitDirection, DamageAmount);
	}

	UGameplayStatics::ApplyPointDamage(OtherActor, DamageAmount, SweepResult.Normal, SweepResult, NULL, DamageCauser, UDamageType::StaticClass());

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
					ServerPlayMontage(this, FName("LeftDestroy"));
					CurState = EEnemyState::LeftDestroy;
				}
				else if(StringPartName.Contains("Right"))
				{
					ServerPlayMontage(this, FName("RightDestroy"));
					CurState = EEnemyState::RightDestroy;
				}
			}
		}

		AItem* HitItem = Cast<AItem>(DamageCauser);
		if (!HitItem) return 0;
		if (AHunter* Hunter = Cast<AHunter>(HitItem->ThisOwner))
		{
			Hunter->SetPartnerTarget(this);
		}


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
	UHitBoxComponent* OverlapHitBox = Cast<UHitBoxComponent>(OverlappedComp);
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
					GrabbedTargets.Add(GrabbedCharacter);
				}
				return;
			}
		}

		HitDirection = OtherActor->GetActorLocation() - OverlappedComp->GetComponentLocation();
		HitDirection.Normalize();
		ServerApplyPointDamage(OtherActor, OverlapHitBox->Damage, HitDirection, this,  SweepResult);
		

	}
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

void AGolemBoss::DestroyPart_Implementation(const FName& PartName)
{

}

void AGolemBoss::DeleteHitBox(const FName& PartName)
{
	// Delete Hit Box를 제거함.
	//오류 계속 발생하면 제거 필요
	int TargetIndex = -1;
	for (int i = 0; i < HitBoxArray.Num(); ++i)
	{
		if (HitBoxArray[i].HitBoxName == PartName)
		{
			TargetIndex = i;
			break;
		}
	}


	if(TargetIndex >= 0) HitBoxArray.RemoveAt(TargetIndex);

}

int AGolemBoss::CheckInRange()
{
	float Distance = FVector::Dist2D(Target->GetTargetLocation(), GetActorLocation());
	for (int i = 0; i < NormalAttackRange.Num(); ++i)
	{
		if (NormalAttackRange[i] >= Distance) return i;
	}
	return 0;
}

int AGolemBoss::CheckPattern()
{
	float CurTime = GetWorld()->GetTimeSeconds();

	for (auto& Pattern : PatternManageArray){
		Pattern.CheckReady(CurTime);
		if (Pattern.GetReady())
		{
			return Pattern.num;
		}
	}
	return 0;
}

void AGolemBoss::Attack(int AttackPattern)
{
	//LongAttack();

	switch (AttackPattern)
	{

	case 0:
		ServerPlayMontage(this, FName("Attack_Grab"));
		
		break;
	case 1:
		ServerPlayMontage(this, FName("Attack_Punch"));
		break;

	case 2:
		ServerPlayMontage(this, FName("Attack_Bind"));
		break;

	case 3:
		ServerPlayMontage(this, FName("Attack"));
		break;
	case 4:
		ServerPlayMontage(this, FName("Block"));
		Block();
		break;

	case 5:
		WideAttack();
		break;
	}
}

void AGolemBoss::PatternAttack(int AttackPattern)
{
	ServerPlayMontage(this, FName("ChargeAttack"));

	return; 
	switch (AttackPattern)
	{

	case 0:
		ServerPlayMontage(this, FName("Throw"));
		break;
	case 1:
		ServerPlayMontage(this, FName("WideAttack"));
		break;

	case 2:
		break;

	case 3:
		ServerPlayMontage(this, FName("JumpAttack"));
		break;
	}

	PatternManageArray[AttackPattern].UseSkill(GetWorld()->GetTimeSeconds());
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
