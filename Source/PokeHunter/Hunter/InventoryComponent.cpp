// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryComponent.h"
#include "PokeHunter/Item/ItemData.h"
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
	Super::BeginPlay();
	for (int i = 0; i < capacity; i++)
	{
		ItemArray.AddDefaulted();
	}
	// ...
	
}

bool UInventoryComponent::AddItem(const AItem* Item)
{
	int NullNum = -1;
	for (int i = 0; i < capacity; ++i)
	{
		if (NullNum == -1 && ItemArray[i] == NULL) NullNum = i;
		else if (ItemArray[i] != NULL)
		{
			if (ItemArray[i]->ItemName == Item->Name) //아이템이 겹칠 경우
			{
				ItemArray[i]->ItemCount++;
				return true;
			}
		}
	}


	if (NullNum != -1) //이미 존재하는 아이템이 없을 경우
	{
		auto ItemData = NewObject<UItemData>(this, UItemData::StaticClass(), TEXT("PLEASE"));
		ItemData->SetItemData(Item);
		ItemArray[NullNum] = ItemData;
		return true;
	}

	//가방이 가득 찬 경우
	return false;
}

void UInventoryComponent::ChangeSlot(FName TargetName, int TargetIndex, FName GoalName, int GoalIndex)
{
	UItemData* Temp = NULL;
	ANpcStorage* StorageNpc;
	if (TargetName == "Inventory")
	{
		Temp = ItemArray[TargetIndex];
		if (GoalName == "Inventory")
		{
			ItemArray[TargetIndex] = ItemArray[GoalIndex];
			ItemArray[GoalIndex] = Temp;
		}
		else 
		{
			StorageNpc = Cast<ANpcStorage>(Hunter->InteractingActor);
			if (StorageNpc)
			{
				ItemArray[TargetIndex] = StorageNpc->Storage->ItemArray[GoalIndex];
				StorageNpc->Storage->ItemArray[GoalIndex] = Temp;
			}
		}
	}

	else if (TargetName == "Storage")
	{
		StorageNpc = Cast<ANpcStorage>(Hunter->InteractingActor);

		if (StorageNpc)
		{
			Temp = StorageNpc->Storage->ItemArray[TargetIndex];

			if (GoalName == "Inventory")
			{
				StorageNpc->Storage->ItemArray[TargetIndex] = ItemArray[GoalIndex];
				ItemArray[GoalIndex] = Temp;
			}
			else
			{
				StorageNpc->Storage->ItemArray[TargetIndex] = StorageNpc->Storage->ItemArray[GoalIndex];
				StorageNpc->Storage->ItemArray[GoalIndex] = Temp;
			}
		}
	}

}

// Called every frame
void UInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

