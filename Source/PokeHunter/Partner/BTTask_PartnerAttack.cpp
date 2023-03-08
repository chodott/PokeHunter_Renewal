// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_PartnerAttack.h"
#include "Partner.h"
#include "PartnerController.h"

UBTTask_PartnerAttack::UBTTask_PartnerAttack()
{
	bNotifyTick = true;
}

EBTNodeResult::Type UBTTask_PartnerAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);
	APartner* Partner = Cast<APartner>(OwnerComp.GetAIOwner()->GetPawn());
	if (Partner == NULL)return EBTNodeResult::Failed;
	Partner->Attack();
	bPlaying = true;

	Partner->OnMontageEnd.AddLambda([this]()->void
		{
			bPlaying = false;
		});

	return EBTNodeResult::Type::InProgress;
}

void UBTTask_PartnerAttack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	if (!bPlaying)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}

