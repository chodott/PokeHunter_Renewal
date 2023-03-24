// Fill out your copyright notice in the Description page of Project Settings.


#include "Trap.h"
#include "Components/BoxComponent.h"
#include "PokeHunter/Enemy/Enemy.h"
#include "Bullet.h"

ATrap::ATrap()
{
	ItemType = EItemType::Trap;

	CollisionBox = CreateDefaultSubobject<UBoxComponent>(FName("CollisionBox"));
	CollisionBox->SetupAttachment(GetRootComponent());
	
	CollisionBox->OnComponentBeginOverlap.AddDynamic(this, &ATrap::OnBeginOverlap);
}

void ATrap::OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->IsA<AEnemy>() || OtherActor->IsA<ABullet>())
	{
		this->Destroy();
	}
}

