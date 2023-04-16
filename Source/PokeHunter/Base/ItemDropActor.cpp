// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemDropActor.h"
#include "PokeHunter/Hunter/Hunter.h"
#include "PokeHunter/Hunter/InventoryComponent.h"

AItemDropActor::AItemDropActor()
{
	InteractionSphere->SetSphereRadius(80.f);
	PrimaryActorTick.bCanEverTick = true;
}

void AItemDropActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bInteracting)
	{
		FVector CurPointVec = CalculatePoint(DeltaTime);
		SetActorLocation(CurPointVec);

		if (RunningTime >= TotalTime)
		{
			DropCnt = FMath::RandRange(1, 5);
			for (int i = 0; i < DropCnt; ++i)
			{
				int RandIndex = FMath::RandRange(0, DropItemArray.Num() - 1);
				DropItemArray[RandIndex].cnt += 1;
			}

			for (int i = 0; i < DropItemArray.Num() - 1; ++i)
			{
				bool bAddSuccess = Master->Inventory->AddItemData(DropItemArray[i]);
				if (!bAddSuccess)
				{
					return;
				}

			}
			//획득 처리 모션 추가 필요
			this->Destroy();
			return;
		}
	}
}

void AItemDropActor::BeginPlay()
{
	Super::BeginPlay();

	//�����ڿ��� �����۵����� Ŭ���� �߰� �� �����ڿ��� ����
	int32 RaritySum = 0;
	for (auto& DataMap : DropItemMap)
	{
		RaritySum += DataMap.Key;
	}
	if (RaritySum != 0) BaseProbability = int32(100 / RaritySum);
}

void AItemDropActor::CreateItemArray(TArray<FName>& ItemArray)
{
	for (int i = 0; i < ItemArray.Num() - 1; ++i)
	{
		FItemCnter ItemCnter;
		ItemCnter.ItemID = ItemArray[i];
		DropItemArray.Add(ItemCnter);
	}
	
}

FVector AItemDropActor::CalculatePoint(float DeltaTimes)
{
	FVector EndPointVec = Master->GetActorLocation();

	float RemainTime = TotalTime - RunningTime;

	FVector CurPointVec = RemainTime * RemainTime * StartPointVec + 2 * RemainTime * RunningTime * TurningPointVec + RunningTime * RunningTime * EndPointVec;

	RunningTime += DeltaTimes;

	return CurPointVec;
}

void AItemDropActor::Interact_Implementation(AHunter* Hunter)
{
	Master = Hunter;

	StartPointVec = GetActorLocation();
	TurningPointVec = Master->GetActorLocation() + FVector(0,100,50);

	bInteracting = true;
	StaticMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SetActorTickEnabled(true);
	

	//DropCnt =  FMath::RandRange(0, 100);

	//for (int32 i = 0; i < DropCnt; ++i)
	//{
	//	int32 Probability = FMath::RandRange(0, 100);
	//	int32 StartProbability = 0;
	//	
	//	//아이템 데이터 클래스
	//	for (auto& DropItem : DropItemMap)
	//	{
	//		if (Probability >= StartProbability && Probability < StartProbability + BaseProbability * DropItem.Key)
	//		{
	//			//���� ���ϱ�
	//			int32 ItemCnt = FMath::RandRange(1, 5);
	//		bool bAddSuccess = Master->Inventory->AddItemData(DropItem.Value, ItemCnt);
	//			if (bAddSuccess) 
	//			{ 
	//				this->Destroy();
	//				return;
	//			}
	//			else {
	//				//Full Inventory
	//				return;
	//			}
	//		}
	//		StartProbability += BaseProbability * DropItem.Key;
	//	}
	//}

	
}
