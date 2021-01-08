// Please don't steal


#include "BTTask_Speak.h"
#include "Perception/AISense_Hearing.h"
#include "BehaviorTree/BlackboardComponent.h"

EBTNodeResult::Type UBTTask_Speak::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	//Save NPC and blackboard
	APawn* NPC = Cast<AAIController>(OwnerComp.GetOwner())->GetPawn();
	UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();

	//If new action status is not idle
	if (NewActionStatus != EActionStatus::Action_Idle)
	{
		//Raise vocal status
		Cast<AStaffAIController>(OwnerComp.GetOwner())->RaiseVocalStatus(NewVocalStatus);
		
		//Set action status
		Blackboard->SetValueAsEnum("ActionStatus", static_cast<uint8>(NewActionStatus));

		//If someone has already spoken nearby recently
		if (IsValid(Blackboard->GetValueAsObject("Speaker")))
			return EBTNodeResult::Succeeded;
		
		//Register speech event
		UAISense_Hearing::ReportNoiseEvent(GetWorld(), NPC->GetActorLocation(), 1, NPC, 0, FName("Speech"));
		
		//Print to log and screen new action status
		UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("%s spoke with status: %s"), *NPC->GetDebugName(NPC), *UEnum::GetDisplayValueAsText(NewActionStatus).ToString()), true, true, FLinearColor::Red, 10.0f);
		
		return EBTNodeResult::Succeeded;
	}
	
	//If vocal status was raised
	if (Cast<AStaffAIController>(OwnerComp.GetOwner())->RaiseVocalStatus(NewVocalStatus))
	{
		//If someone has already spoken nearby recently
		if (IsValid(Blackboard->GetValueAsObject("Speaker")))
			return EBTNodeResult::Succeeded;
		
		//Register speech event
		UAISense_Hearing::ReportNoiseEvent(GetWorld(), NPC->GetActorLocation(), 1, NPC, 0, FName("Speech"));

		//Print to log and screen new vocal status
		UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("%s spoke with status: %s"), *NPC->GetDebugName(NPC), *UEnum::GetDisplayValueAsText(NewVocalStatus).ToString()), true, true, FLinearColor::Red, 10.0f);
	}

	return EBTNodeResult::Succeeded;
}
