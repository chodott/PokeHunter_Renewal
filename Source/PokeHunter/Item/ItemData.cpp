// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemData.h"
#include "Item.h"

UItemData::UItemData()
{
	ItemCount = 1;
	ItemName = "None";
	ItemIcon = NULL;
	ItemClass = NULL;

}

void UItemData::SetItemData(const AItem* Item)
{
	ItemClass = Item->GetClass();
	ItemIcon = Item->Icon;
	ItemName = Item->Name;
}