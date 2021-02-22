// Please don't steal

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Perception/AISenseConfig_Sight.h"
#include "AIControllerBase.generated.h"

UENUM(BlueprintType, meta = (DisplayName = "VocalStatus", ToolTip = "Used to communicate vocally between NPCs via a bark system"))
enum class EVocalStatus : uint8
{
    //Default state
    Vocal_Idle UMETA(DisplayName = "Idle"),
    //Patrolling designated route
    Vocal_Patrol UMETA(DisplayName = "Patrol"),
    //Cautious due to suspicion
    Vocal_Cautious UMETA(DisplayName = "Cautious"),
    //Heightened alert state
    Vocal_Alert UMETA(DisplayName = "Alert"),
    //Searching building for player
    Vocal_Searching UMETA(DisplayName = "Searching"),
    //Arresting player
    Vocal_Subduing UMETA(DisplayName = "Subduing"),
    //Attacking player
    Vocal_Engaging UMETA(DisplayName = "Engaging"),
};

/**
 * 
 */
UCLASS()
class DOUBLEAGENT_API AAIControllerBase : public AAIController
{
    GENERATED_BODY()

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

    //Perception configurations
    UAISenseConfig_Sight* SightConfig;
    UAISenseConfig_Hearing* HearingConfig;
    
    //Range in cm based on fov
    float SightRadius = 3000;

    //Falloff range in cm based on fov
    float LoseSightRadius = 3500;

    //Fov from forward vector
    float FOV = 90;

    //Amount of time in seconds the stimulus stays after being lost
    float MaxStimulusAge = 1;

    //Range in cm based on location
    float HearingRange = 3000;
    
    //NPC spotted tick
    UFUNCTION()
    virtual void NPCVisionTick(AActor* CurrentActor, FAIStimulus& CurrentStimulus);

    //Process hearing perception
    UFUNCTION()
    virtual bool HandleHearing(AActor* CurrentActor, FAIStimulus& CurrentStimulus);

    //Used to disable perception
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bPerceptionEnabled = true;
    
    //Process sight perception
    UFUNCTION()
    virtual void HandleSight(AActor* CurrentActor, FAIStimulus& CurrentStimulus){};
    UFUNCTION()
    virtual void HandleSightTick(AActor* CurrentActor, FAIStimulus& CurrentStimulus, float DeltaTime);
    
    //Override
    virtual void Tick(float DeltaTime) override;

    //Perception component
    UAIPerceptionComponent* PerceptionComponent;

    //Helper Functions
    UFUNCTION(BlueprintCallable)
    bool RaiseDetection(float NewDetection);
    UFUNCTION(BlueprintCallable)
    bool RaiseVocalStatus(EVocalStatus NewVocalStatus);
};
