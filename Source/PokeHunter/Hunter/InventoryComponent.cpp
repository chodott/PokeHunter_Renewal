// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryComponent.h"
#include "PokeHunter/Item/Item.h"
#include "PokeHunter/Npc//NpcStorage.h"
#include "Chaos/Pair.h"

// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	bEditableWhenInherited = true;
	// ...

	InfoArray.Reserve(capacity);
	// FarmingArray.Reserve(capacity);
}


// Called when the game starts
void UInventoryComponent::BeginPlay()
{
	//아이템 배열(클래스) 초기화
	Super::BeginPlay();
	int ArrayLength;
	if (true == InfoArray.IsEmpty())	ArrayLength = 0;
	else								ArrayLength = InfoArray.Num();

	for (int i = ArrayLength; i < capacity; i++)
	{
		InfoArray.AddDefaulted();
	}

}

bool UInventoryComponent::AddItemData(TArray<FItemCnter>& ItemCnterArray)
{
	TMap<FName, TArray<int>> IndexMap;
	bool bInventoryFull = false;
	bool bAddAllItem = true;
	IndexMap.Add("None", {});

	for (int i = 0; i < capacity; ++i)
	{
		FName& ItemName = InfoArray[i].ItemID;
		if (IndexMap.Find(ItemName) == nullptr) IndexMap.Add(ItemName, { i });
		else IndexMap[ItemName].Emplace(i);
	}
	bInventoryFull = IndexMap["None"].IsEmpty();

	for (auto& Item : ItemCnterArray)
	{
		FName& ItemName = Item.ItemID;
		if (IndexMap.Contains(ItemName) == false)
		{
			if (bInventoryFull)
			{
				bAddAllItem = false;
				continue;
			}
			int index = IndexMap["None"][0];
			InfoArray[index] = Item;
		}
		else
		{
			int index = IndexMap[ItemName][0];
			InfoArray[index].cnt += Item.cnt;
		}
	}
	return bAddAllItem;
}


void UInventoryComponent::ChangeSlot(UInventoryComponent* otherInventory, int thisIndex, int otherIndex)
{
	FItemCnter temp;
	temp = InfoArray[thisIndex];
	InfoArray[thisIndex] = otherInventory->InfoArray[otherIndex];
	otherInventory->InfoArray[otherIndex] = temp;
}

int UInventoryComponent::GetItemCnt(FName id)
{
	int cnt = 0;
	for (auto cnter : InfoArray)
	{
		if (cnter.ItemID == id)
		{
			cnt += cnter.cnt;
		}
	}
	return cnt;
}

bool UInventoryComponent::CreateItem(const FItemInfo& ItemInfo)
{
	TMap<FName, TArray<int>> IndexMap;
	for (int i = 0; i < InfoArray.Num(); ++i)
	{
		FName ItemName = InfoArray[i].ItemID;
		if (IndexMap.Find(ItemName) == nullptr) IndexMap.Add(ItemName, { i });
		else IndexMap[ItemName].Emplace(i);
	}

	for (int i = 0; i < ItemInfo.OfferingItemID.Num(); ++i)
	{	//필요한 아이템 수 체크
		FName CreateItemID = ItemInfo.OfferingItemID[i];
		int NeedItemCnt = ItemInfo.OfferingItemCnt[i];
		int CurCnt = 0;
		for (int Index : IndexMap[CreateItemID])
		{
			CurCnt += InfoArray[Index].cnt;
		}

		if (CurCnt < NeedItemCnt)
		{
			return false;
		}
	}

	//개수 처리
	for (int i = 0; i < ItemInfo.OfferingItemID.Num(); ++i)
	{
		FName CreateItemID = ItemInfo.OfferingItemID[i];
		int NeedItemCnt = ItemInfo.OfferingItemCnt[i];
		for (int Index : IndexMap[CreateItemID])
		{
			FItemCnter& ItemCnter = InfoArray[Index];
			if (NeedItemCnt > ItemCnter.cnt)
			{
				NeedItemCnt -= ItemCnter.cnt;
				ItemCnter.cnt = 0;
			}
			else
			{
				ItemCnter.cnt -= NeedItemCnt;
				NeedItemCnt = 0;
			}
		}
	}
	return true;
}


// Called every frame
void UInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

