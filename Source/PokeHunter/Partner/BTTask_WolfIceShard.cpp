// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_WolfIceShard.h"
#include "WolfPartner.h"
#include "PartnerController.h"

UBTTask_WolfIceShard::UBTTask_WolfIceShard()
{
	bNotifyTick = true;
	NodeName = TEXT("IceShard");
}

EBTNodeResult::Type UBTTask_WolfIceShard::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	//일정 범위 안의 enemy 클래스들에게 어그로 끌기
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);
	AWolfPartner* Wolf = Cast<AWolfPartner>(OwnerComp.GetAIOwner()->GetPawn());
	if (Wolf == NULL)return EBTNodeResult::Failed;

	bPlaying = true;
	Wolf->LaunchIceShard();



	Wolf->OnMontageEnd.AddLambda([this]()->void
		{
			bPlaying = false;
		});

	return EBTNodeResult::Type::InProgress;
}

void UBTTask_WolfIceShard::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	if (!bPlaying)
	{
		APartner* Partner = Cast<APartner>(OwnerComp.GetAIOwner()->GetPawn());
		Partner->StopSkill();
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}
