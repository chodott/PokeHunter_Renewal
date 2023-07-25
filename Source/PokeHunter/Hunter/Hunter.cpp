// Fill out your copyright notice in the Description page of Project Settings.

#include "Hunter.h"

#include <mftransform.h>
#include <vector>

#include "InventoryComponent.h"
#include "HunterAnimInstance.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/PlayerController.h"
#include "Camera/CameraComponent.h"
#include "Components\SphereComponent.h"
#include "Components\CapsuleComponent.h"
#include "Components/TimelineComponent.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetTree.h"
#include "PokeHunter/Npc/Npc.h"
#include "PokeHunter/Partner/Partner.h"
#include "PokeHunter/Enemy/Enemy.h"
#include "PokeHunter/Item/Item.h"
#include "PokeHunter/Item/ItemData.h"
#include "PokeHunter/Item/Potion.h"
#include "PokeHunter/Item/Bullet.h"
#include "PokeHunter/Base/InteractActor.h"
#include "PokeHunter/Base/DatabaseActor.h"
#include "PokeHunter/Base/ItemDropActor.h"
#include "PokeHunter/Enemy/EnemyProjectile.h"
#include <Blueprint/WidgetBlueprintLibrary.h>
#include "HunterController.h"




// Sets default values
AHunter::AHunter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

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
	if (GetLocalRole() == ROLE_Authority)
	{
		GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &AHunter::OnOverlapBegin);
		GetCapsuleComponent()->OnComponentEndOverlap.AddDynamic(this, &AHunter::OnOverlapEnd);
	}
	
	// Particle System
	Heal_Effect = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("HealParticle"));
	Heal_Effect->SetupAttachment(GetRootComponent());
	static ConstructorHelpers::FObjectFinder<UParticleSystem> TempHealEffectClass(TEXT("/Game/Hunter/HealEffect/P_Heal_Circle.P_Heal_Circle"));
	if (TempHealEffectClass.Succeeded())
	{
		Heal_Effect->SetTemplate(TempHealEffectClass.Object);
		float characterFoot = GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
		Heal_Effect->SetRelativeLocation(FVector(0, 0, -characterFoot));
		Heal_Effect->Deactivate();
	}

	//Using Controller Rotation
	bUseControllerRotationYaw = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;

	// Character Rotation Movement
	GetCharacterMovement()->bOrientRotationToMovement = true;

	//Inventory
	Inventory = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));
	Inventory->capacity = 24;
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

	for (int i = 0; i < 4; ++i)
	{
		SkillInfoArray.AddDefaulted();
	}

	TeamID = FGenericTeamId(0);

	// Set Game instance
	gameinstance = Cast<UBaseInstance>(UGameplayStatics::GetGameInstance((GetWorld())));

	bReplicates = true;
}

// Called when the game starts or when spawned
void AHunter::BeginPlay()
{
	Super::BeginPlay();

	//Delegate
	FIKeyDelegate.AddDynamic(this, &AHunter::OpenInventory);
	FMouseWheelDelegate.AddDynamic(this, &AHunter::ChangeQuickslot);

	//Animation
	HunterAnim = Cast<UHunterAnimInstance>(GetMesh()->GetAnimInstance());

	//카메라 래깅 관련
	// Controller->bFindCameraComponentWhenViewTarget = true;

	//UI
	MainUI = CreateWidget(GetWorld(), MainUIClass, TEXT("MainUI"));
	MainUI->AddToViewport();
	// PauseUI = CreateWidget(GetWorld(), PauseUIClass, TEXT("PauseUI"));

	//Timeline
	DiveInterpCallback.BindUFunction(this, FName("DiveInterpReturn"));
	if (DiveCurve)
	{
		DiveTimeline.AddInterpFloat(DiveCurve, DiveInterpCallback);
		DiveTimeline.SetTimelineLength(0.66f);
	}

	ADatabaseActor* DatabaseActor = Cast<ADatabaseActor>(UGameplayStatics::GetActorOfClass(GetWorld(), ADatabaseActor::StaticClass()));
	TSubclassOf<APartner> partnerClass = DatabaseActor->FindPartner(gameinstance->myPartner);

	if (HasAuthority())
	{
		FVector SpawnLocation = GetActorLocation() + FVector(0, 200, 0);
		ServerSpawnPartner(this, partnerClass, SpawnLocation);
	}

	for (int i = 0; i < 4; ++i)
	{
		ESkillID SkillID = HunterInfo.PartnerSkillArray[static_cast<int>(PartnerType) * 4 + i];
		if (SkillID != ESkillID::None)
		{
			SkillInfoArray[i] = DatabaseActor->FindSkill(HunterInfo.PartnerSkillArray[static_cast<int>(PartnerType) * 4 + i]);
		}
	}

	// Set player controller in baseintance
	gameinstance->cur_playerController = Cast<APlayerController>(GetController());

	// Set item inventory
	if (false == gameinstance->InfoArray.IsEmpty()) {
		Inventory->InfoArray.Empty();
		int cur = 0;
		for (auto item : gameinstance->InfoArray) {
			Inventory->InfoArray.Add(item);
			++cur;
		}
		for (; cur < Inventory->capacity; ++cur) {
			Inventory->InfoArray.Add(FItemCnter{});
		}
	}

	MaterialIndex = gameinstance->mySkin;
	ServerChangeMaterialIndex(MaterialIndex);
}

