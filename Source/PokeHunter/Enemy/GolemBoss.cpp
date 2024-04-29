// Fill out your copyright notice in the Description page of Project Settings.


#include "GolemBoss.h"
#include "Components/BoxComponent.h"
#include "Net/UnrealNetwork.h"
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

	for (auto& HitBox : HitBoxMap)
	{
		UHitBoxComponent* HitBoxComponent = HitBox.Value;
		if (true == HitBoxComponent->CheckBurning(DeltaTime))
		{
			float DamageAmount = 1;
			HP -= DamageAmount;
			OnDamage.Broadcast(DamageAmount, HitBoxComponent->GetComponentLocation());
		}
	}

	if (GetLocalRole() == ROLE_Authority)
	{
		if (Target != NULL && !EnemyAnim->bPlaying)
		{
			float CurTime = GetWorld()->GetTimeSeconds();
			if (TargetFocusTime < CurTime - FocusStartTime)
			{
				ChangeTarget();
				FocusStartTime = CurTime;
			}
		}
	}

}

void AGolemBoss::BeginPlay()
{
	Super::BeginPlay();
}

void AGolemBoss::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	HitBoxMap.Add("HeadSocket", HeadHitBox);
	HitBoxMap.Add("Body", BodyHitBox);
	HitBoxMap.Add("RightHand", RightHandHitBox);
	HitBoxMap.Add("RightArm", RightArmHitBox);
	HitBoxMap.Add("RightShoulder", RightShoulderHitBox);
	HitBoxMap.Add("LeftHand", LeftHandHitBox);
	HitBoxMap.Add("LeftArm", LeftArmHitBox);
	HitBoxMap.Add("RightLeg", RightLegHitBox);
	HitBoxMap.Add("LeftLeg", LeftLegHitBox);

	for (auto& HitBox : HitBoxMap)
	{
		auto& HitBoxComponent = HitBox.Value;
		HitBoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AGolemBoss::OnOverlapBegin);
		HitBoxComponent->BurningTime = BurningTime;
	}
}

void AGolemBoss::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AGolemBoss, BombArray);
}

void AGolemBoss::Die()
{
	TArray<USceneComponent*> ChildComponents;
	GetMesh()->GetChildrenComponents(true, ChildComponents);

	for (auto Component : ChildComponents)
	{
		auto PrimitiveComponent = Cast<UPrimitiveComponent>(Component);
		if (PrimitiveComponent->IsA<UHitBoxComponent>())
		{
			PrimitiveComponent->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
			PrimitiveComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
		else
		{
			PrimitiveComponent->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
			PrimitiveComponent->SetSimulatePhysics(true);
			PrimitiveComponent->SetCollisionProfileName(FName("Destructible"));
			PrimitiveComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		}
		
	}
}

void AGolemBoss::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	Super::OnMontageEnded(Montage, bInterrupted);
}

void AGolemBoss::SetTarget(AActor* NewTarget)
{
	Super::SetTarget(NewTarget);
	FocusStartTime = GetWorld()->GetTimeSeconds();
}

void AGolemBoss::LongAttack()
{
	if (EnemyAnim)
	{
		CurState = EEnemyState::LongAttack;
		ServerPlayMontage(FName("Throw"));
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

		FVector DirectionVec = RandomPos - GetActorLocation();
		DirectionVec.Normalize();
		ServerSpawnBomb(this, BombClassArray[RandClassNum], RandomPos, FRotator::ZeroRotator);
	}

}

void AGolemBoss::LaunchBombs()
{
	int CloseStoneNum = 0;
	int MinDistance = 99999;

	FVector TargetLoc;
	if (nullptr == Target) TargetLoc = { 0, 0, 0 };
	else TargetLoc = Target->GetActorLocation();

	FVector GolemLoc = GetActorLocation();
	float GolemTargetDistance = FVector::Dist2D(TargetLoc, GolemLoc);
	int BombLength = BombArray.Num();
	for (int i = 0; i < BombLength; ++i)
	{
		if (BombArray[i])
		{
			float BombGolemDistance = FVector::Dist2D(BombArray[i]->GetActorLocation(), GolemLoc);
			FVector BombLoc = BombArray[i]->GetActorLocation();
			FVector BombDirVec = BombLoc - GolemLoc;
			BombDirVec.Z = 0;
			BombDirVec.Normalize();
			FVector PlaneGolemLoc = GolemLoc;
			PlaneGolemLoc.Z = TargetLoc.Z;
			FVector BombTargetPos = PlaneGolemLoc + GolemTargetDistance * BombDirVec;
			FVector LaunchDirection = BombTargetPos - BombLoc;
			LaunchDirection.Normalize();
			BombArray[i]->FireInDirection(LaunchDirection);
		}
	}
	BombArray.Reset();
}

