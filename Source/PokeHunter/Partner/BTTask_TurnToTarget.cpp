// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_TurnToTarget.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Partner.h"
#include "PartnerController.h"

UBTTask_TurnToTarget::UBTTask_TurnToTarget()
{
	bNotifyTick = true;
	NodeName = TEXT("TurnToTarget");
}

EBTNodeResult::Type UBTTask_TurnToTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	return EBTNodeResult::InProgress;
}


void UBTTask_TurnToTarget::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	APartner* Partner = Cast<APartner>(OwnerComp.GetAIOwner()->GetPawn());
	if (Partner == nullptr) FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
	FVector TargetPos = OwnerComp.GetBlackboardComponent()->GetValueAsVector(TEXT("TargetPos"));
	FVector LookVec = TargetPos - Partner->GetActorLocation();

	if (Partner->CurState == EPartnerState::Posing || Partner->CurState == EPartnerState::Unselected)
	{
		LookVec = Partner->LookTargetVec;
	}

	else if (Partner->CurState == EPartnerState::Unselected)
	{
		LookVec = Partner->LookTargetVec;
	}
	FRotator Rot = FRotationMatrix::MakeFromX(LookVec).Rotator();

	Partner->SetActorRotation(FMath::RInterpTo(Partner->GetActorRotation(), Rot, GetWorld()->GetDeltaSeconds(), 5.f));
	if (Partner->GetActorForwardVector() == Partner->LookTargetVec)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}