// Called every frame
void AHunter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	DiveTimeline.TickTimeline(DeltaTime);

	if (CurState == EPlayerState::Zoom)
	{
		CameraBoom->TargetArmLength = FMath::FInterpTo(CameraBoom->TargetArmLength, ArmLengthTo, DeltaTime, ArmSpeed);
		CameraBoom->SetRelativeLocation(FVector(0, FMath::FInterpTo(CameraBoom->GetRelativeLocation().Y, CameraZoomTo, DeltaTime, ArmSpeed), 0));
	}

	else
	{
		CameraBoom->TargetArmLength = FMath::FInterpTo(CameraBoom->TargetArmLength, 500.f, DeltaTime, ArmSpeed);
		float temp = FMath::FInterpTo(CameraBoom->GetRelativeLocation().Y, 0.f, DeltaTime, ArmSpeed);
		CameraBoom->SetRelativeLocation(FVector(0.f, temp, 0.f));
	}

	//Stamina
	if (GetVelocity().Length() > 600.f)
	{
		if (HunterStamina > 0) HunterStamina -= DeltaTime * StaminaPerSecondAmount;
		else
		{
			SetStamina(0);
			GetCharacterMovement()->MaxWalkSpeed = 500.f;
		}
	}
	else
	{
		SetStamina(HunterStamina + DeltaTime * StaminaPerSecondAmount);
		if(HunterStamina >= 100) SetStamina(100);

	}


	//Invincible
	if (bInvincible)
	{
		float ElapsedTime = GetWorld()->TimeSeconds - StartInvincibleTime;
		float TimeLeft = InvincibleTime - ElapsedTime;
		if (TimeLeft <= 0.0f)
		{
			bInvincible = false;
		}
	}

	//HealPerSecond
	if (GetHP() < 100.f)
	{
		float ElapsedTime = GetWorld()->TimeSeconds;
		int CurSecond = FMath::FloorToInt(ElapsedTime);
		if (CurSecond == SaveSecond)
		{

		}
		else
		{
			SaveSecond = CurSecond;
			float NewHP = GetHP() + HealPerSecondAmount;
			if (NewHP > 100.f) NewHP = 100.f;
			SetHP(NewHP);
		}
	}

	//Reload
	if (!bCanShot)
	{
		float ElapsedTime = GetWorld()->TimeSeconds - StartShotTime;
		float TimeLeft = ReloadTime - ElapsedTime;
		if (TimeLeft <= 0.0f)
		{
			bCanShot = true;
		}
	}

	//SkillReload
	for (auto& SkillInfo : SkillInfoArray)
	{
		SkillInfo.CheckTime(DeltaTime);
	}
	UpdateSkillSlots();

	//bFalling
	if (bNoCollision)
	{
		float ElapsedTime = GetWorld()->TimeSeconds - StartNoCollisionTime;
		float TimeLeft = NoCollisionTime - ElapsedTime;
		if (TimeLeft <= 0.0f)
		{
			bNoCollision = false;
			SetActorEnableCollision(true);
		}
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

float AHunter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (bInvincible || CurState == EPlayerState::Die) return 0;

	ABullet* Bullet = Cast<ABullet>(DamageCauser);
	if (Bullet) return 0.f;


	HP -= DamageAmount;
	ServerHunterHP(gameinstance->MyName, HP);

	if (GetHP() <= 0)
	{ //죽었을 때
		ServerPlayMontage(this, FName("Die"));
		SetGenericTeamId(1);
		HealPerSecondAmount = 0.f;
		CurState = EPlayerState::Die;
		AEnemy* DamageEnemy = Cast<AEnemy>(DamageCauser);
		if(DamageEnemy) DamageEnemy->LeaveTarget(this);
		return 0; 
	}
		//입력 제한 필요
	if (DamageEvent.IsOfType(FPointDamageEvent::ClassID))
	{
		FPointDamageEvent& PointDamageEvent = (FPointDamageEvent&)DamageEvent;
		AEnemyProjectile* EnemyBullet = Cast<AEnemyProjectile>(DamageCauser);
		if (EnemyBullet)
		{
			FVector DirectionVec = EnemyBullet->GetActorLocation() - GetActorLocation();
			DirectionVec.Normalize();
			if (DirectionVec.Z <= 0.f)
			{
				DirectionVec.Z *= -1;
			}
			LaunchCharacter(DirectionVec * 1000.f, false, false);
		}
	}
	else
	{
		ServerPlayMontage(this, FName("NormalHit"));
		CurState = EPlayerState::Hit;
	}
	
	
	//HunterAnim->StopAllMontages(1.0f);
	StartInvincibility();
	SetInstallMode();

	return DamageAmount;
}

