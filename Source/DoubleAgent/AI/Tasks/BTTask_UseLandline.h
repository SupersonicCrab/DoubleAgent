// Please don't steal

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "Chaos/AABBTree.h"
#include "DoubleAgent/Security/Landline.h"
#include "BTTask_UseLandline.generated.h"

/**
 * 
 */
UCLASS()
class DOUBLEAGENT_API UBTTask_UseLandline : public UBTTaskNode
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector LandlineObject;
public:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
