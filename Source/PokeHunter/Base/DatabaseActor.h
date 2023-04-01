// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PokeHunter/Item/ItemData.h"
#include "PokeHunter/Base/SkillData.h"
#include "DatabaseActor.generated.h"

UCLASS()
class POKEHUNTER_API ADatabaseActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADatabaseActor();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ItemData")
		TMap<FName, TSubclassOf<class UItemData>> ItemDataClassMap;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ItemData")
		TMap<FName, class UItemData*> ItemDataObjectMap;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "SkillData")
		TArray<FSkillInfo> SkillInfoArray;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ItemData")
		TMap<ESkillID, class USkillData*> SkillDataObjectMap;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	UItemData* FindItem(FName ItemID);

	UFUNCTION(BlueprintCallable)
	FSkillInfo FindSkill(ESkillID SkillID);

	
	void DeepCopyMap() {

	}
};