FGenericTeamId AHunter::GetGenericTeamId()const
{
	return TeamID;
}

void AHunter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);


	
	DOREPLIFETIME(AHunter, HunterStamina);
	DOREPLIFETIME(AHunter, CurState);
	DOREPLIFETIME(AHunter, Partner);
	DOREPLIFETIME(AHunter, InteractingActor);
	DOREPLIFETIME(AHunter, bUpperOnly);

	DOREPLIFETIME(AHunter, MaterialIndex);
	DOREPLIFETIME(AHunter, Material);
}

void AHunter::MultiSprint_Implementation(AHunter* Hunter, bool bSprinting)
{
	if (bSprinting && Hunter->CurState == EPlayerState::Idle)
	{
		Hunter->GetCharacterMovement()->MaxWalkSpeed = 700.f;
	}
	else if (!bSprinting)
	{
		Hunter->GetCharacterMovement()->MaxWalkSpeed = 500.f;
	}
}

void AHunter::ServerSprint_Implementation(AHunter* Hunter, bool bSprinting)
{
	Hunter->MultiSprint(Hunter, bSprinting);
}

void AHunter::MultiPlayMontage_Implementation(AHunter* Hunter, FName Session)
{
	Hunter->HunterAnim->PlayCombatMontage(Session);
}

void AHunter::ServerPlayMontage_Implementation(AHunter* Hunter ,FName Session)
{
	MultiPlayMontage(Hunter, Session);
	
}

void AHunter::ServerZoom_Implementation(AHunter* Hunter, bool bZoom)
{
	MultiZoom(Hunter, bZoom);
	if (bZoom)
	{
		MultiPlayMontage(Hunter, FName("Zoom"));
		Hunter->bUpperOnly = true;
	}
}

void AHunter::MultiZoom_Implementation(AHunter* Hunter, bool bZoom)
{
	if (bZoom)
	{
		if (CurState == EPlayerState::Idle && !GetCharacterMovement()->IsFalling())
		{
			CurState = EPlayerState::Zoom;
			bUseControllerRotationYaw = true;
			//bUseControllerRotationPitch = true;
			GetCharacterMovement()->bOrientRotationToMovement = false;

			GetCharacterMovement()->MaxWalkSpeed = 300.f;

		}
	}
	else
	{
		if (CurState == EPlayerState::Zoom)
		{
			SetInstallMode();
		}
	}
}

void AHunter::ServerInteractObject_Implementation(AInteractActor* TargetActor, AHunter* OwnerHunter)
{
	TargetActor->MultiInteract(OwnerHunter);
	OwnerHunter->MultiInteractObject(OwnerHunter);
}


void AHunter::MultiInteractObject_Implementation(AHunter* OwnerHunter)
{
	bUpperOnly = true;
}

void AHunter::ServerHunterHP_Implementation(FName PlayerName, float NewHP)
{
	MultiHunterHP(PlayerName, NewHP);
}

void AHunter::MultiHunterHP_Implementation(FName PlayerName, float NewHP)
{
	float* TempHP = PartyMemberHP.Find(PlayerName);
	if (TempHP) {
		*TempHP = NewHP;
	}
}

void AHunter::ServerPetHP_Implementation(FName PlayerName, float NewHP)
{
	MultiPetHP(PlayerName, NewHP);
}

void AHunter::MultiPetHP_Implementation(FName PlayerName, float NewHP)
{
	float* TempHP = PartyMemberPetHP.Find(PlayerName);
	if (TempHP) {
		*TempHP = NewHP;
	}
}

// Called to bind functionality to input
void AHunter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis("MoveForward", this, &AHunter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AHunter::MoveRight);

	//PlayerInputComponent->BindAxis("Turn", this, &AHunter::AddControllerYawInput);		// X
	//PlayerInputComponent->BindAxis("LookUp", this, &AHunter::AddControllerPitchInput);	// Y

	PlayerInputComponent->BindAxis("Turn", this, &AHunter::Turn);		// X
	PlayerInputComponent->BindAxis("LookUp", this, &AHunter::LookUp);	// Y

	//PlayerInputComponent->BindAxis("MouseWheel", this, &AHunter::WheelInput);

	PlayerInputComponent->BindAction("SpaceBar", IE_Pressed, this, &AHunter::SpaceDown);
	PlayerInputComponent->BindAction("LShift", IE_Pressed, this, &AHunter::LShiftDown);
	PlayerInputComponent->BindAction("LShift", IE_Released, this, &AHunter::LShiftUp);
	PlayerInputComponent->BindAction("LMB", IE_Pressed, this, &AHunter::LMBDown);
	PlayerInputComponent->BindAction("RMB", IE_Pressed, this, &AHunter::RMBDown);
	PlayerInputComponent->BindAction("RMB", IE_Released, this, &AHunter::RMBUp);
	PlayerInputComponent->BindAction("I_Key", IE_Pressed, this, &AHunter::OpenInventory);
	PlayerInputComponent->BindAction("E_Key", IE_Pressed, this, &AHunter::EKeyDown);
	PlayerInputComponent->BindAction("R_Key", IE_Pressed, this, &AHunter::RKeyDown);
	PlayerInputComponent->BindAction("Ctrl", IE_Pressed, this, &AHunter::CtrlDown);
	PlayerInputComponent->BindAction("Ctrl", IE_Released, this, &AHunter::CtrlUp);
	PlayerInputComponent->BindAction("1_Key", IE_Pressed, this, &AHunter::Use1Skill);
	PlayerInputComponent->BindAction("2_Key", IE_Pressed, this, &AHunter::Use2Skill);
	PlayerInputComponent->BindAction("3_Key", IE_Pressed, this, &AHunter::Use3Skill);
	PlayerInputComponent->BindAction("4_Key", IE_Pressed, this, &AHunter::Use4Skill);

	// P_Key는 Debug Only
	PlayerInputComponent->BindAction("P_Key", IE_Pressed, this, &AHunter::ESCKeyDown);
	PlayerInputComponent->BindAction("Escape", IE_Pressed, this, &AHunter::ESCKeyDown);
}

