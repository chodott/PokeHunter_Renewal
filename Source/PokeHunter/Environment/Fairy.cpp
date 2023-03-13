// Fill out your copyright notice in the Description page of Project Settings.


#include "Fairy.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Particles/ParticleSystem.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "Runtime/Engine/Classes/Kismet/KismetSystemLibrary.h"
#include "PokeHunter/Item/Item.h"

AFairy::AFairy()
{
	PrimaryActorTick.bCanEverTick = true;

	InteractionSphere->SetSphereRadius(10.f);
	InteractionSphere->SetCollisionProfileName(FName("OverlapAllDynamic"));
	InteractionSphere->OnComponentBeginOverlap.AddDynamic(this, &AFairy::OnHit);
}

void AFairy::Tick(float DeltaTime)
{
	//날아다녀야 할듯

}

void AFairy::Interact_Implementation(AHunter* Hunter)
{
	if (bCaught)
	{
		AItemDropActor::Interact_Implementation(Hunter);
	}
}

void AFairy::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& Hit)
{
	AItem* HitItem = Cast<AItem>(OtherActor);
	if (HitItem)
	{
		//포획 아이템과 충돌 시
		StaticMesh->SetStaticMesh(CaughtStaticMesh);
		InteractionSphere->SetSimulatePhysics(true);
		InteractionSphere->SetCollisionProfileName(FName("SmallItem"));
		if (CaughtParticle)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), CaughtParticle, GetActorTransform());
		}
		bCaught = true;
	}
}
