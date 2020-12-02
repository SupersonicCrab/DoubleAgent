// Please don't steal

#pragma once

#include "CoreMinimal.h"
#include "FSMState.h"
#include "DoubleAgent/AI/NPC/AIControllerBase.h"
#include "FSMController.generated.h"

/**
 * 
 */
UCLASS()
class DOUBLEAGENT_API AFSMController : public AAIControllerBase
{
	GENERATED_BODY()
public:
	FSMState CurrentState;

	void Update();
};