void AGolemBoss::DropBombs()
{
	if (!BombArray.IsEmpty())
	{
		for (auto& Bomb : BombArray)
		{
			Bomb->StaticMesh->SetSimulatePhysics(true);
		}
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

	//FCollisionShape CapsuleShape = FCollisionShape::MakeCapsule(MaxRadius, 1000.f);

	TArray<FHitResult> HitResults;

	TArray<AActor*> OverlappedActors;
	EarthquakeCollision->GetOverlappingActors(OverlappedActors, ACharacter::StaticClass());
	for (auto* actor : OverlappedActors)
	{
		ServerApplyDamage(actor, 10.f, nullptr, nullptr, UDamageType::StaticClass());
	}


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

void AGolemBoss::ServerSpawnBomb_Implementation(class AGolemBoss* Golem, TSubclassOf<class AEnemyProjectile>SpawnClass, const FVector& SpawnLoc, const FRotator& Rotation)
{
	AEnemyProjectile* SpawnBomb = GetWorld()->SpawnActor<AEnemyProjectile>(SpawnClass, SpawnLoc, Rotation);
	Golem->BombArray.Add(SpawnBomb);
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

	FVector HitLoc;	//데미지 출력 위치
	if (DamageEvent.IsOfType(FPointDamageEvent::ClassID))
	{	//탄환 데미지
		const FPointDamageEvent& PointDamageEvent = static_cast<const FPointDamageEvent&>(DamageEvent);
		HitLoc = PointDamageEvent.HitInfo.Location;

		//부위 별 히트박스 데미지 적용
		UHitBoxComponent* HitBox = Cast<UHitBoxComponent>(PointDamageEvent.HitInfo.GetComponent());
		HitBox->TakeDamage(DamageAmount);
		AItem* HitItem = Cast<AItem>(DamageCauser);
		if (!HitItem) return 0;
		HitItem->AnnounceTarget(this);

	}
	else
	{	//부위 타격이 아닐 때 데미지 출력 위치
		HitLoc = GetActorLocation();
	}
	HP -= DamageAmount;
	//반사 공격 데미지 축적
	if (bReflecting) ReflectDamgeAmount += DamageAmount;
	//Die
	if (HP <= 0) fallDown();
	//데미지 출력 이벤트 호출
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
					GrabbedTargets.AddUnique(GrabbedCharacter);
				}
				return;
			}
		}

		HitDirection = OtherActor->GetActorLocation() - OverlappedComp->GetComponentLocation();
		HitDirection.Normalize();
		ServerApplyPointDamage(OtherActor, OverlapHitBox->Damage, HitDirection, this,  SweepResult);
		

	}
}

void AGolemBoss::DestroyPart_Implementation(const FName& PartName)
{

}

void AGolemBoss::DeleteHitBox(const FName& PartName)
{
	HitBoxMap.Remove(PartName);
}

void AGolemBoss::DestroyLeftArm()
{
	ServerPlayMontage(FName("LeftDestroy"));
	CurState = EEnemyState::LeftDestroy;
	DropBombs();
}

void AGolemBoss::DestroyRightArm()
{
	ServerPlayMontage(FName("RightDestroy"));
	CurState = EEnemyState::RightDestroy;
	DropBombs();
}

void AGolemBoss::fallDown()
{
	ServerPlayMontage(FName("Die"));
	bDied = true;
	SetActorTickEnabled(false);
	DropBombs();
}

int AGolemBoss::CheckInRange()
{
	//if (Target)
	//{
	//	float Distance = FVector::Dist2D(Target->GetTargetLocation(), GetActorLocation());
	//	for (int i = 0; i < NormalAttackRange.Num(); ++i)
	//	{
	//		if (NormalAttackRange[i] >= Distance) return i;
	//	}
	//}
	//int PatternNum = FMath::RandRange(0, 3);

	//수정필
	return 0;
}

int AGolemBoss::CheckPattern()
{
	float CurTime = GetWorld()->GetTimeSeconds();

	int PatternNum = FMath::RandRange(0,4);


	return PatternNum;
}

void AGolemBoss::Attack(int AttackPattern)
{
	AttackPattern = FMath::RandRange(0, 4);

	switch (AttackPattern)
	{
	case 0:
		if (!bLoseLeftHand && !bLoseRightHand)
		{
			ServerPlayMontage(FName("Attack_Grab"));
		}
		else ServerPlayMontage(FName("WideAttack"));
		break;
	case 1:
		if (bLoseLeftHand)
		{
			if (bLoseRightHand) ServerPlayMontage(FName("WideAttack"));
			else ServerPlayMontage( FName("Attack"));
		}
		else ServerPlayMontage( FName("Attack_Punch"));
		break;

	case 2:
		ServerPlayMontage( FName("Attack"));
		break;

	case 3:
		if (bLoseRightHand)
		{
			if (bLoseLeftHand)  ServerPlayMontage(FName("WideAttack"));
			else ServerPlayMontage(FName("Attack_Punch"));
		}
		else ServerPlayMontage(FName("Attack"));
		break;
	case 5:
		ServerPlayMontage(FName("Block"));
		Block();
		break;

	case 4:
		WideAttack();
		break;
	}
}

void AGolemBoss::PatternAttack(int AttackPattern)
{

	switch (AttackPattern)
	{

	case 0:
		if (!bLoseLeftHand) ServerPlayMontage(FName("Throw"));
		else ServerPlayMontage(FName("ChargeAttack"));

		ServerPlayMontage(FName("ChargeAttack"));
		break;
	case 1:
		ServerPlayMontage(FName("WideAttack"));
		break;

	case 2:
		ServerPlayMontage(FName("ChargeAttack"));
		break;

	case 3:
		ServerPlayMontage(FName("JumpAttack"));
		break;

	case 4:
		LongAttack();
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
			if (ApplyActor == NULL) continue;
			Execute_InteractEarthquake(OverlapActor);
		}
}
