// Please don't steal

#pragma once

#include "CoreMinimal.h"
#include "StaffAIController.h"
#include "ButlerAI.generated.h"

/**
 * 
 */
UCLASS()
class DOUBLEAGENT_API AButlerAI : public AStaffAIController
{
	GENERATED_BODY()

	AButlerAI(){};

	virtual bool HandleHearing(AActor* CurrentActor, FAIStimulus& CurrentStimulus) override;
};
