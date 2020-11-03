// Please don't steal

#include "StaffAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "DoubleAgent/Player_Character.h"
#include "DoubleAgent/AI/RoomVolume.h"
#include "Kismet/KismetArrayLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Perception/AIPerceptionComponent.h"

void AStaffAIController::HandleSightTick(AActor* CurrentActor, FAIStimulus& CurrentStimulus, float DeltaTime)
{
    Super::HandleSightTick(CurrentActor, CurrentStimulus, DeltaTime);

    if (CurrentActor->IsA(APlayer_Character::StaticClass()))
    {
        PlayerVisionTick(CurrentActor, CurrentStimulus, DeltaTime);
    }
}

void AStaffAIController::HandleSight(AActor* CurrentActor, FAIStimulus& CurrentStimulus)
{
    Super::HandleSight(CurrentActor, CurrentStimulus);

    if (CurrentActor->IsA(APlayer_Character::StaticClass()))
    {
        PlayerVisionUpdate(CurrentActor, CurrentStimulus);
    }
}

FTrackedPlayer::FTrackedPlayer(AActor* Actor_, FVector Location_, float Detection_)
{
    Actor = Actor_;
    Location = Location_;
    Detection = Detection_;
}

void AStaffAIController::PlayerVisionTick(AActor* CurrentPlayer, FAIStimulus& CurrentStimulus, float DeltaTime)
{
    //Setup
    float DetectionStep = 0;

    //If no players have been seen before
    if (Memory.Players.Num() == 0)
    {
        //Detection calculations
        if (Blackboard->GetValueAsFloat("Detection") >= 40)
            DetectionStep = 40 + DetectionRate * CurrentStimulus.Strength * DeltaTime;
        else
            DetectionStep = DetectionRate * CurrentStimulus.Strength * DeltaTime;

        //Add player to memory
        Memory.Players.Add(FTrackedPlayer(CurrentPlayer, CurrentStimulus.StimulusLocation, DetectionStep));

        //Update blackboard detection if needed
        if (Blackboard->GetValueAsFloat("Detection") < Memory.Players[0].Detection)
            Blackboard->SetValueAsFloat("Detection", Memory.Players[0].Detection);
    }
    else
    {
        //Iterate through all seen players
        for (int i = 0; i < Memory.Players.Num(); i++)
        {
            //If current player is stored in memory
            if (Memory.Players[i].Actor == CurrentPlayer)
            {
                //Distance modifier
                float DistanceModifier = UKismetMathLibrary::Abs(SightRadius / FVector().Dist(CurrentStimulus.StimulusLocation, GetPawn()->GetActorLocation()));
                //UE_LOG(LogTemp, Warning, TEXT("DistanceModifier %f"), DistanceModifier);
                
                //Angle modifier               
                float AngleModifier = 1 - UKismetMathLibrary::Abs(FVector().DotProduct(Cast<ACharacter>(GetPawn())->GetMesh()->GetSocketLocation("headSocket").ForwardVector, UKismetMathLibrary::FindLookAtRotation(GetPawn()->GetActorLocation(), CurrentStimulus.StimulusLocation).Vector()));
                //UE_LOG(LogTemp, Warning, TEXT("AngleModifier %f"), AngleModifier);
                
                //Detection calculations
                DetectionStep = Memory.Players[i].Detection + DetectionRate * CurrentStimulus.Strength * (AngleModifier + DistanceModifier)/2 * DeltaTime;
                //UE_LOG(LogTemp, Warning, TEXT("DetectionStep %f"), DetectionRate * CurrentStimulus.Strength * (AngleModifier + DistanceModifier)/2 * DeltaTime);

                //Update memory
                Memory.Players[i].Detection = DetectionStep;
                Memory.Players[i].Location = CurrentStimulus.ReceiverLocation;
                //Clamp if needed
                if (Memory.Players[i].Detection > 100)
                    Memory.Players[i].Detection = 100;

                //Update blackboard detection if needed
                if (Blackboard->GetValueAsFloat("Detection") < Memory.Players[i].Detection)
                    Blackboard->SetValueAsFloat("Detection", Memory.Players[i].Detection);
                break;
            }
        }

        //If player was not found in memory
        if (DetectionStep == 0)
        {
            //Detection calculations
            if (Blackboard->GetValueAsFloat("Detection") >= 40)
                DetectionStep = 40 + DetectionRate * CurrentStimulus.Strength * DeltaTime;
            else
                DetectionStep = DetectionRate * CurrentStimulus.Strength * DeltaTime;

            //Add player to memory
            Memory.Players.Add(FTrackedPlayer(CurrentPlayer, CurrentStimulus.StimulusLocation, DetectionStep));

            //Update blackboard detection if needed
            if (Blackboard->GetValueAsFloat("Detection") < Memory.Players[0].Detection)
                Blackboard->SetValueAsFloat("Detection", Memory.Players[0].Detection);
        }
    }
}

