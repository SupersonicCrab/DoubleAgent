// Please don't steal

#pragma once

#include "CoreMinimal.h"
#include "DoubleAgent/AI/NPC/FSM/FSM.h"
#include "CivilianAI.generated.h"


/**
 * 
 */
UCLASS()
class DOUBLEAGENT_API ACivilianAI : public AFSMController
{
	GENERATED_BODY()
public:
    ACivilianAI();

	//Override base
	virtual void HandleSight(AActor* CurrentActor, FAIStimulus& CurrentStimulus) override;
};
