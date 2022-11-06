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

	//생성자에서 아이템데이터 클래스 추가 시 생성자에서 진행
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

	//랜덤한 횟수로 배열 인덱스 접근
	//랜덤한 인덱스 아이템 데이터 클래스 전달
	//랜덤한 아이템 개수 전달

	DropCnt =  FMath::RandRange(0, 100);

	for (int32 i = 0; i < DropCnt; ++i)
	{
		int32 Probability = FMath::RandRange(0, 100);
		int32 StartProbability = 0;
		for (auto& DataMap : ItemDataClassMap)
		{
			if (Probability >= StartProbability && Probability < StartProbability + BaseProbability * DataMap.Key)
			{
				//개수 정하기
				int32 ItemCnt = FMath::RandRange(1, 5);
				bool bAddSuccess = Master->Inventory->AddItemData(DataMap.Value, ItemCnt);
				if (bAddSuccess) 
				{ 
					this->Destroy(); 
					return;
				}
				else {
					//습득 실패 - 창 띄우기
					return;
				}
			}
			StartProbability += BaseProbability * DataMap.Key;
		}
	}
}
