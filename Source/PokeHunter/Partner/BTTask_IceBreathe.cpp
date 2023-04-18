// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_IceBreathe.h"
#include "WolfPartner.h"
#include "PartnerController.h"

UBTTask_IceBreathe::UBTTask_IceBreathe()
{
	bNotifyTick = true;
	NodeName = TEXT("IceBreathe");
}

EBTNodeResult::Type UBTTask_IceBreathe::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);
	AWolfPartner* Wolf = Cast<AWolfPartner>(OwnerComp.GetAIOwner()->GetPawn());
	if (Wolf == NULL)return EBTNodeResult::Failed;
	if (Wolf->CurState != EPartnerState::IceBreath) return EBTNodeResult::Failed;
	bPlaying = true;
	Wolf->IceBreathe();

	Wolf->OnMontageEnd.AddLambda([this]()->void
		{
			bPlaying = false;
		});

	return EBTNodeResult::Type::InProgress;
}

void UBTTask_IceBreathe::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	if (!bPlaying)
	{
		APartner* Partner = Cast<APartner>(OwnerComp.GetAIOwner()->GetPawn());
		Partner->StopSkill();
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}
