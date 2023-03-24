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
	Damage = 50.f;
}

void AExplosionTrap::OnExplosionOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->IsA<AEnemy>())
	{
		UGameplayStatics::ApplyDamage(OtherActor, Damage,NULL,this, UDamageType::StaticClass());
	}
}
