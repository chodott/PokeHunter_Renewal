// Fill out your copyright notice in the Description page of Project Settings.


#include "PartnerBTService.h"
#include "PartnerController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "PokeHunter/Hunter/Hunter.h"
#include "Partner.h"

void UPartnerBTService::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	APartner* Partner = Cast<APartner>(OwnerComp.GetAIOwner()->GetPawn());
	OwnerComp.GetBlackboardComponent()->SetValueAsObject(FName("Hunter"),Partner->Hunter);
}