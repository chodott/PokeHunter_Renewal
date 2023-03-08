// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Partner.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnMontageEndDelegate);


UENUM(BlueprintType)
enum class EPartnerState : uint8
{
	Posing,
	Unselected,
};

UCLASS()
class POKEHUNTER_API APartner : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APartner();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	class AHunter* Hunter;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UPartnerAnimInstance* PartnerAnim;

	FOnMontageEndDelegate OnMontageEnd;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	UFUNCTION()
	virtual void Attack();
	inline void SetTarget(AActor* setTarget) { Target = setTarget;  };

	//AnimationFunction
	UFUNCTION()
	void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted);

public:
	EPartnerState CurState;
	FVector TargetPos;
	FVector LookTargetVec;
	AActor* Target;
	float HP{ 100 };
	bool bPosing;
	bool bOrdered;
};
