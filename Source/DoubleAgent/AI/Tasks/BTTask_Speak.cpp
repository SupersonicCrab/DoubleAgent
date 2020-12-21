// Please don't steal


#include "BTTask_Speak.h"
#include "Perception/AISense_Hearing.h"
#include "BehaviorTree/BlackboardComponent.h"

EBTNodeResult::Type UBTTask_Speak::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    //Save NPC and blackboard
    APawn* NPC = Cast<AAIController>(OwnerComp.GetOwner())->GetPawn();
    UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();

    //Register noise event
    UAISense_Hearing::ReportNoiseEvent(GetWorld(), NPC->GetActorLocation(), 1, NPC, 0, FName("Local"));

    //Raise vocal status
    Cast<AStaffAIController>(OwnerComp.GetOwner())->RaiseVocalStatus(NewVocalStatus);

    //If new action status is not idle
    if (NewActionStatus != EActionStatus::Action_Idle)
    {
        Blackboard->SetValueAsEnum("ActionStatus", static_cast<uint8>(NewActionStatus));
        //Print to log and screen new action status
        UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("%s spoke with status: %s"), *NPC->GetDebugName(NPC), *UEnum::GetDisplayValueAsText(NewActionStatus).ToString()), true, true, FLinearColor::Red, 10.0f);
    }

    //If no action status was set
    else
        UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("%s spoke with status: %s"), *NPC->GetDebugName(NPC), *UEnum::GetDisplayValueAsText(NewVocalStatus).ToString()), true, true, FLinearColor::Red, 10.0f);
    
    return EBTNodeResult::Succeeded;
}
