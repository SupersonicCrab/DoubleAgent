// Please don't steal

#include "BTDecorator_IsUsingNavLink.h"
#include "AIController.h"

bool UBTDecorator_IsUsingNavLink::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	return OwnerComp.GetAIOwner()->GetPathFollowingComponent()->IsCurrentSegmentNavigationLink();
}
