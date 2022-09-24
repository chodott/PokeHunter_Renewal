// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Npc.h"
#include "NpcInterface.h"
#include "NpcStorage.generated.h"

UCLASS()
class POKEHUNTER_API ANpcStorage : public ANpc
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ANpcStorage();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh")
		UStaticMeshComponent* SubStaticMesh;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void interact_Implementation() override;

public:
	float currentAngle;
	bool bActive;

};

