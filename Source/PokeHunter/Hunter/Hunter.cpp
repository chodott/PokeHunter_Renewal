// Fill out your copyright notice in the Description page of Project Settings.

#include "Hunter.h"

#include <mftransform.h>
#include <vector>

#include "InventoryComponent.h"
#include "HunterAnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/PlayerController.h"
#include "Camera/CameraComponent.h"
#include "Components\SphereComponent.h"
#include "Components\CapsuleComponent.h"
#include "Components/TimelineComponent.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetTree.h"
#include "PokeHunter/Npc/Npc.h"
#include "PokeHunter/Partner/Partner.h"
#include "PokeHunter/Item/Item.h"
#include "PokeHunter/Item/ItemData.h"
#include "PokeHunter/Item/Bullet.h"
#include "PokeHunter/Base/InteractActor.h"
#include "PokeHunter/Base/DatabaseActor.h"
#include "PokeHunter/Base/ItemDropActor.h"




// Sets default values
AHunter::AHunter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//  Mesh
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_HUNTER(TEXT("/Game/Hunter/Asset/YBot/Y_Bot.Y_Bot"));
	if (SK_HUNTER.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(SK_HUNTER.Object);
		GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -90.0f), FRotator(0.0f, -90.0f, 0.0f));
	}

	// Animation
	static ConstructorHelpers::FClassFinder<UAnimInstance> ANIM_HUNTER(TEXT("/Game/Hunter/Blueprint/BP_HunterAnimInstance"));
	if (ANIM_HUNTER.Succeeded()) 
	{
		GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
		GetMesh()->SetAnimInstanceClass(ANIM_HUNTER.Class);
	}

	//Timeline
	static ConstructorHelpers::FObjectFinder<UCurveFloat>DIVE_CURVE(TEXT("/Game/Hunter/Blueprint/DiveCurve.DiveCurve"));
	if (DIVE_CURVE.Succeeded())
	{
		DiveCurve = DIVE_CURVE.Object;
	}



	//Camera
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetOffset = FVector(0, 0, GetDefaultHalfHeight());
	CameraBoom->TargetArmLength = 500.f;
	CameraBoom->bUsePawnControlRotation = true;
	CameraBoom->bEnableCameraLag = true;
	ArmLengthTo = 200.f;
	ArmSpeed = 20.f;
	CameraZoomTo = 100.f;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	//Collision
	GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &AHunter::OnOverlapBegin);
	GetCapsuleComponent()->OnComponentEndOverlap.AddDynamic(this, &AHunter::OnOverlapEnd);

	//UI
	static ConstructorHelpers::FClassFinder<UUserWidget>TempInvenClass(TEXT("/Game/UI/InvenStorage/WBP_InventoryList"));
	if (TempInvenClass.Succeeded())
	{
		InventoryUIClass = TempInvenClass.Class;
	}
	
	static ConstructorHelpers::FClassFinder<UUserWidget>TempMainClass(TEXT("/Game/UI/WBP_MainUI"));
	if (TempMainClass.Succeeded())
	{
		MainUIClass = TempMainClass.Class;
	}

	//Using Controller Rotation
	bUseControllerRotationYaw = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;

	// Character Rotation Movement
	GetCharacterMovement()->bOrientRotationToMovement = true;

	//Inventory
	Inventory = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));
	Inventory->capacity = 20;
	Inventory->Hunter = this;

	//Set Quickslot Default
	for (int i = 0; i < 10; ++i)
	{
		QuickSlotArray.AddDefaulted();
	}

	//Set PartnerSkill Default
	for (int i = 0; i < 12; ++i)
	{
		HunterInfo.PartnerSkillArray.AddDefaulted();
	}

	TeamID = FGenericTeamId(0);

}

