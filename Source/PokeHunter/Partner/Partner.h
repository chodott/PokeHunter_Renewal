// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Partner.generated.h"

UCLASS()
class POKEHUNTER_API APartner : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APartner();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	class AHunter* Hunter;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
	FVector TargetPos;
	bool bPosing;
	bool bOrdered;
};
