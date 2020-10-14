// Please don't steal

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Perception/AISenseConfig_Sight.h"
#include "AIControllerBase.generated.h"

/**
 * 
 */
UCLASS()
class DOUBLEAGENT_API AAIControllerBase : public AAIController
{
    GENERATED_BODY()
public:
    AAIControllerBase();

private:
    //Range in cm based on fov
    UPROPERTY(EditAnywhere)
    float SightRadius = 3000;

    //Falloff range in cm based on fov
    UPROPERTY(EditAnywhere)
    float LoseSightRadius = 3500;

    //Fov from forward vector
    UPROPERTY(EditAnywhere)
    float FOV = 90;

    //Amount of time in seconds the stimulus stays after being lost
    UPROPERTY(EditAnywhere)
    float MaxStimulusAge = 1;

    //Range in cm based on location
    UPROPERTY(EditAnywhere)
    float HearingRange = 3250;

    //Perception component
    UAIPerceptionComponent* PerceptionComponent;

    //Perception configurations
    UAISenseConfig_Sight* SightConfig;
    UAISenseConfig_Hearing* HearingConfig;

    //Behaviour tree asset
    UPROPERTY(EditAnywhere)
    UBehaviorTree* BehaviourTree;

    //Overrides
    virtual void OnPossess(APawn* InPawn) override;

    virtual void Tick(float DeltaSeconds) override;
    
    //Called when perceived actors is updated
    UFUNCTION()
    void OnPerceptionUpdated(const TArray<AActor*>& DetectedActors);
    
    //Process hearing perception
    UFUNCTION()
    bool HandleHearing(AActor* CurrentActor, FAIStimulus& CurrentStimulus);

    //Process sight perception
    UFUNCTION()
    void HandleSight(AActor* CurrentActor, FAIStimulus& CurrentStimulus);

    //NPC spotted tick
    UFUNCTION()
    void NPCVisionTick(AActor* CurrentActor, FAIStimulus& CurrentStimulus);
};