// Called when the game starts or when spawned
void AHunter::BeginPlay()
{
	Super::BeginPlay();

	//Delegate
	FIKeyDelegate.AddDynamic(this, &AHunter::OpenInventory);
	FMouseWheelDelegate.AddDynamic(this, &AHunter::ChangeQuickslot);

	//카메라 래깅 관련
	// Controller->bFindCameraComponentWhenViewTarget = true;

	//UI
	MainUI = CreateWidget(GetWorld(), MainUIClass, TEXT("MainUI"));
	MainUI->AddToViewport();

	//Timeline
	DiveInterpCallback.BindUFunction(this, FName("DiveInterpReturn"));
	if (DiveCurve)
	{
		DiveTimeline.AddInterpFloat(DiveCurve, DiveInterpCallback);
		DiveTimeline.SetTimelineLength(1.63f);
	}
}

// Called every frame
void AHunter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	DiveTimeline.TickTimeline(DeltaTime);

	if (CurState == EPlayerState::Zoom)
	{
		CameraBoom->TargetArmLength = FMath::FInterpTo(CameraBoom->TargetArmLength,ArmLengthTo, DeltaTime, ArmSpeed);
		CameraBoom->SetRelativeLocation(FVector(0,FMath::FInterpTo(CameraBoom->GetRelativeLocation().Y, CameraZoomTo, DeltaTime, ArmSpeed),0));
	}

	else if (CurState == EPlayerState::Run)
	{
		if (HunterInfo.HunterStamina > 0) HunterInfo.HunterStamina -= DeltaTime * 1;
		else
		{
			HunterInfo.HunterStamina = 0;
			GetCharacterMovement()->MaxWalkSpeed = 600.f;
		}
	}

	else
	{
		CameraBoom->TargetArmLength = FMath::FInterpTo(CameraBoom->TargetArmLength, 500.f, DeltaTime, ArmSpeed);
		float temp = FMath::FInterpTo(CameraBoom->GetRelativeLocation().Y, 0.f, DeltaTime, ArmSpeed);
		CameraBoom->SetRelativeLocation(FVector(0.f, temp, 0.f));

		if (HunterInfo.HunterStamina < 100) HunterInfo.HunterStamina += DeltaTime * 1;
		else HunterInfo.HunterStamina = 100;
	}
}

void AHunter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	auto AnimInstance = GetMesh()->GetAnimInstance();
	AnimInstance->OnMontageEnded.AddDynamic(this, &AHunter::OnMontageEnded);

}

void AHunter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
}

// Called to bind functionality to input
void AHunter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis("MoveForward", this, &AHunter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AHunter::MoveRight);

	//PlayerInputComponent->BindAxis("Turn", this, &AHunter::AddControllerYawInput);		// X
	//PlayerInputComponent->BindAxis("LookUp", this, &AHunter::AddControllerPitchInput);	// Y

	PlayerInputComponent->BindAxis("Turn", this, &AHunter::Turn);		// X
	PlayerInputComponent->BindAxis("LookUp", this, &AHunter::LookUp);	// Y

	PlayerInputComponent->BindAxis("MouseWheel", this, &AHunter::WheelInput);

	PlayerInputComponent->BindAction("SpaceBar", IE_Pressed, this, &AHunter::SpaceDown);
	PlayerInputComponent->BindAction("LShift", IE_Pressed, this, &AHunter::LShiftDown);
	PlayerInputComponent->BindAction("LShift", IE_Released, this, &AHunter::LShiftUp);
	PlayerInputComponent->BindAction("LMB", IE_Pressed, this, &AHunter::LMBDown);
	PlayerInputComponent->BindAction("RMB", IE_Pressed, this, &AHunter::RMBDown);
	PlayerInputComponent->BindAction("RMB", IE_Released, this, &AHunter::RMBUp);
	PlayerInputComponent->BindAction("I_Key", IE_Pressed, this, &AHunter::OpenInventory);
	PlayerInputComponent->BindAction("E_Key", IE_Pressed, this, &AHunter::EKeyDown);
	PlayerInputComponent->BindAction("G_Key", IE_Pressed, this, &AHunter::GKeyDown);
	PlayerInputComponent->BindAction("Ctrl", IE_Pressed, this, &AHunter::CtrlDown);
	PlayerInputComponent->BindAction("Ctrl", IE_Released, this, &AHunter::CtrlUp);
	PlayerInputComponent->BindAction("1_Key", IE_Pressed, this, &AHunter::Use1Skill);
	PlayerInputComponent->BindAction("2_Key", IE_Pressed, this, &AHunter::Use2Skill);
	PlayerInputComponent->BindAction("3_Key", IE_Pressed, this, &AHunter::Use3Skill);
	PlayerInputComponent->BindAction("4_Key", IE_Pressed, this, &AHunter::Use4Skill);
}

