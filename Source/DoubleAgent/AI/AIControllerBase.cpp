// Please don't steal

#include "AIControllerBase.h"

#include "AICharacterBase_CHARACTER.h"
#include "NavigationPath.h"
#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"

AAIControllerBase::AAIControllerBase()
{
    //Create default sense config
    SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("Sight Config"));
    HearingConfig = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("Hearing Config"));

    //Setup sight
    SightConfig->SightRadius = SightRadius;
    SightConfig->LoseSightRadius = LoseSightRadius;
    SightConfig->PeripheralVisionAngleDegrees = FOV;
    SightConfig->SetMaxAge(MaxStimulusAge);
    SightConfig->DetectionByAffiliation.bDetectEnemies = true;
    SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
    SightConfig->DetectionByAffiliation.bDetectFriendlies = true;

    //Setup hearing
    HearingConfig->HearingRange = HearingRange;
    HearingConfig->bUseLoSHearing = true;
    HearingConfig->LoSHearingRange = HearingRange * 2;
    HearingConfig->SetMaxAge(MaxStimulusAge);
    HearingConfig->DetectionByAffiliation.bDetectEnemies = true;
    HearingConfig->DetectionByAffiliation.bDetectNeutrals = true;
    HearingConfig->DetectionByAffiliation.bDetectFriendlies = true;

    //Create perception component
    SetPerceptionComponent(*CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("Perception Component")));
    PerceptionComponent = GetPerceptionComponent();
    PerceptionComponent->SetDominantSense(*SightConfig->GetSenseImplementation());
    PerceptionComponent->ConfigureSense(*HearingConfig);
    PerceptionComponent->ConfigureSense(*SightConfig);

    //Disable control rotation
    bSetControlRotationFromPawnOrientation = false;
}

void AAIControllerBase::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);

    //Assigning perception delegate
    PerceptionComponent->OnPerceptionUpdated.AddDynamic(this, &AAIControllerBase::OnPerceptionUpdated);

    //Starting assigned behaviour tree and blackboard
    if (IsValid(BehaviourTree))
    {
        RunBehaviorTree(BehaviourTree);
    }
}

void AAIControllerBase::OnPerceptionUpdated(const TArray<AActor*>& DetectedActors)
{
    for (int a = 0; a < DetectedActors.Num(); a++)
    {
        //Get last sensed stimuli for each sense
        TArray<FAIStimulus> Stimuli = PerceptionComponent->GetActorInfo(*DetectedActors[a])->LastSensedStimuli;

        //Handle different perception of actors
        HandleHearing(DetectedActors[a], Stimuli[0]);
        HandleSight(DetectedActors[a], Stimuli[1]);
    }
}

void AAIControllerBase::HandleSight(AActor* CurrentActor, FAIStimulus& CurrentStimulus)
{
    if (CurrentActor->GetClass()->IsChildOf(AAICharacterBase_CHARACTER::StaticClass()))
    {
        NPCVisionTick(CurrentActor, CurrentStimulus);
    }
}

void AAIControllerBase::NPCVisionTick(AActor* CurrentActor, FAIStimulus& CurrentStimulus)
{
    AAIController* OtherNPCController = Cast<AAIController>(Cast<APawn>(CurrentActor)->GetController());
    UBlackboardComponent* OtherNPCBlackboard = OtherNPCController->GetBlackboardComponent();
    
    //UnconsciousNPC
    UObject* UnconsciousNPC = Blackboard->GetValueAsObject("UnconsciousNPC");
    if (IsValid(UnconsciousNPC) && UnconsciousNPC == CurrentActor && OtherNPCController->BrainComponent->IsRunning())
        {
            Blackboard->ClearValue("UnconsciousNPC");
        }
    else if (!IsValid(UnconsciousNPC))
        {
            Blackboard->SetValueAsObject("UnconsciousNPC", CurrentActor);
        }

    //Speaker
    Blackboard->SetValueAsBool("CanSeeSpeaker",IsValid(Blackboard->GetValueAsObject("Speaker")) && Blackboard->GetValueAsObject("Speaker") == CurrentActor);

    //Copy detection
    if (Blackboard->GetValueAsFloat("Detection") < 90 && OtherNPCBlackboard->GetValueAsFloat("Detection") >= 90)
    {
        Blackboard->SetValueAsFloat("Detection", 90);
    }
}

void AAIControllerBase::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    //Get visible actors
    TArray<AActor*> DetectedActors;
    TSubclassOf<UAISense> SightSense;
    PerceptionComponent->GetCurrentlyPerceivedActors(SightSense, DetectedActors);

    for (int a = 0; a < DetectedActors.Num(); a++)
    {
        //Get last sensed stimuli for each sense
        TArray<FAIStimulus> Stimuli = PerceptionComponent->GetActorInfo(*DetectedActors[a])->LastSensedStimuli;

        //Handle sight perception
        HandleSight(DetectedActors[a], Stimuli[1]);
    }
}

bool AAIControllerBase::HandleHearing(AActor* CurrentActor, FAIStimulus& CurrentStimulus)
{
    //Sound not coming from self and if sound was just heard
    if (CurrentActor != this && CurrentStimulus.GetAge() == 0)
    {
        //Get path to sound
        UNavigationPath* Path = UNavigationSystemV1::FindPathToLocationSynchronously(GetWorld(), CurrentActor->GetActorLocation(), CurrentStimulus.StimulusLocation, nullptr);
        //Path is not partial
        if (!Path->IsPartial())
        {
            //Speaker
            if (CurrentStimulus.Tag == "Speaker")
            {
                Blackboard->SetValueAsObject("Speaker", CurrentActor);
            }

            //Noise
            else if (CurrentStimulus.Tag == "LoudNoise")
            {
                Blackboard->SetValueAsVector("LoudNoiseLocation", CurrentStimulus.StimulusLocation);
            }

            //Something was perceived
            return true;
        }
    }

    //Nothing could be perceived
    return false;
}
