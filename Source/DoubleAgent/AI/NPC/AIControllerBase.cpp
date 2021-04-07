// Please don't steal

#include "AIControllerBase.h"
#include "Perception/AIPerceptionComponent.h"
#include "NavigationPath.h"
#include "NavigationSystem.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Components/CapsuleComponent.h"
#include "DoubleAgent/AI/AICharacterBase_CHARACTER.h"
#include "DoubleAgent/AI/RoomVolume.h"
#include "DoubleAgent/AI/Senses/AISenseConfig_SightBase.h"
#include "Kismet/KismetSystemLibrary.h"

AAIControllerBase::AAIControllerBase()
{
    //Create default sense config
    SightConfig = CreateDefaultSubobject<UAISenseConfig_SightBase>(TEXT("Sight Config"));
    HearingConfig = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("Hearing Config"));

    //Create perception component
    SetPerceptionComponent(*CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("Perception Component")));
    PerceptionComponent = GetPerceptionComponent();

    //Setup hearing
    HearingConfig->HearingRange = HearingRange;
    HearingConfig->SetMaxAge(MaxStimulusAge);
    HearingConfig->DetectionByAffiliation.bDetectEnemies = true;
    HearingConfig->DetectionByAffiliation.bDetectNeutrals = true;
    HearingConfig->DetectionByAffiliation.bDetectFriendlies = true;
    PerceptionComponent->ConfigureSense(*HearingConfig);
        
    //Setup sight
    SightConfig->SightRadius = SightRadius;
    SightConfig->LoseSightRadius = LoseSightRadius;
    SightConfig->PeripheralVisionAngleDegrees = FOV;
    SightConfig->SetMaxAge(MaxStimulusAge);
    SightConfig->DetectionByAffiliation.bDetectEnemies = true;
    SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
    SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
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
    if (!bPerceptionEnabled)
        return;

    //Iterate through detected actors
    for (int a = 0; a < DetectedActors.Num(); a++)
    {
        //Get last sensed stimuli for each sense
        TArray<FAIStimulus> Stimuli = PerceptionComponent->GetActorInfo(*DetectedActors[a])->LastSensedStimuli;
        //Iterate through sense
        for (int i = 0; i < Stimuli.Num(); i++)
        {
            //If sense is hearing and was sensed
            if (Stimuli[i].Type.Name == "Default__AISense_Hearing" && Stimuli[i].WasSuccessfullySensed())
            {
                HandleHearing(DetectedActors[a], Stimuli[i]);
            }
            //If sense is sight
            else if (Stimuli[i].Type.Name == "Default__AISense_Sight")
            {
                HandleSight(DetectedActors[a], Stimuli[i]);
            }
        }
    }
}

void AAIControllerBase::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (!bPerceptionEnabled)
        return;
    
    //Get visible actors
    TArray<AActor*> DetectedActors;
    TSubclassOf<UAISense> SightSense;
    PerceptionComponent->GetCurrentlyPerceivedActors(SightSense, DetectedActors);

    //Iterate through all visible actors
    for (int a = 0; a < DetectedActors.Num(); a++)
    {
        //Get last sensed stimuli for each sense
        TArray<FAIStimulus> Stimuli = PerceptionComponent->GetActorInfo(*DetectedActors[a])->LastSensedStimuli;
        //Iterate through all senses
        for (int i = 0; i < Stimuli.Num(); i++)
        {
            if (Stimuli[i].GetAge() == 0 && Stimuli[i].IsActive() && Stimuli[i].Type.Name == "Default__AISense_Sight")
            {
                HandleSightTick(DetectedActors[a], Stimuli[i], DeltaTime);
            }
        }      
    }
}

bool AAIControllerBase::RaiseDetection(float NewDetection)
{
    //If new detection is greater than current
    if (Blackboard->GetValueAsFloat("Detection") < NewDetection)
    {
        Blackboard->SetValueAsFloat("Detection", NewDetection);

        //Update suspicion if above 40
        if (NewDetection > 40 && !Blackboard->GetValueAsBool("Suspicious"))
            Blackboard->SetValueAsBool("Suspicious", true);

        return true;
    }
    return false;
}

