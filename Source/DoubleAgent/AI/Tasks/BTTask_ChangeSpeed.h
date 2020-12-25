// Please don't steal

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "UObject/ObjectMacros.h"
#include "BTTask_ChangeSpeed.generated.h"

UENUM()
enum class EMoveSpeed : uint8
{
	Speed_Idle UMETA(DisplayName="Idle"),
	Speed_Walk UMETA(DisplayName="Walking"),
	Speed_Jog UMETA(DisplayName="Jogging"),	
	Speed_Run UMETA(DisplayName="Running"),	
};

/**
 * 
 */
UCLASS()
class DOUBLEAGENT_API UBTTask_ChangeSpeed : public UBTTaskNode
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	EMoveSpeed NewSpeed;
	
public:
    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
