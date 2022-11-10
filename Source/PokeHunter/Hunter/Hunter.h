// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "..\PokeHunter.h"
#include "GameFramework/Character.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "Hunter.generated.h"

//Dynamic 
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDynamicDele);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDynamicDeleParam, float, val);

UCLASS()
class POKEHUNTER_API AHunter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AHunter();

	//ī�޶�
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
		class USpringArmComponent* CameraBoom;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
		class UCameraComponent* FollowCamera;
	//�κ��丮
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
		class UInventoryComponent* Inventory;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:

	//��ȣ�ۿ�
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction")
	class AInteractActor* InteractingActor;

	//�� ����
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "QuickSlot")
	TArray<class UItemData*> QuickSlotArray;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "QuickSlot")
	int CurQuickKey = 0;

	//������
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item")
	class AItem* CurItem;

	//UI
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
	TSubclassOf <UUserWidget> MainUIClass;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category = "UI")
	class UUserWidget* MainUI;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSubclassOf <UUserWidget> InventoryUIClass;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
	class UUserWidget* InventoryUI;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSubclassOf <UUserWidget> StorageUIClass;
	class UUserWidget* StorageUI;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Partner")
	class APartner* Partner;
	

	//Delegate
	UPROPERTY(BlueprintAssignable, VisibleAnywhere, BlueprintCallable, BlueprintReadWrite)
	FDynamicDeleParam FMouseWheelDelegate;
	UPROPERTY(BlueprintAssignable, VisibleAnywhere, BlueprintCallable, BlueprintReadWrite)
	FDynamicDele FIKeyDelegate;


public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void PostInitializeComponents() override;
	virtual void PossessedBy(AController* NewController) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	void LMBDown();
	UFUNCTION(BlueprintCallable)
	void RMBDown();
	UFUNCTION(BlueprintCallable)
	void RMBUp();
	UFUNCTION(BlueprintCallable)
	void WheelInput(float Val);
	UFUNCTION(BlueprintCallable)
	void ChangeQuickslot(float Val);
	UFUNCTION(BlueprintCallable)
	void SetQuickslot(class UItemData* TargetData, int Key);
	UFUNCTION(BlueprintCallable)
	void OpenInventory();

	UFUNCTION()
	void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	bool bZoom;

private:
	// Character Movement Input
	void MoveForward(float Val);
	void MoveRight(float Val);
	void LookUp(float NewAxisValue);
	void Turn(float NewAxisValue);
};
