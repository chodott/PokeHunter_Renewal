// Fill out your copyright notice in the Description page of Project Settings.


#include "Bullet.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Components/PrimitiveComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "PokeHunter/Base/ItemInteractInterface.h"
#include "PokeHunter/Base/HitBoxComponent.h"
#include "PokeHunter/Hunter/Hunter.h"
#include "PokeHunter/Partner/Partner.h"
#include "PokeHunter/Enemy/Enemy.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"

ABullet::ABullet()
{

	//Collision
	StaticMesh->SetCollisionProfileName("Bullet");

	//Movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->InitialSpeed = 1000.f;

	ProjectileMovement->MaxSpeed = 3000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = true;

	
	if (GetLocalRole() == ROLE_Authority)
	{
		StaticMesh->OnComponentBeginOverlap.AddDynamic(this, &ABullet::OnOverlapBegin);
	}

	ItemType = EItemType::Bullet;

};

void ABullet::BeginPlay()
{
	Super::BeginPlay();

	bool bResult = FMath::RandBool();
	if (!bResult) Pitch = -1;
	bResult = FMath::RandBool();
	if (!bResult) Roll = -1;
}

void ABullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector RotVec = FVector(Roll * RotateSpeed, Pitch * RotateSpeed, 0.f);
	StaticMesh->AddLocalRotation(RotVec.Rotation(), false);
}

void ABullet::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComponent, 
													int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ServerSpawnEffect();
	//발사한 본인과의 충돌 무시
	if (OtherActor == ThisOwner) return;
	
	const FHitResult HitInfo = FHitResult(OtherActor, OtherComponent, StaticMesh->GetComponentLocation(),FVector::ZeroVector);
	if (OtherActor->Implements<UItemInteractInterface>())
	{	
		ApplyAbillity(OtherActor, OtherComponent);
	}
	
	ServerApplyDamage(OtherActor, Damage, GetActorForwardVector(), HitInfo, ThisOwner->GetController(), this, UDamageType::StaticClass());
	ServerDestroy();
}


void ABullet::UseItem(AHunter* ItemOwner, FVector InitialPos, FVector EndPos)
{
	FVector Velocity = FVector::ZeroVector;

	UGameplayStatics::SuggestProjectileVelocity(this, Velocity, InitialPos, EndPos, 
	ProjectileMovement->InitialSpeed, false, 0.f, GetWorld()->GetGravityZ(),ESuggestProjVelocityTraceOption::DoNotTrace);
	ThisOwner = ItemOwner;
	ProjectileMovement->Velocity = Velocity;
	ProjectileMovement->SetVelocityInLocalSpace(Velocity);
	SetLifeSpan(TimeLimit);

	ProjectileMovement->UpdateComponentVelocity();
	StaticMesh->AddImpulse(Velocity, FName(""),true);
}

void ABullet::MultiLaunchBullet_Implementation(AHunter* BulletOwner, FVector InitialPos, FVector EndPos)
{

	FVector Velocity = FVector::ZeroVector;
	ThisOwner = BulletOwner;
	bool bSuccess =
	UGameplayStatics::SuggestProjectileVelocity(this, Velocity, InitialPos, EndPos,
		ProjectileMovement->InitialSpeed, false, 0.f, GetWorld()->GetGravityZ(), ESuggestProjVelocityTraceOption::DoNotTrace);

	ProjectileMovement->Velocity = Velocity;
	ProjectileMovement->SetVelocityInLocalSpace(Velocity);

	SetLifeSpan(TimeLimit);

	ProjectileMovement->UpdateComponentVelocity();
	StaticMesh->AddImpulse(Velocity, FName(""), true);
}


void ABullet::ServerSpawnEmitter_Implementation(UParticleSystem* SpawnParticle, const FVector& SpawnLoc)
{
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SpawnParticle, SpawnLoc);
}

void ABullet::OnHitNotEnemy_Implementation(const FVector& HitVec)
{
	ServerDestroy();
}


void ABullet::ServerApplyDamage_Implementation(AActor* DamagedActor, int DamageAmount, FVector Direction, const FHitResult& HitInfo, AController* EventInstigator, AActor* DamageCauser, TSubclassOf<UDamageType> DamageTypeClass)
{
	MultiApplyDamage(DamagedActor, DamageAmount, Direction, HitInfo, EventInstigator, DamageCauser, DamageTypeClass);
}

void ABullet::MultiApplyDamage_Implementation(AActor* DamagedActor, int DamageAmount, FVector Direction, const FHitResult& HitInfo, AController* EventInstigator, AActor* DamageCauser, TSubclassOf<UDamageType> DamageTypeClass)
{
	FPointDamageEvent DamageEvent(DamageAmount, HitInfo, HitInfo.Normal, UDamageType::StaticClass());
	//DamagedActor->TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	UGameplayStatics::ApplyPointDamage(DamagedActor, DamageAmount, Direction, HitInfo, EventInstigator, DamageCauser, DamageTypeClass);
}

void ABullet::ServerSpawnEffect_Implementation()
{
	MultiSpawnEffect();
}

void ABullet::MultiSpawnEffect_Implementation()
{
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), HitEffect, StaticMesh->GetComponentLocation(), GetActorRotation());
}