void AHunter::SpaceDown()
{
	if (GetCharacterMovement()->IsFalling()) return;

	if(CurState == EPlayerState::Idle)
	{
		FVector Speed = GetVelocity();
		FVector XYspeed = FVector(Speed.X, Speed.Y, 0.f);
		if (XYspeed.Size() <= 0.f) return;
		else if (XYspeed.Size() <= 500.f) LastSpeed = 500.0f;
		else
		{
			LastSpeed = XYspeed.Size();
		}
		SetStamina(HunterStamina - 15.f);
		CurState = EPlayerState::Dive;
		LastInput = GetCharacterMovement()->GetLastInputVector();
		GetCharacterMovement()->Velocity = LastInput * GetCharacterMovement()->GetMaxSpeed();
		auto AnimInstance = Cast<UHunterAnimInstance>(GetMesh()->GetAnimInstance());
		ServerPlayMontage(this, FName("Dive"));
	
		ServerStartInvincibility();
		DiveTimeline.PlayFromStart();
	}
}

void AHunter::MoveForward(float Val)
{
	if (CurState == EPlayerState::Dive || CurState == EPlayerState::Install || bGrabbed) return;
	if (Val != 0.0f)
	{
		FHitResult HitResult;
		FVector EndLocation = GetActorLocation() + GetActorForwardVector() * 50;
		FCollisionQueryParams TraceParams = FCollisionQueryParams::DefaultQueryParam;
		TraceParams.AddIgnoredActor(this);
		GetWorld()->LineTraceSingleByProfile(HitResult, GetActorLocation(), EndLocation, FName("Player"), TraceParams);
		//GetWorld()->SweepSingleByProfile(HitResult, GetActorLocation(), EndLocation, FQuat::Identity, FName("Player"), GetCapsuleComponent()->GetCollisionShape(), TraceParams);
		if (HitResult.bBlockingHit)
		{
			//DrawDebugLine(GetWorld(), GetActorLocation(), EndLocation, FColor::Red,false, 10.f, 0U , 3.f);
			return;
		}
		
		bDamaged = false;
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

		//���� ����
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		 AddMovementInput(Direction, Val);
	}
}

void AHunter::MoveRight(float Val)
{
	if (CurState == EPlayerState::Dive || CurState == EPlayerState::Install || bGrabbed) return;
	
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
	if (bBound) return;
	ServerSprint(this, true);
}

void AHunter::LShiftUp()
{
	if(CurState != EPlayerState::Zoom)
	ServerSprint(this, false);
}

