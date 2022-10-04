// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "..\PokeHunter.h"
#include "GameFramework/Character.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "Hunter.generated.h"

UCLASS()
class POKEHUNTER_API AHunter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AHunter();

	//카메라
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
		class USpringArmComponent* CameraBoom;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
		class UCameraComponent* FollowCamera;

	//상호작용
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction")
		class ANpc* CurrentNpc;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:

	//인벤토리
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
		class UInventoryComponent* Inventory;

	//UI
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSubclassOf <UUserWidget> InventoryUIClass;
	class UUserWidget* InventoryUI;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSubclassOf <UUserWidget> StorageUIClass;
	class UUserWidget* StorageUI;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Partner")
	class APartner* Partner;


public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void PostInitializeComponents() override;
	virtual void PossessedBy(AController* NewController) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	UFUNCTION(BlueprintCallable)
	void RMBDown();
	void OpenInventory();

	UFUNCTION()
	void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

private:
	// Character Movement Input
	void MoveForward(float Val);
	void MoveRight(float Val);
	void LookUp(float NewAxisValue);
	void Turn(float NewAxisValue);
};
