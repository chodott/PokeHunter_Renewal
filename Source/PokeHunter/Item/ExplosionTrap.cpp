// Fill out your copyright notice in the Description page of Project Settings.


#include "ExplosionTrap.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PokeHunter/Enemy/Enemy.h"

AExplosionTrap::AExplosionTrap()
{
	ExplosionCollision = CreateDefaultSubobject<USphereComponent>(FName("ExplosionCollision"));
	ExplosionCollision->SetupAttachment(GetRootComponent());
	ExplosionCollision->OnComponentBeginOverlap.AddDynamic(this, &AExplosionTrap::OnExplosionOverlap);
	ExplosionCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	Damage = 50.f;
}

void AExplosionTrap::OnExplosionOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->IsA<AEnemy>())
	{
		ServerApplyDamage(OtherActor, Damage, NULL, this, UDamageType::StaticClass());
	}
}

void AExplosionTrap::ServerApplyDamage_Implementation(AActor* DamagedActor, int DamageAmount, class AController* ItemOwner, AActor* DamageCauser, TSubclassOf<UDamageType> DamageTypeClass)
{
	MultiApplyDamage(DamagedActor, DamageAmount, ItemOwner, DamageCauser, DamageTypeClass);
}

void AExplosionTrap::MultiApplyDamage_Implementation(AActor* DamagedActor, int DamageAmount, class AController* ItemOwner, AActor* DamageCauser, TSubclassOf<UDamageType> DamageTypeClass)
{
	UGameplayStatics::ApplyDamage(DamagedActor, DamageAmount,	ItemOwner, DamageCauser, DamageTypeClass);
}

void AExplosionTrap::OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ExplosionCollision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	/*if (OtherActor->IsA<AEnemy>() || OtherActor->IsA<ABullet>())
	{
		this->Destroy();
	}*/
}