void AHunter::LMBDown()
{
	if (CurState == EPlayerState::Dive || bBound) return;

	if (bPartnerMode)
	{
		FHitResult HitResult;
		APlayerController* PlayerController = Cast<APlayerController>(GetController());
		PlayerController->GetHitResultUnderCursorByChannel(ETraceTypeQuery::TraceTypeQuery1, 0, HitResult);
		//HitResult.Location;
		if (HitResult.bBlockingHit)
		{
			ServerSetPartnerPosition(Partner, HitResult.Location);
		}
		return;
	}

	if (QuickSlotArray[CurQuickKey].ItemID != FName("None"))
	{
		FName ItemID = QuickSlotArray[CurQuickKey].ItemID;
		ADatabaseActor* DatabaseActor = Cast<ADatabaseActor>(UGameplayStatics::GetActorOfClass(GetWorld(), ADatabaseActor::StaticClass()));
		if (DatabaseActor)
		{
			TSubclassOf<AItem> ItemClass = DatabaseActor->FindItem(ItemID)->ItemInfo.ItemClass;
			if (ItemClass == NULL) return; 
			auto AnimInstance = Cast<UHunterAnimInstance>(GetMesh()->GetAnimInstance());
			
			//ZoomMode
			if (!bCanShot) return;
			if (CurState == EPlayerState::Zoom)
			{
				bool bIsBullet = ItemClass->IsChildOf(ABullet::StaticClass());
				if (!bIsBullet) return;
				FVector StartTrace = FollowCamera->GetComponentLocation() + FollowCamera->GetForwardVector() * 150.f;
				FVector EndTrace = FollowCamera->GetComponentLocation() + FollowCamera->GetForwardVector() * 3000.f;
				FHitResult* HitResult = new FHitResult();
				FCollisionQueryParams BulletTraceParams(FName("Visibility"), true, this);
				if(GetWorld()->LineTraceSingleByChannel(*HitResult, StartTrace, EndTrace, ECC_Visibility, BulletTraceParams))
				{
					//Debug LineTrace
					/*DrawDebugLine(
						GetWorld(),
						StartTrace,
						HitResult->Location,
						FColor(255, 0, 0),
						false, 3, 0,
						12.333
					);*/
					EndTrace = HitResult->Location;
				}
				else
				{
					EndTrace = FollowCamera->GetComponentLocation() + FollowCamera->GetForwardVector() * 3000.f;
				}
				StartTrace = GetMesh()->GetSocketLocation(FName("Muzzle"));

				ServerSpawnBullet(this, ItemClass, StartTrace, EndTrace, GetControlRotation());
			}
			//NormalMode
			else
			{
				if (ItemClass->IsChildOf(APotion::StaticClass()))
				{
					bUpperOnly = true;
					ServerUsePotion(this, ItemClass);
				}
				else
				{
					ServerSpawnItem(this, ItemClass, GetActorLocation(), FVector::ZeroVector, GetControlRotation());
				}
				//AItem* Item = GetWorld()->SpawnActor<AItem>(ItemClass, GetActorLocation(),FRotator::ZeroRotator);
				//switch (Item->ItemType)
				//{
				//case EItemType::Bullet:
				//	Item->Destroy();
				//	return;
				//	break;

				//case EItemType::Potion:
				//	ServerPlayMontage(this, FName("Drink"));
				//	Item->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, FName("PotionSocket"));
				//	CurItem = Item;
				//	CurState = EPlayerState::Drink;
				//	bUpperOnly = true;
				//	break;

				//case EItemType::Trap:
				//	ServerPlayMontage(this, FName("Install"));
				//	FHitResult* HitResult = new FHitResult();
				//	FVector SpawnLoc = GetActorLocation() + GetActorForwardVector() * 200;
				//	if (GetWorld()->LineTraceSingleByChannel(*HitResult, SpawnLoc, SpawnLoc + FVector(0, 0, -100), ECollisionChannel::ECC_Pawn))
				//	{
				//		Item->SetActorLocation(HitResult->Location);
				//		CurState = EPlayerState::Install;
				//		Item->UseItem(this);
				//	}
				//	else
				//	{
				//		//실패 처리 필요
				//		Item->Destroy();
				//		return;
				//	}
				//	break;
				//}
				//
			}

			bCanShot = false;
			StartShotTime = GetWorld()->GetTimeSeconds();

			//인벤토리 처리
			bool bEmpty = 0;
			int SameItemCnt = 0;
			QuickSlotArray[CurQuickKey].cnt--;
			for (auto & Info : Inventory->InfoArray)
			{
				if (Info.ItemID == ItemID)
				{
					SameItemCnt++;
					Info.cnt -= 1;
					if (Info.cnt == 0)
					{
						bEmpty = true;
						Info.ItemID = FName("None");
					}
					else break;
				}
			}
			if (bEmpty && SameItemCnt == 1)
			{
				QuickSlotArray[CurQuickKey].ItemID = FName("None");
				QuickSlotArray[CurQuickKey].cnt = 0;
			}
			UpdateQuickSlot();
		}
	}
	
}

void AHunter::RMBDown()
{
	if (bBound) return;
	bUpperOnly = true;
	ServerZoom(this,true);
}

void AHunter::RMBUp()
{
	ServerZoom(this, false);
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
}

void AHunter::SetQuickslot(FName ItemID, int index)
{
	int TotalCnt = 0;
	for (auto& ItemCnt : Inventory->InfoArray)
	{
		if (ItemCnt.ItemID == ItemID)
		{
			TotalCnt += ItemCnt.cnt;
		}
	}
	QuickSlotArray[index].ItemID = ItemID;
	QuickSlotArray[index].cnt = TotalCnt;
}

void AHunter::OpenInventory()
{

}

void AHunter::EKeyDown()
{
	if (CurState == EPlayerState::Zoom || CurState == EPlayerState::Dive) return;

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
			bUpperOnly = true;
			if (sign <= 0)
			{
				ServerPlayMontage(this, FName("PickUp"));
			}
			else
			{
				ServerPlayMontage(this, FName("PickUp"));
			}
			
			ServerInteractObject(InteractingActor, this);

		}

		else
		{
			//npc
			InteractingActor->Interact_Implementation(this);
			ServerSprint(this, false);
		}

		return;
	}
}