bool AAIControllerBase::RaiseVocalStatus(EVocalStatus NewVocalStatus)
{
    //If cautious
    if (NewVocalStatus == EVocalStatus::Vocal_Cautious)
        RaiseDetection(40.0f);
    
    //If greater than cautious
    else if (NewVocalStatus > EVocalStatus::Vocal_Cautious)
        RaiseDetection(90.0f);
    
    //If new vocal status is greater than current
    if (static_cast<EVocalStatus>(Blackboard->GetValueAsEnum("VocalStatus")) < NewVocalStatus)
    {       
        Blackboard->SetValueAsEnum("VocalStatus", static_cast<uint8>(NewVocalStatus));
        return true;
    }
    return false;
}

void AAIControllerBase::HandleSightTick(AActor* CurrentActor, FAIStimulus& CurrentStimulus, float DeltaTime)
{
    //NPC
    if (CurrentActor->IsA(AAICharacterBase_CHARACTER::StaticClass()))
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
    else if (UnconsciousNPC == NULL && !Cast<AAIControllerBase>(Cast<AAICharacterBase_CHARACTER>(CurrentActor)->GetController())->BrainComponent->IsRunning())
    {
        RaiseDetection(90.0f);
        RaiseVocalStatus(EVocalStatus::Vocal_Engaging);

        //If character is not tranquilizers
        if (!Cast<AAICharacterBase_CHARACTER>(CurrentActor)->bTranquilized)
            Blackboard->SetValueAsObject("UnconsciousNPC", CurrentActor);
    }

    //Copy detection
    else if (Blackboard->GetValueAsFloat("Detection") < 90 && OtherNPCBlackboard->GetValueAsFloat("Detection") >= 90)
    {
        RaiseDetection(90.0f);
    }
}

bool AAIControllerBase::HandleHearing(AActor* CurrentActor, FAIStimulus& CurrentStimulus)
{
    //Get path to sound
    UNavigationPath* Path = UNavigationSystemV1::FindPathToLocationSynchronously(
        GetWorld(), CurrentActor->GetActorLocation(), CurrentStimulus.StimulusLocation, nullptr);

    //Path is not partial
    if (!Path->IsPartial())
    {
        //Speaker
        if (CurrentStimulus.Tag == "Speech")
        {
            Blackboard->SetValueAsObject("Speaker", CurrentActor);
            RaiseVocalStatus(static_cast<EVocalStatus>(Cast<AAIController>(Cast<APawn>(CurrentActor)->GetController())->GetBlackboardComponent()->GetValueAsEnum("VocalStatus")));
        }

        //Noise
        if (CurrentStimulus.Tag == "Noise")
        {
            //Get which rooms actor is inside
            TArray<AActor*> Rooms;
            Cast<ABaseCharacter_CHARACTER>(CurrentActor)->GetCapsuleComponent()->GetOverlappingActors(Rooms, ARoomVolume::StaticClass());

            //If there is no room or detection is above or at 90
            if (Rooms.Num() == 0 || Blackboard->GetValueAsFloat("Detection") >= 90)
            {
                Blackboard->SetValueAsVector("NoiseLocation", CurrentStimulus.StimulusLocation);
                return true;
            }

            bool bPublic = false;
            for (int i = 0; i < Rooms.Num(); i++)
            {
                //If the room isn't public
                if (Cast<ARoomVolume>(Rooms[i])->bPublic)
                    bPublic = true;
            }

            if (!bPublic)
            {
                Blackboard->SetValueAsVector("NoiseLocation", CurrentStimulus.StimulusLocation);
                return true;
            }
        }
        
        //LoudNoise
        else if (CurrentStimulus.Tag == "LoudNoise")
            Blackboard->SetValueAsVector("LoudNoiseLocation", CurrentStimulus.StimulusLocation);

        //Something was perceived
        return true;
    }

    //Nothing could be perceived
    return false;
}