void AHunter::SpaceDown()
{
	if(CurState == EPlayerState::Idle)
	{
		CurState = EPlayerState::Dive;
		auto AnimInstance = Cast<UHunterAnimInstance>(GetMesh()->GetAnimInstance());
		if(AnimInstance) AnimInstance->PlayCombatMontage(FName("Dive"));
		FVector Speed = GetVelocity();
		FVector XYspeed = FVector(Speed.X, Speed.Y, 0.f);
		LastInput = GetCharacterMovement()->GetLastInputVector();
		if (XYspeed.Size() <= 300.f) LastSpeed = 300.0f;
		else 
		{
			LastSpeed = XYspeed.Size();
		}

		DiveTimeline.PlayFromStart();
	}
}

void AHunter::MoveForward(float Val)
{
	if (CurState == EPlayerState::Dive) return;
	if (Val != 0.0f)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

		//���� ����
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		 AddMovementInput(Direction, Val);
	}
}

void AHunter::MoveRight(float Val)
{
	if (CurState == EPlayerState::Dive) return;
	
	if (Val != 0.0f)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

		//���� ����
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		 AddMovementInput(Direction, Val);
	}
}

void AHunter::Turn(float NewAxisValue)
{
	if (bPartnerMode) return;

	AddControllerYawInput(NewAxisValue);
}

void AHunter::LookUp(float NewAxisValue)
{
	if (bPartnerMode) return;

	AddControllerPitchInput(NewAxisValue);
}

void AHunter::LShiftDown()
{
	if (CurState == EPlayerState::Idle)
	{
		GetCharacterMovement()->MaxWalkSpeed = 1000.f;
	}
}

void AHunter::LShiftUp()
{
	
}

void AHunter::LMBDown()
{
	if (CurState == EPlayerState::Dive) return;

	if (bPartnerMode)
	{
		FHitResult HitResult;
		APlayerController* PlayerController = Cast<APlayerController>(GetController());
		PlayerController->GetHitResultUnderCursorByChannel(ETraceTypeQuery::TraceTypeQuery1, 0, HitResult);
		//HitResult.Location;
		if (HitResult.bBlockingHit)
		{
			Partner->TargetPos = HitResult.Location;
			Partner->bOrdered = true;
			Partner->CurState = EPartnerState::MoveTarget;
		}

	}
	else if (CurState == EPlayerState::Zoom)
	{
		auto AnimInstance = Cast<UHunterAnimInstance>(GetMesh()->GetAnimInstance());
		if (AnimInstance) AnimInstance->PlayCombatMontage(FName("Shot"));

		FName ItemID = QuickSlotArray[CurQuickKey].ItemID;
		AActor* TempActor = UGameplayStatics::GetActorOfClass(GetWorld(), ADatabaseActor::StaticClass());
		ADatabaseActor* DatabaseActor = Cast<ADatabaseActor>(TempActor);
		if (DatabaseActor && ItemID != "None")
		{
			TSubclassOf<AItem> ItemClass = DatabaseActor->FindItem(ItemID)->ItemInfo.ItemClass;
			if (ItemClass == NULL) return;
			else
			{
				FHitResult* HitResult = new FHitResult();
				FVector StartTrace = FollowCamera->GetComponentLocation();
				FVector EndTrace = StartTrace + FollowCamera->GetForwardVector() * 5000.f;

				if (GetWorld()->LineTraceSingleByChannel(*HitResult, StartTrace, EndTrace, ECC_Visibility))
				{
					DrawDebugLine(
						GetWorld(),
						StartTrace,
						HitResult->Location,
						FColor(255, 0, 0),
						false, 3, 0,
						12.333
					);
					EndTrace = HitResult->Location;
				}

			
				//ABullet* Bullet = GetWorld()->SpawnActor<ABullet>(ItemClass, GetActorLocation() + GetActorForwardVector() * 100.f, GetControlRotation());
				ABullet* Bullet = GetWorld()->SpawnActor<ABullet>(ItemClass, GetMesh()->GetSocketLocation(FName("Muzzle")),GetControlRotation());
				Bullet->UseItem(this, GetMesh()->GetSocketLocation(FName("Muzzle")), EndTrace);
				QuickSlotArray[CurQuickKey].cnt--;
				if (QuickSlotArray[CurQuickKey].cnt == 0)
				{
					for (auto& Info : Inventory->InfoArray)
					{
						if (Info.ItemID == ItemID)
						{
							Info.ItemID == FName("None");
							Info.cnt = 0;
						}
						QuickSlotArray[CurQuickKey].ItemID = FName("None");
					}
				}
				
			}
		}
	}
	else 
	{
		if (QuickSlotArray[CurQuickKey].ItemID != FName("None"))
		{
			//Use Item Update need
			FName ItemID = QuickSlotArray[CurQuickKey].ItemID;
			AActor* TempActor = UGameplayStatics::GetActorOfClass(GetWorld(), ADatabaseActor::StaticClass());
			ADatabaseActor* DatabaseActor = Cast<ADatabaseActor>(TempActor);
			if(DatabaseActor)
			{ 
				TSubclassOf<AItem> ItemClass = DatabaseActor->FindItem(ItemID)->ItemInfo.ItemClass;
				if (ItemClass == NULL) return;
				else GetWorld()->SpawnActor<AItem>(ItemClass, GetActorLocation(), GetControlRotation());
			}

			QuickSlotArray[CurQuickKey].cnt--;
			if (QuickSlotArray[CurQuickKey].cnt == 0)
			{
				for (auto& Info : Inventory->InfoArray)
				{
					if (Info.ItemID == ItemID)
					{
						Info.ItemID == FName("None");
						Info.cnt = 0;
					}
					QuickSlotArray[CurQuickKey].ItemID = FName("None");
				}
			}

		}
	}
}

