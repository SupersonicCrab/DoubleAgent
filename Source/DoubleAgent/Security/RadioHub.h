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
class DOUBLEAGENT_API ARadioHub : public APowerComponent{
	GENERATED_BODY()

	ARadioHub();
	
    FRadioEvent RadioEvent;
public:
    FTimerHandle SpeechTimer;

	UStaticMeshComponent* StaticMesh;
	
    bool StartRadioEvent(FRadioEvent RadioEvent_);
	
    UFUNCTION()
    void BroadcastRadioEvent();

    UFUNCTION()
    void PowerOn();
    UFUNCTION()
    void PowerOff();
};
