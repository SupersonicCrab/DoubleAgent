// Please don't steal

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BehaviorTree/BehaviorTree.h"
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

    //Perception configurations
    UAISenseConfig_Sight* SightConfig;
    UAISenseConfig_Hearing* HearingConfig;

    //Behaviour tree asset
    UPROPERTY(EditAnywhere)
    UBehaviorTree* BehaviourTree;
    
    //Called when perceived actors is updated
    UFUNCTION()
    void OnPerceptionUpdated(const TArray<AActor*>& DetectedActors);
    
public:
    AAIControllerBase();

    //Override
    virtual void OnPossess(APawn* InPawn) override;
  
    //Range in cm based on fov
    UPROPERTY(EditAnywhere)
    float SightRadius = 3000;
    
    //NPC spotted tick
    UFUNCTION()
    virtual void NPCVisionTick(AActor* CurrentActor, FAIStimulus& CurrentStimulus);

    //Process hearing perception
    UFUNCTION()
    virtual bool HandleHearing(AActor* CurrentActor, FAIStimulus& CurrentStimulus);

    //Process sight perception
    UFUNCTION()
    virtual void HandleSight(AActor* CurrentActor, FAIStimulus& CurrentStimulus){};
    UFUNCTION()
    virtual void HandleSightTick(AActor* CurrentActor, FAIStimulus& CurrentStimulus, float DeltaTime);

    //Override
    virtual void Tick(float DeltaTime) override;

    //Perception component
    UAIPerceptionComponent* PerceptionComponent;
};