void AHunter::RMBDown()
{
	if (CurState == EPlayerState::Idle && !GetCharacterMovement()->IsFalling())
	{
		CurState = EPlayerState::Zoom;
		bUseControllerRotationYaw = true;
		bUseControllerRotationPitch = true;
		GetCharacterMovement()->bOrientRotationToMovement = false;

		GetCharacterMovement()->MaxWalkSpeed = 300.f;
	}
}

void AHunter::RMBUp()
{
	if(CurState == EPlayerState::Zoom)
	{
		bUseControllerRotationYaw = false;
		bUseControllerRotationPitch = false;
		GetCharacterMovement()->bOrientRotationToMovement = true;
		CurState = EPlayerState::Idle;

		SetActorRelativeRotation(FRotator(0, GetControlRotation().Yaw, GetControlRotation().Roll));
		Cast<UCharacterMovementComponent>(GetCharacterMovement())->MaxWalkSpeed = 600.0f;
	}
}

void AHunter::WheelInput(float Val)
{
	if (Val != 0.0f)
	{
		ChangeQuickslot(Val);
	}
}

void AHunter::ChangeQuickslot(float Val)
{
	CurQuickKey += int(Val);
	if (CurQuickKey < 0) CurQuickKey += 10;
	else if (CurQuickKey > 9) CurQuickKey -= 10;
	if (FMouseWheelDelegate.IsBound()) FMouseWheelDelegate.Broadcast(Val);
}

void AHunter::SetQuickslot(FName ItemID, int index)
{
	QuickSlotArray[index].ItemID = ItemID;
	QuickSlotArray[index].cnt = 10;
}

void AHunter::OpenInventory()
{

}

void AHunter::EKeyDown()
{
	if (InteractingActor)
	{
		auto AnimInstance = Cast<UHunterAnimInstance>(GetMesh()->GetAnimInstance());
		//Left Right Check
		FVector TargetVector = (InteractingActor->GetActorLocation() - GetActorLocation());
		FVector HunterForwardVector = GetActorForwardVector();
		TargetVector.Z = 0;
		TargetVector.Normalize();
		HunterForwardVector.Z = 0;
		FVector cross = FVector::CrossProduct(HunterForwardVector, TargetVector);
		float sign = FMath::Sign(cross.Z);
		if (Cast<AItemDropActor>(InteractingActor))
		{
			if (sign <= 0)
			{
				AnimInstance->PlayInteractMontage(FName("RunLeftPickup"));
			}
			else
			{
				AnimInstance->PlayInteractMontage(FName("RunRightPickup"));
			}
			bUpperOnly = true;
		}

		InteractingActor->Interact_Implementation(this);
		return;
	}
}

