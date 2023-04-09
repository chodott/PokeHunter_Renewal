// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "..\PokeHunter.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "GenericTeamAgentInterface.h"
#include "PokeHunter/Item/ItemData.h"
#include "PokeHunter/Base/SkillData.h"
#include "PokeHunter/Base/ItemInteractInterface.h"
#include "PokeHunter/Base/EnemyInteractInterface.h"
#include "Hunter.generated.h"

//Dynamic 
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDynamicDele);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDynamicDeleParam, float, val);

UENUM(BlueprintType)
enum class EPlayerState : uint8
{
	Idle,
	Dive,
	Zoom,
	Install,
	Drink
};

USTRUCT(BlueprintType)
struct FHunterInfo
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Status")
	int HunterNum;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Status")
	float HunterHP{100};

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Status")
	float HunterStamina{ 100 };

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, category = "Status")
	bool bCanCombat;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Partner Skill")
	TArray<ESkillID> PartnerSkillArray;

	
};

UCLASS()
class POKEHUNTER_API AHunter : public ACharacter, public IGenericTeamAgentInterface, public IItemInteractInterface, public IEnemyInteractInterface
{
	GENERATED_BODY()

public:

	//Camera Component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	class USpringArmComponent* CameraBoom;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	class UCameraComponent* FollowCamera;
	//Inventory Component
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Inventory")
	class UInventoryComponent* Inventory;

	//
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Partner")
	class UStaticMeshComponent* DestinationMesh;

	//HunterInfo
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "HunterStatus")
	FHunterInfo HunterInfo;
	
	//
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction")
	class AInteractActor* InteractingActor;

	//QuickSlot
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "QuickSlot")
	TArray<FItemCnter> QuickSlotArray;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "QuickSlot")
	int CurQuickKey{};

	//Item
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

	//Partner
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Partner")
	class APartner* Partner;

	//Delegate
	UPROPERTY(BlueprintAssignable, VisibleAnywhere, BlueprintCallable, BlueprintReadWrite)
	FDynamicDeleParam FMouseWheelDelegate;
	UPROPERTY(BlueprintAssignable, VisibleAnywhere, BlueprintCallable)
	FDynamicDele FIKeyDelegate;

	//Timeline
	FTimeline DiveTimeline;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Timeline")
	UCurveFloat* DiveCurve;
	FOnTimelineFloat DiveInterpCallback;
	float LastSpeed;
	FVector LastInput;

	//Camera Variable
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Camera")
	float ArmLengthTo;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Camera")
	float CameraZoomTo;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Camera")
	float ArmSpeed;

	//PlayerState
	UPROPERTY(VisibleAnywhere, Replicated, BlueprintReadOnly, Category = "Animation")
	EPlayerState CurState {EPlayerState::Idle};

	//Animation
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation")
	class UHunterAnimInstance* HunterAnim;

	//Bool
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation")
	bool bUpperOnly;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Partner")
	bool bPartnerMode;
	
	//TeamID
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Battle")
	FGenericTeamId TeamID;

	//
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Battle")
	float ReloadTime{};
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Battle")
	float StartShotTime;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Battle")
	bool bCanShot;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Battle")
	bool bInvincible{ false};
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Battle")
	float InvincibleTime{ 1.f };
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Battle")
	float StartInvincibleTime;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Battle")
	float HealPerSecondAmount;
	int SaveSecond;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Battle")
	bool bGrabbed;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Battle")
	bool bBound;

protected:
	// Sets default values for this character's properties
	AHunter();
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void PostInitializeComponents() override;
	virtual void PossessedBy(AController* NewController) override;
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser);

	//Perception
	FGenericTeamId GetGenericTeamId()const override;

	//Replicated
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps)const;
	UFUNCTION(Server, Reliable)
	void ServerPlayMontage (AHunter* Hunter, FName Session);
	UFUNCTION(NetMulticast, Reliable)
	void MultiPlayMontage(AHunter* Hunter, FName Session);

	UFUNCTION(Server, Reliable)
	void ServerSprint(AHunter* Hunter, bool bSprinting);
	UFUNCTION(NetMulticast, Reliable)
	void MultiSprint(AHunter* Hunter, bool bSprinting);

	UFUNCTION(Server, Reliable)
	void ServerZoom(AHunter* Hunter, bool bZoom);
	UFUNCTION(NetMulticast, Reliable)
	void MultiZoom(AHunter* Hunter, bool bZoom);

	UFUNCTION(Server, Reliable)
	void ServerStartInvincibility();
	UFUNCTION(NetMulticast, Reliable)
	void MultiStartInvincibility();
	UFUNCTION(BlueprintCallable)
	void StartInvincibility();

	//UFUNCTION(Server, Reliable)
	//void ServerSpawnItem(TSubclassOf<AItem> SpawnItemClass, FVector StartLoc, FVector EndLoc, FRotator Rotation);
	//UFUNCTION(NetMulticast, Reliable)
	//void MultiSpawnItem(TSubclassOf<AItem> SpawnItemClass, FVector StartLoc, FVector EndLoc, FRotator Rotation);

	//Status
	UFUNCTION(BlueprintCallable)
	void SetHP(float HP) { HunterInfo.HunterHP = HP; };
	UFUNCTION(BlueprintCallable)
	float GetHP() { return HunterInfo.HunterHP; };
	UFUNCTION(BlueprintCallable)
	void SetStamina(float Stamina) { HunterInfo.HunterStamina = Stamina; };


	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	UFUNCTION(BlueprintCallable)
	void SpaceDown();
	UFUNCTION(BlueprintCallable)
	void LShiftDown();
	UFUNCTION(BlueprintCallable)
	void LShiftUp();
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
	void SetQuickslot(FName ItemID, int index);
	UFUNCTION(BlueprintCallable)
	void OpenInventory();
	UFUNCTION(BlueprintCallable)
	void EKeyDown();
	UFUNCTION(BlueprintCallable)
	void GKeyDown();
	UFUNCTION(BlueprintCallable)
	void CtrlDown();
	UFUNCTION(BlueprintCallable)
	void CtrlUp();
	UFUNCTION(BlueprintCallable)
	void Use1Skill();
	UFUNCTION(BlueprintCallable)
	void Use2Skill();
	UFUNCTION(BlueprintCallable)
	void Use3Skill();
	UFUNCTION(BlueprintCallable)
	void Use4Skill();

	//Partner
	UFUNCTION(BlueprintCallable)
	void SetPartnerSkill(TArray<ESkillID> SkillArray, int SkillListNum);
	void SetPartnerTarget(AActor* setTarget);
	void SetPartner(class APartner* SelectedPartner) { Partner = SelectedPartner; };

	//Collision Function
	UFUNCTION()
	void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	//Animation Function
	UFUNCTION()
	void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	//Timeline Function
	UFUNCTION()
	void DiveInterpReturn(float Value);

	//ItemInterface Function
	virtual void InteractHealArea_Implementation();
	virtual void OutHealArea_Implementation();

	//EnemyInterface Function
	virtual void InteractEarthquake_Implementation();
	virtual void InteractAttack_Implementation(FVector HitLoc);
	
private:
	// Character Movement Input
	void MoveForward(float Val);
	void MoveRight(float Val);
	void LookUp(float NewAxisValue);
	void Turn(float NewAxisValue);
};
