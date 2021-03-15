// Please don't steal

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTDecorator_IsUsingNavLink.generated.h"

/**
 * 
 */
UCLASS()
class DOUBLEAGENT_API UBTDecorator_IsUsingNavLink : public UBTDecorator
{
	GENERATED_BODY()

	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
};
