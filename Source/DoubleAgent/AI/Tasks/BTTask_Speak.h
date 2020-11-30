// Please don't steal

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "DoubleAgent/AI/NPC/AIControllerBase.h"
#include "UObject/ObjectMacros.h"
#include "BTTask_Speak.generated.h"

/**
 * 
 */
UCLASS()
class DOUBLEAGENT_API UBTTask_Speak : public UBTTaskNode
{
	GENERATED_BODY()

    UPROPERTY(EditAnywhere)
    EVocalStatus NewVocalStatus;

public:
    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
