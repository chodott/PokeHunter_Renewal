// Fill out your copyright notice in the Description page of Project Settings.


#include "GolemBoss.h"
#include "Components/BoxComponent.h"

AGolemBoss::AGolemBoss()
{
	
	/*HeadCollisionBox->CreateDefaultSubobject<UBoxComponent>(FName("HeadCollisionBox"));
	HeadCollisionBox->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);

	BodyCollisionBox->CreateDefaultSubobject<UBoxComponent>(FName("BodyCollisionBox"));
	BodyCollisionBox->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);

	LeftArmCollisionBox->CreateDefaultSubobject<UBoxComponent>(FName("LeftArmCollisionBox"));
	LeftArmCollisionBox->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);

	RightArmCollisionBox->CreateDefaultSubobject<UBoxComponent>(FName("RightArmCollisionBox"));
	RightArmCollisionBox->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);

	LeftLegCollisionBox->CreateDefaultSubobject<UBoxComponent>(FName("LeftLegCollisionBox"));
	LeftLegCollisionBox->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);

	RightLegCollisionBox->CreateDefaultSubobject<UBoxComponent>(FName("RightLegCollisionBox"));
	RightLegCollisionBox->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);*/
}

void AGolemBoss::BeginPlay()
{
	Super::BeginPlay();
}
