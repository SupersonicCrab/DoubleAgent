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

	UBTTask_UseRadio();
	
    UPROPERTY(EditAnywhere)
    FRadioEvent NewRadioEvent;

	UPROPERTY(EditAnywhere)
	bool bUseBlackboardLocation;

	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector Location;
    
public:
    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