void AHunter::RKeyDown()
{
	if (Partner)
	{
		ServerCancelOrder(Partner);
	}
}

void AHunter::ESCKeyDown()
{

}

void AHunter::CtrlDown()
{
	if (Partner == NULL) return;
	bPartnerMode = true;
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	FInputModeGameAndUI InputMode;
	InputMode.SetHideCursorDuringCapture(false);
	PlayerController->bShowMouseCursor = true;
	PlayerController->SetInputMode(InputMode);
}

void AHunter::CtrlUp()
{
	if (Partner == NULL) return;
	bPartnerMode = false;
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	PlayerController->bShowMouseCursor = false;
	PlayerController->SetInputMode(FInputModeGameOnly());
}

void AHunter::Use1Skill()
{
	UE_LOG(LogTemp, Warning, TEXT("Skill1"));
	if (Partner)
	{
		if (SkillInfoArray[0].CheckReady())
		{
			bUpperOnly = true;
			ServerPlayMontage(this, FName("Order"));
			ServerUsePartnerNormalSkill(Partner, SkillInfoArray[0].ID);
		}
	}
}

void AHunter::Use2Skill()
{
	UE_LOG(LogTemp, Warning, TEXT("Skill2"));
	if (Partner)
	{
		if (SkillInfoArray[1].CheckReady())
		{
			bUpperOnly = true;
			ServerPlayMontage(this, FName("Order"));
			ServerUsePartnerNormalSkill(Partner, SkillInfoArray[1].ID);
		}
	}
}

void AHunter::Use3Skill()
{
	UE_LOG(LogTemp, Warning, TEXT("Skill3"));
	if (Partner)
	{
		if (SkillInfoArray[2].CheckReady())
		{
			bUpperOnly = true;
			ServerPlayMontage(this, FName("Order"));
			ServerUsePartnerSpecialSkill(Partner, SkillInfoArray[2].ID);
		}
	}
}

void AHunter::Use4Skill()
{
	UE_LOG(LogTemp, Warning, TEXT("Skill4"));
	if (Partner)
	{
		if (SkillInfoArray[3].CheckReady())
		{
			bUpperOnly = true;
			ServerPlayMontage(this, FName("Order"));
			ServerUsePartnerSpecialSkill(Partner, SkillInfoArray[3].ID);
		}
	}
}

void AHunter::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (CurState == EPlayerState::Dive) 
	{
		UnCrouch();
		bInvincible = false;
		CurState = EPlayerState::Idle;
	}
	else if (CurState == EPlayerState::Install)
	{
		CurState = EPlayerState::Idle;
	}
	else if (CurState == EPlayerState::Drink)
	{
		CurItem->UseItem(this);
		Heal_Effect->Activate();
		if (CurItem != NULL) CurItem->Destroy();
		CurState = EPlayerState::Idle;
	}
	if(!bInterrupted)
	{
		if (bUpperOnly) bUpperOnly = false;
	}
}

void AHunter::OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (InteractingActor == nullptr)
	{
		if (OtherActor->Implements<UInteractInterface>())
		{
			InteractingActor = Cast<AInteractActor>(OtherActor);
		}
	}
}