void AStaffAIController::PlayerVisionUpdate(AActor* CurrentPlayer, FAIStimulus& CurrentStimulus)
{
    //If player was just seen
    if (CurrentStimulus.IsActive())
    {
        //If there is no tracked player
        if (!IsValid(Blackboard->GetValueAsObject("LastPlayer")))
        {
            Blackboard->SetValueAsObject("LastPlayer", CurrentPlayer);
        }
    }
        //If player was just lost
    else
    {
        //If player was being tracked
        if (Blackboard->GetValueAsObject("LastPlayer") == CurrentPlayer)
        {
            Blackboard->ClearValue("LastPlayer");
        }
    }
}

void AStaffAIController::MarkSearchLocationSearched(ASearchLocation* SearchLocation)
{
    //Iterate through all rooms that contain this search location
    for (int i = 0; i < SearchLocation->RoomVolumes.Num(); i++)
    {
        //Iterate through search locations in that room
        ARoomVolume* CurrentRoom = SearchLocation->RoomVolumes[i];
        for (int a = 0; a < CurrentRoom->SearchLocations.Num(); a++)
        {
            //If current search location has not already been searched
            if (SearchedLocations.Find(CurrentRoom->SearchLocations[i]) == -1)
            {
                //Add search location to searched list
                SearchedLocations.Add(CurrentRoom->SearchLocations[i]);

                //If search location was being searched
                if (Blackboard->GetValueAsEnum("ActionStatus") == static_cast<uint8>(EActionStatus::Action_Searching) && Cast<ASearchLocation>(Blackboard->GetValueAsObject("TempObject")) == CurrentRoom->SearchLocations[i])
                {
                    //Set action status to idle
                    Blackboard->SetValueAsEnum("ActionStatus", static_cast<uint8>(EActionStatus::Action_Idle));
                }
            }
        }
    }
}

void AStaffAIController::RaiseDetection(float NewDetection)
{
    //If new detection is greater than current
    if (Blackboard->GetValueAsFloat("Detection") < NewDetection)
    {
        Blackboard->SetValueAsFloat("Detection", NewDetection);
    }
}

void AStaffAIController::RaiseVocalStatus(EVocalStatus NewVocalStatus)
{
    //If new vocal status is greater than current
    if (static_cast<EVocalStatus>(Blackboard->GetValueAsEnum("VocalStatus")) < NewVocalStatus)
    {
        Blackboard->SetValueAsEnum("VocalStatus", static_cast<uint8>(NewVocalStatus));
        //If cautious
        if (NewVocalStatus == EVocalStatus::Vocal_Cautious)
        {
            RaiseDetection(40);
        }
        //If greater than cautious
        else if (NewVocalStatus > EVocalStatus::Vocal_Cautious)
        {
            RaiseDetection(90);
        }
    }
}

void AStaffAIController::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    DetectionDecay(DeltaTime);
}

