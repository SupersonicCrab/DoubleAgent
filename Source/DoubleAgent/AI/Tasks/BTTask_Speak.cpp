// Please don't steal


#include "BTTask_Speak.h"
#include "Perception/AISense_Hearing.h"
#include "BehaviorTree/BlackboardComponent.h"

EBTNodeResult::Type UBTTask_Speak::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    APawn* NPC = Cast<AAIController>(OwnerComp.GetOwner())->GetPawn();
    UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
    Blackboard->SetValueAsEnum("VocalStatus", static_cast<uint8>(NewVocalStatus));
    if (NewActionStatus != EActionStatus::Action_Idle)
        Blackboard->SetValueAsEnum("ActionStatus", static_cast<uint8>(NewActionStatus));

    UAISense_Hearing::ReportNoiseEvent(GetWorld(), NPC->GetActorLocation(), 1, NPC, 0, FName("Local"));
    GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, FString::Printf(TEXT("%s spoke with status: %s"), *NPC->GetDebugName(NPC), *UEnum::GetDisplayValueAsText(NewVocalStatus).ToString()));
    
    return EBTNodeResult::Succeeded;
}
