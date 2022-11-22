// Fill out your copyright notice in the Description page of Project Settings.


#include "HunterAnimInstance.h"
#include "Hunter.h"

UHunterAnimInstance::UHunterAnimInstance()
{
	//CombatMontage Init
	ConstructorHelpers::FObjectFinder<UAnimMontage> CombatMontageObj(TEXT("/Game/Hunter/Blueprint/CombatMontage.CombatMontage"));
	if(CombatMontageObj.Succeeded()) CombatMontage = CombatMontageObj.Object;
	MovementSpeed = 0.0f;

	//InteractMontage Init
	ConstructorHelpers::FObjectFinder<UAnimMontage> InteractMontageObj(TEXT("/Game/Hunter/Blueprint/InteractMontage.InteractMontage"));
	if (CombatMontageObj.Succeeded()) InteractMontage = InteractMontageObj.Object;
	
}

void UHunterAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	::UAnimInstance::NativeUpdateAnimation(DeltaSeconds);

	auto Pawn = TryGetPawnOwner();
	if (::IsValid(Pawn)) 
	{
		MovementSpeed = Pawn->GetVelocity().Size();
	}
}

void UHunterAnimInstance::NativeInitializeAnimation()
{
	::UAnimInstance::NativeInitializeAnimation();

	if (Hunter == nullptr)
	{
		Hunter = Cast<AHunter>(TryGetPawnOwner());
	}
}

void UHunterAnimInstance::UpdateAnimationProperties()
{
	if(Hunter != nullptr)
	{
		//Zoom
		bZoom = Hunter->bZoom;
		bRunning = Hunter->bRunning;
		
		FVector Speed = Hunter->GetVelocity();
		FVector XYspeed = FVector(Speed.X, Speed.Y, 0.f);
		MovementSpeed = XYspeed.Size();

		UInputComponent* Input = Hunter->InputComponent;
		Direction = CalculateDirection(Speed, Hunter->GetActorRotation());

		//Upper
		bUpperOnly = Hunter->bUpperOnly;
		
	}
}

void UHunterAnimInstance::PlayCombatMontage()
{
	if (CombatMontage) Montage_Play(CombatMontage,1.0f);
}

void UHunterAnimInstance::PlayInteractMontage(FName Section)
{
	Montage_Play(InteractMontage, 1.0f);
	Montage_JumpToSection(Section, InteractMontage);
}
