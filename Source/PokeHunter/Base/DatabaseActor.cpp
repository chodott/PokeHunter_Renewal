// Fill out your copyright notice in the Description page of Project Settings.


#include "DatabaseActor.h"

// Sets default values
ADatabaseActor::ADatabaseActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ADatabaseActor::BeginPlay()
{
	Super::BeginPlay();

	for (auto& ItemDataClass : ItemDataClassMap)
	{
		auto ItemData = NewObject<UItemData>(this, ItemDataClass.Value);
		ItemDataObjectMap.Add(ItemDataClass.Key, ItemData);
	}

	for (auto& SkillInfo : SkillInfoArray)
	{
		auto SkillData = NewObject<USkillData>(this);
		SkillData->SkillInfo = SkillInfo;
		SkillDataObjectMap.Add(SkillInfo.ID, SkillData);
	}
	
}

// Called every frame
void ADatabaseActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

UItemData* ADatabaseActor::FindItem(FName ItemID)
{
	if (ItemID == "None") return NULL;
	UItemData* ItemData = *ItemDataObjectMap.Find(ItemID);
	if (ItemData != nullptr) 
		return ItemData;
	else return NULL;
}

FSkillInfo ADatabaseActor::FindSkill(FString SkillID)
{
	for (auto Skill : SkillInfoArray)
	{
		if (SkillID == Skill.ID)
		{
			return Skill;
		}
	
	}

	FSkillInfo info;
	return info;

}

