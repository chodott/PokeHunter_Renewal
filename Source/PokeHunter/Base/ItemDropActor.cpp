// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemDropActor.h"
#include "PokeHunter/Hunter/Hunter.h"
#include "PokeHunter/Hunter/InventoryComponent.h"

AItemDropActor::AItemDropActor()
{
	
}

void AItemDropActor::BeginPlay()
{
	Super::BeginPlay();

	//�����ڿ��� �����۵����� Ŭ���� �߰� �� �����ڿ��� ����
	int32 RaritySum = 0;
	for (auto& DataMap : ItemDataClassMap)
	{
		RaritySum += DataMap.Key;
	}
	if (RaritySum != 0) BaseProbability = int32(100 / RaritySum);
}

void AItemDropActor::Interact_Implementation(AHunter* Hunter)
{
	Master = Hunter;

	//������ Ƚ���� �迭 �ε��� ����
	//������ �ε��� ������ ������ Ŭ���� ����
	//������ ������ ���� ����

	DropCnt =  FMath::RandRange(0, 100);

	for (int32 i = 0; i < DropCnt; ++i)
	{
		int32 Probability = FMath::RandRange(0, 100);
		int32 StartProbability = 0;
		
		//아이템 데이터 클래스
		//for (auto& DataMap : ItemDataClassMap)
		//{
		//	if (Probability >= StartProbability && Probability < StartProbability + BaseProbability * DataMap.Key)
		//	{
		//		//���� ���ϱ�
		//		int32 ItemCnt = FMath::RandRange(1, 5);
		//		bool bAddSuccess = Master->Inventory->AddItemData(DataMap.Value, ItemCnt);
		//		if (bAddSuccess) 
		//		{ 
		//			this->Destroy(); 
		//			return;
		//		}
		//		else {
		//			//���� ���� - â ����
		//			return;
		//		}
		//	}
		//	StartProbability += BaseProbability * DataMap.Key;
		//}

		//아이템 데이터 구조체
		StartProbability = 0;
		for (auto& DataMap : ItemInfoMap)
		{
			if (Probability >= StartProbability && Probability < StartProbability + BaseProbability * DataMap.Key)
			{
				//���� ���ϱ�
				int32 ItemCnt = FMath::RandRange(1, 5);
				bool bAddSuccess = Master->Inventory->AddItemInfo(DataMap.Value, ItemCnt);
				if (bAddSuccess) 
				{ 
					this->Destroy(); 
					return;
				}
				else {
					//���� ���� - â ����
					return;
				}
			}
			StartProbability += BaseProbability * DataMap.Key;
		}
	}
}
