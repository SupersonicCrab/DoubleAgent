// Please don't steal


#include "BTTask_UseRadio.h"
#include "DoubleAgent/AI/NPC/AIControllerBase.h"
#include "DoubleAgent/SecuritySystem/RadioHub.h"
#include "Kismet/GameplayStatics.h"

EBTNodeResult::Type UBTTask_UseRadio::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    //Print to log and screen
    UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("%s used the radio with %s status at %s"),  *AActor::GetDebugName(OwnerComp.GetOwner()), *UEnum::GetDisplayValueAsText(NewRadioEvent.RadioEvent).ToString(), *OwnerComp.GetOwner()->GetActorLocation().ToString()), true, true, FLinearColor::Red);

    //Get radio hub
    ARadioHub* RadioHub = Cast<ARadioHub>(UGameplayStatics::GetActorOfClass(GetWorld(), ARadioHub::StaticClass()));

    //If no radio hub exists
    if (RadioHub == nullptr)
        return EBTNodeResult::Failed;

    //Update radio event structure
    NewRadioEvent.NPC = Cast<AAICharacterBase_CHARACTER>(Cast<AAIControllerBase>(OwnerComp.GetOwner())->GetPawn());
    NewRadioEvent.Location = OwnerComp.GetOwner()->GetActorLocation();

    //If radio hub does not have any power
    if (!RadioHub->StartRadioEvent(NewRadioEvent))
        return EBTNodeResult::Failed;

    return EBTNodeResult::Succeeded;

}