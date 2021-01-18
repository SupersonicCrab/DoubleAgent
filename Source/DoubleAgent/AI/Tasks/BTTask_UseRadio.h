// Please don't steal

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "DoubleAgent/Radio.h"
#include "UObject/ObjectMacros.h"


#include "BTTask_UseRadio.generated.h"

/**
 * 
 */
UCLASS()
class DOUBLEAGENT_API UBTTask_UseRadio : public UBTTaskNode
{
	GENERATED_BODY()

    UPROPERTY(EditAnywhere)
    FRadioEvent NewRadioEvent;

	UPROPERTY(EditAnywhere)
	bool bUseBlackboardLocation;

	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector Location;
    
public:
    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
