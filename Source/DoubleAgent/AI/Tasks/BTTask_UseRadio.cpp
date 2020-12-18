// Please don't steal


#include "BTTask_UseRadio.h"

EBTNodeResult::Type UBTTask_UseRadio::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, FString::Printf(TEXT("%s used the radio with %s status at %s"),  *AActor::GetDebugName(OwnerComp.GetOwner()), *UEnum::GetDisplayValueAsText(NewRadioEvent).ToString(), *OwnerComp.GetOwner()->GetActorLocation().ToString()));
    return EBTNodeResult::Succeeded;
}
