// Fill out your copyright notice in the Description page of Project Settings.


#include "Partner.h"
#include "PartnerController.h"
#include "PartnerAnimInstance.h"
#include "Components/CapsuleComponent.h"
#include "PokeHunter/Hunter/Hunter.h"

// Sets default values
APartner::APartner()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void APartner::BeginPlay()
{
	Super::BeginPlay();

	PartnerAnim = Cast<UPartnerAnimInstance>(GetMesh()->GetAnimInstance());
	PartnerAnim->OnMontageEnded.AddDynamic(this, &APartner::OnMontageEnded);
}

// Called every frame
void APartner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void APartner::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void APartner::Attack()
{
	if (Target != NULL)
	{
		PartnerAnim->PlayCombatMontage(TEXT("Attack"));
	}

	
}

void APartner::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	PartnerAnim->bPlaying = false;

	OnMontageEnd.Broadcast();

	if (HP <= 0) this->Destroy();
}