void AHunter::GKeyDown()
{
	FHitResult HitResult;
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	PlayerController->GetHitResultUnderCursorByChannel(ETraceTypeQuery::TraceTypeQuery1,0,HitResult);
	//HitResult.Location;
	if (HitResult.bBlockingHit)
	{
		Partner->TargetPos = HitResult.Location;
		Partner->bOrdered = true;
	}
}

void AHunter::CtrlDown()
{
	if (Partner == NULL) return;
	bPartnerMode = true;
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	PlayerController->bShowMouseCursor = true;

}

void AHunter::CtrlUp()
{
	if (Partner == NULL) return;
	bPartnerMode = false;
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	PlayerController->bShowMouseCursor = false;
}

void AHunter::Use1Skill()
{
	UE_LOG(LogTemp, Warning, TEXT("Skill1"));
	if (Partner)
	{
		Partner->UseNormalSkill(HunterInfo.PartnerSkillArray[0]);
	}
}

void AHunter::Use2Skill()
{
	UE_LOG(LogTemp, Warning, TEXT("Skill2"));
	if (Partner)
	{
		Partner->UseNormalSkill(HunterInfo.PartnerSkillArray[1]);
	}
}

void AHunter::Use3Skill()
{
	UE_LOG(LogTemp, Warning, TEXT("Skill3"));
	if (Partner)
	{
		Partner->UseSpecialSkill(HunterInfo.PartnerSkillArray[2]);
	}
}

void AHunter::Use4Skill()
{
	UE_LOG(LogTemp, Warning, TEXT("Skill4"));
	if (Partner)
	{
		Partner->UseSpecialSkill(HunterInfo.PartnerSkillArray[3]);
	}
}

void AHunter::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (CurState == EPlayerState::Dive) 
	{
		CurState = EPlayerState::Idle;
	}

	if (bUpperOnly) bUpperOnly = false;
}

void AHunter::OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OverlappedComp == GetCapsuleComponent()) UE_LOG(LogTemp, Warning, TEXT("OverlapBegin"));
	if (InteractingActor == nullptr)
	{
		if (OtherActor->GetClass()->ImplementsInterface(UInteractInterface::StaticClass()))
		{
			InteractingActor = Cast<AInteractActor>(OtherActor);
		}
	}
}

void AHunter::OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor->GetClass()->ImplementsInterface(UInteractInterface::StaticClass()))
	{
		if (InteractingActor == Cast<AInteractActor>(OtherActor))
		{
			UE_LOG(LogTemp, Warning, TEXT("OverlapEnd"));
			InteractingActor = NULL;
		}
		else return;
	}

	if (InteractingActor == nullptr)
	{
		TArray<AActor*> OverlappedActors;
		GetCapsuleComponent()->GetOverlappingActors(OverlappedActors);
		for (int32 i = 0; i < OverlappedActors.Num(); ++i)
		{
			if (OverlappedActors[i]->GetClass()->ImplementsInterface(UInteractInterface::StaticClass()))
			{
				InteractingActor = Cast<AInteractActor>(OverlappedActors[i]);
				break;
			}
		}
	}
}

void AHunter::DiveInterpReturn(float Value)
{
	AddMovementInput(LastInput, 1.0f);
}

void AHunter::SetPartnerSkill(TArray<ESkillID> SkillArray, int SkillListNum)
{
	for (int i = 0; i < 4; ++i)
	{
		HunterInfo.PartnerSkillArray[4 * SkillListNum + i] = SkillArray[i];
	}
}

void AHunter::SetPartnerTarget(AActor* setTarget)
{
	if (Partner != NULL)
	{
		Partner->SetTarget(setTarget);
	}
	
}