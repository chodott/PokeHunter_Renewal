// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_TurnToTarget.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Partner.h"
#include "PartnerController.h"

UBTTask_TurnToTarget::UBTTask_TurnToTarget()
{
	NodeName = TEXT("TurnToTarget");
}

EBTNodeResult::Type UBTTask_TurnToTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	APartner* Partner = Cast<APartner>(OwnerComp.GetAIOwner()->GetPawn());
	if (Partner == nullptr) return EBTNodeResult::Failed;
	FVector TargetPos = OwnerComp.GetBlackboardComponent()->GetValueAsVector(TEXT("TargetPos"));
	FVector LookVec = TargetPos - Partner->GetActorLocation();
	FRotator Rot = FRotationMatrix::MakeFromX(LookVec).Rotator();
	Partner->SetActorRotation(FMath::RInterpTo(Partner->GetActorRotation(), Rot, GetWorld()->GetDeltaSeconds(), 1.0f));

	return EBTNodeResult::Succeeded;
}