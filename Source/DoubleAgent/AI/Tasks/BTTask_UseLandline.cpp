// Please don't steal


#include "BTTask_UseLandline.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"

EBTNodeResult::Type UBTTask_UseLandline::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	//Get landline
	ALandline* Landline = Cast<ALandline>(OwnerComp.GetBlackboardComponent()->GetValue<UBlackboardKeyType_Object>(LandlineObject.GetSelectedKeyID()));

	//If landline is valid or calling for backup failed
	if (!IsValid(Landline) || !Landline->CallBackup())
		return EBTNodeResult::Failed;
	
	return EBTNodeResult::Succeeded;
}
