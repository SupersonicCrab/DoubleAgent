// Please don't steal

#pragma once

#include "CoreMinimal.h"
#include "DoubleAgent/Radio.h"
#include "DoubleAgent/Power/PowerComponent.h"
#include "RadioHub.generated.h"

/**
 * 
 */
UCLASS()
class DOUBLEAGENT_API ARadioHub : public APowerComponent
{
	GENERATED_BODY()

    FRadioEvent RadioEvent;
public:
    FTimerHandle SpeechTimer;
    
    bool StartRadioEvent(FRadioEvent RadioEvent_);
    UFUNCTION()
    void BroadcastRadioEvent();
};
