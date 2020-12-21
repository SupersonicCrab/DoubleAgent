// Please don't steal

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "Chaos/AABBTree.h"
#include "DoubleAgent/AI/NPC/AIControllerBase.h"
#include "BTTask_RaiseDetection.generated.h"

/**
 * 
 */
UCLASS()
class DOUBLEAGENT_API UBTTask_RaiseDetection : public UBTTaskNode
{
	GENERATED_BODY()

	UBTTask_RaiseDetection(){};
	
	UPROPERTY(EditAnywhere)
	float NewDetection;
	
public:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override
	{
		//Call helper function
		Cast<AAIControllerBase>(OwnerComp.GetOwner())->RaiseDetection(NewDetection);
		
		return EBTNodeResult::Succeeded;
	};
};