void AStaffAIController::DetectionDecay(float DeltaTime)
{
    //Setup
    float DecayStep = 0;

    //If no player is being targeted
    if (!IsValid(Blackboard->GetValueAsObject("LastPlayer")))
    {
        //Decay memory of players
        for (int i = 0; i < Memory.Players.Num(); i++)
        {
            const int MemoryDetection = round(Memory.Players[i].Detection);
            if (MemoryDetection > 0 && MemoryDetection != 90 && MemoryDetection != 40)
            {
                //Decay calculations
                DecayStep = DetectionRate * DeltaTime;

                Memory.Players[i].Detection -= DecayStep;
                //Clamp if needed
                if (Memory.Players[i].Detection < 0)
                    Memory.Players[i].Detection = 0;
            }
        }
    }
    else
    {
        //Iterate through all players
        for (int i = 0; i < Memory.Players.Num(); i++)
        {
            TArray<FAIStimulus> Stimuli = PerceptionComponent->GetActorInfo(*Memory.Players[i].Actor)->LastSensedStimuli;
            if (!Stimuli[1].IsActive())
            {
                const int MemoryDetection = round(Memory.Players[i].Detection);
                if (MemoryDetection > 0 && MemoryDetection != 90 && MemoryDetection != 40)
                {
                    //Decay calculations
                    DecayStep = DetectionRate * DeltaTime;

                    Memory.Players[i].Detection -= DecayStep;
                    //Clamp if needed
                    if (Memory.Players[i].Detection < 0)
                        Memory.Players[i].Detection = 0;
                }
            }
        }
    }
    if (DecayStep != 0)
    {
        //Decay blackboard detection if greater than 0 and not 40 or 90
        const int BlackboardDetection = round(Blackboard->GetValueAsFloat("Detection"));
        if (BlackboardDetection > 0 && BlackboardDetection != 90 && BlackboardDetection != 40)
        {
            Blackboard->SetValueAsFloat("Detection",Blackboard->GetValueAsFloat("Detection") - DetectionRate * DeltaTime);
            //Clamp if needed
            if (Blackboard->GetValueAsFloat("Detection") < 0)
                Blackboard->SetValueAsFloat("Detection", 0);
        }
    }
}

void AStaffAIController::NPCVisionTick(AActor* CurrentActor, FAIStimulus& CurrentStimulus)
{
    Super::NPCVisionTick(CurrentActor, CurrentStimulus);

    AAIController* OtherNPCController = Cast<AAIController>(Cast<APawn>(CurrentActor)->GetController());
    UBlackboardComponent* OtherNPCBlackboard = OtherNPCController->GetBlackboardComponent();

    //PlayerLastSeen
    if (!Blackboard->IsVectorValueSet("PlayerLastSeen") && OtherNPCBlackboard->IsVectorValueSet("PlayerLastSeen"))
    {
        Blackboard->SetValueAsVector("PlayerLastSeen", OtherNPCBlackboard->GetValueAsVector("PlayerLastSeen"));
    }

    //Backup
    else if (Blackboard->GetValueAsBool("BackupAvailable") && !OtherNPCBlackboard->GetValueAsBool("BackupAvailable"))
    {
        Blackboard->SetValueAsBool("BackupAvailable", false);
    }

    //Vocal status
    else if (Blackboard->GetValueAsEnum("VocalStatus") < OtherNPCBlackboard->GetValueAsEnum("VocalStatus"))
    {
        Blackboard->SetValueAsEnum("VocalStatus", OtherNPCBlackboard->GetValueAsEnum("VocalStatus"));
    }
}

bool AStaffAIController::HandleHearing(AActor* CurrentActor, FAIStimulus& CurrentStimulus)
{
    if (Super::HandleHearing(CurrentActor, CurrentStimulus))
    {
        //Noise
        if (CurrentStimulus.Tag == "Noise")
        {
            Blackboard->SetValueAsVector("NoiseLocation", CurrentStimulus.StimulusLocation);
        }

        //Movement
        else if (CurrentStimulus.Tag == "Movement" && Blackboard->GetValueAsFloat("Detection") < 90)
        {
            Blackboard->SetValueAsVector("NoiseLocation", CurrentStimulus.StimulusLocation);
        }

        //Something was perceived
        return true;
    }

    //Nothing could be perceived
    return false;
}
