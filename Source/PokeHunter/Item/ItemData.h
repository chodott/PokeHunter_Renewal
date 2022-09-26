// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ItemData.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType, DefaultToInstanced, EditInlineNew, Abstract)
class POKEHUNTER_API UItemData : public UObject
{
	GENERATED_BODY()

public:

	UItemData();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 Index;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 ItemNum;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 ItemCount;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString ItemName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UTexture2D* Icon;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSubclassOf<class AItem> ItemClass;

	int32 GetIndex() 
	{
		return Index;
	}
	int32 GetItemNum() const
	{
		return ItemNum;
	}
	int32 GetItemCount() const
	{
		return ItemCount;
	}
	FString GetItemName() const
	{
		return ItemName;
	}
	UTexture2D* GetIcon() const
	{
		return Icon;
	}

	void SetItemIndex(int32 index)
	{
		Index = index;
	}
	void SetItemNum(int32 num)
	{
		ItemNum = num;
	}
	void SetItemCount(int32 cnt)
	{
		ItemCount = cnt;
	}
	void SetItemName(const FString& name)
	{
		ItemName = name;
	}
	void SetIcon(UTexture2D* texture)
	{
		Icon = texture;
	}

};
