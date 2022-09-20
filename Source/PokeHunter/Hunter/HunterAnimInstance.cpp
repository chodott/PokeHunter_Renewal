// Fill out your copyright notice in the Description page of Project Settings.


#include "HunterAnimInstance.h"
#include "Hunter.h"

void UHunterAnimInstance::NativeInitializeAnimation()
{
	Super::UAnimInstance::NativeInitializeAnimation();

	if (Hunter == nullptr)
	{
		Hunter = Cast<AHunter>(TryGetPawnOwner());

	}
}

void UHunterAnimInstance::UpdateAnimationProperties()
{
	if(Hunter != nullptr)
	{
		FVector Speed = Hunter->GetVelocity();
		FVector XYspeed = FVector(Speed.X, Speed.Y, 0.f);
		MovementSpeed = XYspeed.Size();

		UInputComponent* Input = Hunter->InputComponent;
		Direction = CalculateDirection(Speed, Hunter->GetActorRotation());
	}
}