void AHunter::OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor->Implements<UInteractInterface>())
	{
		if (InteractingActor == Cast<AInteractActor>(OtherActor))
		{
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

void AHunter::SetDiveCurveTime(float length)
{
	DiveTimeline.SetPlayRate(length);
}

void AHunter::InteractHealArea_Implementation()
{
	HealPerSecondAmount = 10.f;
}

void AHunter::OutHealArea_Implementation()
{
	HealPerSecondAmount = 1.f;
}

void AHunter::InteractPotion_Implementation(float HealAmount)
{
	SetHP(GetHP() + HealAmount);
}

void AHunter::InteractEarthquake_Implementation()
{
	if (GetCharacterMovement()->IsFalling()) return;

	if (bInvincible) return;

	SetStamina(HunterStamina - 30.f);

	LaunchCharacter(FVector(0, 0, 1000), false, false);
}

void AHunter::InteractAttack_Implementation(FVector HitDirection, float DamageAmount)
{
	if (DamageAmount <= 0.f || bDamaged)
	{
		return;
	}

	if (bInvincible) return;

	if (HitDirection.Z < 0.f) HitDirection.Z *= -1;

	//bDamaged = true;
	FVector TargetVec = FVector(HitDirection.X * -1, HitDirection.Y * -1, 0);
	FRotator TargetRot = TargetVec.Rotation();
	TargetRot.Pitch = 0;
	SetActorRelativeRotation(TargetRot);
	CurState = EPlayerState::Knockback;
	LaunchCharacter(HitDirection * 1000.f,false,false);
	StartNoCollisionTime = GetWorld()->GetTimeSeconds();
	bNoCollision = true;
	SetActorEnableCollision(false);
}

void AHunter::InteractGrabAttack_Implementation()
{
	bNoCollision = true;
	StartNoCollisionTime = GetWorld()->GetTimeSeconds();
}

void AHunter::InteractWideAttack_Implementation(float DamageAmount)
{
	if (DamageAmount <= 0.f || bDamaged)
	{
		return;
	}

	if (bInvincible) return;

	CurState = EPlayerState::Knockback;
	LaunchCharacter(FVector(0,0,1000), false, false);
	StartNoCollisionTime = GetWorld()->GetTimeSeconds();
	bNoCollision = true;
	SetActorEnableCollision(false);
}

void AHunter::SetPartnerSkill(TArray<ESkillID> SkillArray, int SkillListNum)
{
	for (int i = 0; i < 4; ++i)
	{
		HunterInfo.PartnerSkillArray[4 * SkillListNum + i] = SkillArray[i];
	}
}

void AHunter::SetPartnerTarget(ACharacter* setTarget)
{
	if (Partner != NULL)
	{
		Partner->SetTarget(setTarget);
	}
	
}

void AHunter::SetPartner(APartner* SelectedPartner)
{
	Partner = SelectedPartner;
	//gameinstance->myPartner = EPartnerType::WolfPartner;
}

bool AHunter::SuccessUseSkill(ESkillID SkillID)
{
	for (auto& SkillInfo : SkillInfoArray)
	{
		if (SkillInfo.ID == SkillID)
		{
			SkillInfo.UsedSkill();
			break;
		}
	}
	return true;
}

bool AHunter::CheckUseSkill(ESkillID SkillID)
{
	for (auto& SkillInfo : SkillInfoArray)
	{
		if (SkillInfo.ID == SkillID)
		{
			bool bCanUse = SkillInfo.CheckReady();
			if (bCanUse)
			{
				bUpperOnly = true;
				ServerPlayMontage(this, FName("Order"));
				ServerUsePartnerNormalSkill(Partner, HunterInfo.PartnerSkillArray[static_cast<int>(PartnerType) * 4 + 0]);
				return true;
			}
			else
			{
				return false;
			}
		}
	}
	return false;
}

void AHunter::UpdateSkillSlots_Implementation()
{
}

void AHunter::ServerStartInvincibility_Implementation()
{
	MultiStartInvincibility();
}

void AHunter::MultiStartInvincibility_Implementation()
{
	bInvincible = true;
	StartInvincibleTime = GetWorld()->TimeSeconds;
}

void AHunter::StartInvincibility()
{
	ServerStartInvincibility();
}

void AHunter::SetInstallMode()
{
	bUpperOnly = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationPitch = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	Cast<UCharacterMovementComponent>(GetCharacterMovement())->MaxWalkSpeed = 500.0f;
	if (CurState == EPlayerState::Zoom)
	{
		//SetActorRelativeRotation(FRotator(0, GetControlRotation().Yaw, GetControlRotation().Roll));
		HunterAnim->StopAllMontages(0.2f);
	}
	
	//shot or zoom start shut down/ BlendTime Option
	
	CurState = EPlayerState::Idle;
	
}

void AHunter::ServerSetPartnerPosition_Implementation(APartner* MyPartner, const FVector& LocVec)
{
	MultiSetPartnerPosition(LocVec);
}

void AHunter::MultiSetPartnerPosition_Implementation(const FVector& LocVec)
{
	Partner->TargetPos = LocVec;
	Partner->bOrdered = true;
}


void AHunter::ServerSpawnPartner_Implementation(AHunter* OwnerHunter, TSubclassOf<APartner> SpawnPartnerClass, const FVector& SpawnLoc)
{
	APartner* NewPartner = GetWorld()->SpawnActor<APartner>(SpawnPartnerClass, SpawnLoc, FRotator::ZeroRotator);
	OwnerHunter->Partner = NewPartner;
	NewPartner->Hunter = OwnerHunter;
	NewPartner->CurState = EPartnerState::MoveTarget;
	//NewPartner->MultiSetHunter(OwnerHunter);
	//MultiSetPartner(NewPartner);

}

void AHunter::MultiSetPartner_Implementation (APartner* NewPartner)
{
	SetPartner(NewPartner);
}

void AHunter::ServerUsePotion_Implementation(AHunter* OwnerHunter, TSubclassOf<AItem> SpawnItemClass)
{
	APotion* Potion = GetWorld()->SpawnActor<APotion>(SpawnItemClass,FVector(0,0,0), FRotator::ZeroRotator);
	Potion->MultiAttachPotion(OwnerHunter);
	MultiPlayMontage(OwnerHunter, FName("Drink"));
	MultiUsePotion(Potion);
}

void AHunter::MultiUsePotion_Implementation(APotion* Potion)
{
	CurItem = Potion;
	bUpperOnly = true;
	CurState = EPlayerState::Drink;
}

void AHunter::ServerSpawnBullet_Implementation(AHunter* OwnerHunter, TSubclassOf<AItem> SpawnItemClass, FVector StartLoc, FVector EndLoc, FRotator Rotation)
{
	ABullet* Bullet = GetWorld()->SpawnActor<ABullet>(SpawnItemClass, StartLoc, Rotation);
	Bullet->StaticMesh->SetCollisionProfileName(FName("Bullet"), true);
	OwnerHunter->CurItem = Bullet;
	OwnerHunter->bUpperOnly = true;
	Bullet->MultiLaunchBullet(OwnerHunter, StartLoc, EndLoc);
	MultiPlayMontage(OwnerHunter, FName("Shot"));
}

void AHunter::ServerSpawnItem_Implementation(AHunter* OwnerHunter, TSubclassOf<AItem> SpawnItemClass, FVector StartLoc, FVector EndLoc, FRotator Rotation)
{
	UE_LOG(LogTemp, Warning, TEXT("spawn item"));

	AItem* SpawnedItem = GetWorld()->SpawnActor<AItem>(SpawnItemClass, StartLoc, FRotator::ZeroRotator);
	switch (SpawnedItem->ItemType)
	{
	case EItemType::Bullet:
		SpawnedItem->Destroy();
		break;
	case EItemType::Potion:
		MultiPlayMontage(OwnerHunter, FName("Drink"));

		break;
	case EItemType::Trap:
		MultiPlayMontage(OwnerHunter, FName("Install"));
		FHitResult* HitResult = new FHitResult();
		FVector SpawnLoc = StartLoc + OwnerHunter->GetActorForwardVector() * 200;
		if (GetWorld()->LineTraceSingleByChannel(*HitResult, SpawnLoc, SpawnLoc + FVector(0, 0, -100), ECollisionChannel::ECC_Pawn))
		{
			SpawnedItem->SetActorLocation(HitResult->Location);
			OwnerHunter->CurState = EPlayerState::Install;
			SpawnedItem->UseItem(OwnerHunter);
		}
		else
		{
			//실패 처리 필요
			SpawnedItem->Destroy();
		}
		break;
	}
}

void AHunter::ServerUsePartnerNormalSkill_Implementation(APartner* MyPartner,ESkillID SkillID)
{
	MyPartner->MultiUseNormalSkill(SkillID);
}

void AHunter::ServerUsePartnerSpecialSkill_Implementation(APartner* MyPartner, ESkillID SkillID)
{
	MyPartner->MultiUseSpecialSkill(SkillID);
}

void AHunter::ServerShotBullet_Implementation(ABullet* Bullet, AHunter* OwnerHunter, FVector InitialPos, FVector EndPos)
{
	Bullet->MultiLaunchBullet(OwnerHunter, InitialPos, EndPos);
}

void AHunter::UpdateQuickSlot_Implementation()
{
}

void AHunter::ServerCancelOrder_Implementation(APartner* MyPartner)
{
	MyPartner->MultiCancelOrder();
}

void AHunter::SetStamina(float setStamina)
{
	if (GetLocalRole() == ROLE_Authority)
	{
		HunterStamina = setStamina;
	}
}

void AHunter::OnRep_Material()
{
	FSoftObjectPath MaterialPath{};
	switch (MaterialIndex)
	{
	case 1:
		MaterialPath = TEXT("/Game/Hunter/Asset/Hunter/M_Hunter");
		break;
	case 2:
		MaterialPath = TEXT("/Game/Hunter/Asset/Hunter/M_Hunter02");
		break;
	default:
		MaterialPath = TEXT("/Game/Hunter/Asset/Hunter/M_Hunter");
		break;
	}
	Material = Cast<UMaterialInterface>(MaterialPath.TryLoad());
	GetMesh()->SetMaterial(0, Material);
}

void AHunter::ServerChangeMaterialIndex_Implementation(int32 NewMaterialIndex)
{
	FSoftObjectPath MaterialPath{};
	switch (NewMaterialIndex)
	{
	case 1:
		MaterialPath = TEXT("/Game/Hunter/Asset/Hunter/M_Hunter");
		break;
	case 2:
		MaterialPath = TEXT("/Game/Hunter/Asset/Hunter/M_Hunter02");
		break;
	default:
		MaterialPath = TEXT("/Game/Hunter/Asset/Hunter/M_Hunter");
		break;
	}
	Material = Cast<UMaterialInterface>(MaterialPath.TryLoad());
	GetMesh()->SetMaterial(0, Material);

	MulticastChangeMaterialIndex(NewMaterialIndex);
}

void AHunter::MulticastChangeMaterialIndex_Implementation(int32 NewMaterialIndex)
{
	FSoftObjectPath MaterialPath{};
	switch (NewMaterialIndex)
	{
	case 1:
		MaterialPath = TEXT("/Game/Hunter/Asset/Hunter/M_Hunter");
		break;
	case 2:
		MaterialPath = TEXT("/Game/Hunter/Asset/Hunter/M_Hunter02");
		break;
	default:
		MaterialPath = TEXT("/Game/Hunter/Asset/Hunter/M_Hunter");
		break;
	}
	Material = Cast<UMaterialInterface>(MaterialPath.TryLoad());
	GetMesh()->SetMaterial(0, Material);
}