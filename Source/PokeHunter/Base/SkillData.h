// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "SkillData.generated.h"

/**
 * 
 */

UENUM(BlueprintType)
enum class ESkillID : uint8
{
	None = 0,
	Rush = 201,
	Howling = 202,
	Bress = 203
	
};

USTRUCT(BlueprintType)
struct FSkillInfo
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill")
	ESkillID ID {};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill")
	FName Name {};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill")
	FName Type {};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill")
	FText ToolTip {};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill")
	UTexture2D* Icon;
};


UCLASS(Blueprintable, BlueprintType)
class POKEHUNTER_API USkillData : public UObject
{
	GENERATED_BODY()
	
public:
	USkillData();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FSkillInfo SkillInfo;


};
