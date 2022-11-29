// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryComponent.h"
#include "PokeHunter/Item/Item.h"
#include "PokeHunter/Npc//NpcStorage.h"
#include "Hunter.h"

// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	bEditableWhenInherited = true;
	// ...
}


// Called when the game starts
void UInventoryComponent::BeginPlay()
{
	//아이템 배열(클래스) 초기화
	Super::BeginPlay();
	for (int i = 0; i < capacity; i++)
	{
		//아이템 class
		ItemArray.AddDefaulted();
		//아이템 struct
		InfoArray.AddDefaulted();
	}
	
}

bool UInventoryComponent::AddItemData(FName ItemName, int Cnt)
{
	int NullNum = -1;
	for (int i = 0; i < capacity; ++i)
	{
		if (NullNum == -1 && InfoArray[i].ItemID == FName("None")) NullNum = i;
		else if (InfoArray[i].ItemID != FName("None"))
		{
			if (InfoArray[i].ItemID == ItemName) 
			{
				//Add ItemCnt Update need
				InfoArray[i].cnt += Cnt;
				return true;
			}
		}
	}

	if (NullNum != -1)
	{
		//Find Object Need
		InfoArray[NullNum].ItemID = ItemName;
		InfoArray[NullNum].cnt += Cnt;
		return true;
	}

	return false;
}

bool UInventoryComponent::AddItemInfo(FName ItemID, int Cnt)
{
	int NullNum = -1;
	for (int i = 0; i < capacity; ++i)
	{
		if (NullNum == -1 && InfoArray[i].ItemID.IsNone()) NullNum = i;
		else if (!InfoArray[i].ItemID.IsNone())
		{
			if (InfoArray[i].ItemID == ItemID)
				{
				InfoArray[i].cnt += Cnt;
				return true;
				}
		}
	}

	if (NullNum != -1)
	{
		InfoArray[NullNum].ItemID = ItemID;
		InfoArray[NullNum].cnt = Cnt;
		return true;
	}

	return false;
}


void UInventoryComponent::ChangeSlot(FName TargetName, int TargetIndex, FName GoalName, int GoalIndex)
{
	ANpcStorage* StorageNpc;
	FItemCnter temp;
	if (TargetName == "Inventory")
	{
		temp = InfoArray[TargetIndex];
		if (GoalName == "Inventory")
		{
			InfoArray[TargetIndex] = InfoArray[GoalIndex];
			InfoArray[GoalIndex] = temp;
		}
		else 
		{
			StorageNpc = Cast<ANpcStorage>(Hunter->InteractingActor);
			if (StorageNpc)
			{
				InfoArray[TargetIndex] = StorageNpc->Storage->InfoArray[GoalIndex];
				StorageNpc->Storage->InfoArray[GoalIndex] = temp;
			}
		}
	}

	else if (TargetName == "Storage")
	{
		StorageNpc = Cast<ANpcStorage>(Hunter->InteractingActor);

		if (StorageNpc)
		{
			temp = StorageNpc->Storage->InfoArray[TargetIndex];

			if (GoalName == "Inventory")
			{
				StorageNpc->Storage->InfoArray[TargetIndex] = InfoArray[GoalIndex];
				InfoArray[GoalIndex] = temp;
			}
			else
			{
				StorageNpc->Storage->InfoArray[TargetIndex] = StorageNpc->Storage->InfoArray[GoalIndex];
				StorageNpc->Storage->InfoArray[GoalIndex] = temp;
			}
		}
	}

}

void UInventoryComponent::SwapSlot(int TargetIndex, int GoalIndex)
{
	FItemCnter Temp = InfoArray[TargetIndex];
	InfoArray[TargetIndex] = InfoArray[GoalIndex];
	InfoArray[GoalIndex] = Temp;
}


// Called every frame
void UInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

