// Please don't steal

#include "StaffAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "DoubleAgent/Player_Character.h"
#include "DoubleAgent/AI/RoomVolume.h"
#include "Kismet/KismetArrayLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Perception/AIPerceptionComponent.h"
#include "NavigationSystem.h"
#include "Engine/EngineTypes.h"

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

    //If actor is player
    if (CurrentActor->IsA(APlayer_Character::StaticClass()))
        PlayerVisionUpdate(CurrentActor, CurrentStimulus);

    //If actor is staffAI
    if (Cast<APawn>(CurrentActor) != nullptr && Cast<AStaffAIController>(Cast<APawn>(CurrentActor)->GetController()) != nullptr)
        StaffVisionUpdate(CurrentActor, CurrentStimulus);

    //If actor is camera
    if (Cast<ACamera>(CurrentActor) != nullptr)
        CameraVisionUpdate(CurrentActor, CurrentStimulus);

    //If actor is a door
    if (Cast<ADoor>(CurrentActor) != nullptr && Blackboard->GetValueAsFloat("Detection") < 90)
        DoorVisionUpdate(CurrentActor, CurrentStimulus);
        
}

void AStaffAIController::StaffVisionUpdate(AActor* CurrentActor, FAIStimulus& CurrentStimulus)
{
}

FTrackedActor::FTrackedActor(AActor* Actor_, FVector Location_, float Detection_)
{
    Actor = Actor_;
    Location = Location_;
    Detection = Detection_;
}

FTrackedCamera::FTrackedCamera(ACamera* Camera_)
{
    Camera = Camera_;
    bAutoRotate = Camera->bAutoRotate;
}

FTrackedDoor::FTrackedDoor(ADoor* Door_)
{
    Door = Door_;
    bDoorOpen = Door->bDoorOpen;
}

void AStaffAIController::PlayerVisionTick(AActor* CurrentPlayer, FAIStimulus& CurrentStimulus, float DeltaTime)
{
    //Skip detection calculations if player is doing nothing wrong
    APlayer_Character* Player = Cast<APlayer_Character>(CurrentPlayer);
    if (!Player->bTresspassing && !Player->bIllegalAction)
        return;
    
    //Setup
    float DetectionStep = 0;

    //Set last player if there is no tracked player
    if (!IsValid(Blackboard->GetValueAsObject("LastPlayer")))
    {
        Blackboard->SetValueAsObject("LastPlayer", CurrentPlayer);
    }

    //If no players have been seen before
    if (Memory.Players.Num() == 0)
    {
        //Detection calculations
        if (Blackboard->GetValueAsFloat("Detection") >= 40)
            DetectionStep = 40 + DetectionRate * CurrentStimulus.Strength * DeltaTime;
        else
            DetectionStep = DetectionRate * CurrentStimulus.Strength * DeltaTime;

        //Add player to memory
        Memory.Players.Add(FTrackedActor(CurrentPlayer, CurrentStimulus.StimulusLocation, DetectionStep));

        //Update blackboard detection
        RaiseDetection(Memory.Players[0].Detection);
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
                float DistanceModifier = UKismetMathLibrary::Abs(
                    SightRadius / FVector().Dist(CurrentStimulus.StimulusLocation, GetPawn()->GetActorLocation()));

                //Angle modifier               
                float AngleModifier = 1 - UKismetMathLibrary::Abs(FVector().DotProduct(
                    Cast<ACharacter>(GetPawn())->GetMesh()->GetSocketLocation("headSocket").ForwardVector,
                    UKismetMathLibrary::FindLookAtRotation(GetPawn()->GetActorLocation(),
                                                           CurrentStimulus.StimulusLocation).Vector()));

                //Detection calculations
                DetectionStep = Memory.Players[i].Detection + DetectionRate * CurrentStimulus.Strength * (AngleModifier
                    + DistanceModifier) / 2 * DeltaTime;
                
                //Clamp if needed
                if (DetectionStep < 0)
                    DetectionStep = 0;

                //Update memory
                Memory.Players[i].Detection = DetectionStep;
                Memory.Players[i].Location = CurrentStimulus.StimulusLocation;
                
                //Clamp if needed
                if (Memory.Players[i].Detection > 100)
                {
                    //Set player last seen
                    if (Blackboard->GetValueAsObject("LastPlayer") == CurrentPlayer)
                        Blackboard->SetValueAsVector("PlayerLastSeen", CurrentStimulus.StimulusLocation);
                    Memory.Players[i].Detection = 100;
                }

                //Update blackboard detection
                RaiseDetection(Memory.Players[i].Detection);
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
            Memory.Players.Add(FTrackedActor(CurrentPlayer, CurrentStimulus.StimulusLocation, DetectionStep));

            //Update blackboard detection
            RaiseDetection(Memory.Players[0].Detection);
        }
    }
}

void AStaffAIController::PlayerVisionUpdate(AActor* CurrentPlayer, FAIStimulus& CurrentStimulus)
{
    //If player was lost and was being tracked
    if (!CurrentStimulus.IsActive() && Blackboard->GetValueAsObject("LastPlayer") == CurrentPlayer)
    {
        Blackboard->ClearValue("LastPlayer");
    }
}

void AStaffAIController::CameraVisionUpdate(AActor* CurrentActor, FAIStimulus& CurrentStimulus)
{
    ACamera* Camera = Cast<ACamera>(CurrentActor);

    //Search for camera in memory
    int SearchResult = -1;
    for (int i = 0; i < Memory.Cameras.Num(); i++)
    {
        if (Camera == Memory.Cameras[i].Camera)
            SearchResult = i;
    }
    
    //If camera was found in memory
    if (SearchResult != -1)
    {
        //If camera was previously rotating
        if (Camera->bAutoRotate != Memory.Cameras[SearchResult].bAutoRotate)
        {
            Blackboard->SetValueAsBool("CamerasActive", false);
            RaiseDetection(40);
        }   
    }

    //Camera was not found in memory
    else
        Memory.Cameras.Add(Camera);
}

void AStaffAIController::DoorVisionUpdate(AActor* CurrentActor, FAIStimulus& CurrentStimulus)
{
    ADoor* Door = Cast<ADoor>(CurrentActor);

    //If door is being used by an NPC
    if (Door->InteractingNPC != nullptr)
        return;
    
    //Search for door in memory
    int SearchResult = -1;
    for (int i = 0; i < Memory.Doors.Num(); i++)
    {
        if (Memory.Doors[i].Door == Door)
        {
            SearchResult = i;
        }
    }

    //If door was not found in memory
    if (SearchResult != -1)
    {
        //If door was previously closed
        if (Door->bDoorOpen && !Memory.Doors[SearchResult].bDoorOpen)
        {
            Blackboard->SetValueAsObject("OpenedDoor", Door);
            RaiseDetection(40);
        }
    }

    //Door was not found in memory
    else
        Memory.Doors.Add(Door);
}

void AStaffAIController::HandleRadioEvent(FRadioEvent RadioEvent)
{
    switch (RadioEvent.RadioEvent)
    {
        //Alert
    case ERadioEvent::Radio_Alert:
        Blackboard->SetValueAsVector("LoudNoiseLocation", RadioEvent.Location);
        RaiseVocalStatus(EVocalStatus::Vocal_Alert);
        break;

        //Engage
    case ERadioEvent::Radio_Engage:
        Blackboard->SetValueAsVector("LoudNoiseLocation", RadioEvent.Location);
        Blackboard->ClearValue("InitialLastSeen");
        RaiseVocalStatus(EVocalStatus::Vocal_Engaging);
        break;
    }
}

void AStaffAIController::MarkSearchLocationSearched(ASearchLocation* SearchLocation)
{
    SearchedLocations.Add(SearchLocation);
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
                if (Blackboard->GetValueAsEnum("ActionStatus") == static_cast<uint8>(EActionStatus::Action_Searching) &&
                    Cast<ASearchLocation>(Blackboard->GetValueAsObject("TempObject")) == CurrentRoom->SearchLocations[i]
                )
                {
                    //Set action status to idle
                    Blackboard->SetValueAsEnum("ActionStatus", static_cast<uint8>(EActionStatus::Action_Idle));
                }
            }
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

            for (int a = 0; a < Stimuli.Num(); a++)
            {
                if (Stimuli[a].Type.Name == "Default__AISense_Sight" && !Stimuli[a].IsActive())
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
                    break;
                }
            }
        }
    }
    if (DecayStep != 0)
    {
        //Decay blackboard detection if greater than 0 and not 40 or 90
        float BlackboardDetection = round(Blackboard->GetValueAsFloat("Detection"));
        if (BlackboardDetection > 0 && BlackboardDetection != 90 && BlackboardDetection != 40)
        {
            Blackboard->SetValueAsFloat("Detection",Blackboard->GetValueAsFloat("Detection") - DetectionRate * DeltaTime);
            //Clamp if needed
            BlackboardDetection = Blackboard->GetValueAsFloat("Detection");
            if (BlackboardDetection < 0)
                Blackboard->SetValueAsFloat("Detection", 0.0f);
            if (BlackboardDetection < 91 && BlackboardDetection > 90)
                Blackboard->SetValueAsFloat("Detection", 90.0f);
            if (BlackboardDetection < 41 && BlackboardDetection > 40)
                Blackboard->SetValueAsFloat("Detection", 40.0f);
        }
    }
}

void AStaffAIController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);

    Blackboard->SetValueAsBool("CamerasActive", true);
    Blackboard->SetValueAsBool("Power", true);
    Blackboard->SetValueAsBool("BackupAvailable", true);
    Blackboard->SetValueAsObject("SelfActor", GetPawn());
}

void AStaffAIController::NPCVisionTick(AActor* CurrentActor, FAIStimulus& CurrentStimulus)
{
    Super::NPCVisionTick(CurrentActor, CurrentStimulus);

    UBlackboardComponent* OtherNPCBlackboard = Cast<AAIController>(Cast<APawn>(CurrentActor)->GetController())->GetBlackboardComponent();
    AAIController* OtherNPCController = Cast<AAIController>(Cast<APawn>(CurrentActor)->GetController());

    //If NPC is unconscious there is no more information to gather
    if (!OtherNPCController->BrainComponent->IsRunning())
        return;
    
    //Get player last seen if other NPC can see player
    if (!Blackboard->IsVectorValueSet("PlayerLastSeen") && IsValid(OtherNPCBlackboard->GetValueAsObject("LastPlayer")) && OtherNPCBlackboard->IsVectorValueSet("PlayerLastSeen"))
    {
        Blackboard->SetValueAsVector("PlayerLastSeen", OtherNPCBlackboard->GetValueAsVector("PlayerLastSeen"));
    }

    //Disable backup if not set
    else if (Blackboard->GetValueAsBool("BackupAvailable") && !OtherNPCBlackboard->GetValueAsBool("BackupAvailable"))
    {
        Blackboard->SetValueAsBool("BackupAvailable", false);
    }

    //Inherit vocal status if higher
    else if (Blackboard->GetValueAsEnum("VocalStatus") < OtherNPCBlackboard->GetValueAsEnum("VocalStatus"))
    {
        RaiseVocalStatus(static_cast<EVocalStatus>(OtherNPCBlackboard->GetValueAsEnum("VocalStatus")));
    }

    //If NPC is staff
    if (Cast<AStaffAIController>(Cast<APawn>(CurrentActor)->GetController()) != nullptr)
        StaffVisionTick(CurrentActor, CurrentStimulus);
}

void AStaffAIController::StaffVisionTick(AActor* CurrentActor, FAIStimulus& CurrentStimulus)
{
    UBlackboardComponent* OtherNPCBlackboard = Cast<AAIController>(Cast<APawn>(CurrentActor)->GetController())->GetBlackboardComponent();
    EActionStatus ActionStatus = static_cast<EActionStatus>(Blackboard->GetValueAsEnum("ActionStatus"));

    //If action status is not idle and both staffAI are performing the same action
    if (ActionStatus != EActionStatus::Action_Idle && static_cast<EActionStatus>(OtherNPCBlackboard->GetValueAsEnum("ActionStatus")) == ActionStatus)
    {
        //If both staff is going to turn back on the same light switch
        if (ActionStatus == EActionStatus::Action_LightSwitch && Blackboard->GetValueAsObject("LightSwitch") == OtherNPCBlackboard->GetValueAsObject("LightSwitch"))
        {
            Blackboard->ClearValue("LightSwitch");
            Blackboard->SetValueAsEnum("ActionStatus", static_cast<uint8>(EActionStatus::Action_Idle));
        }

        //If both staff is going to check out the same noise
        else if (ActionStatus == EActionStatus::Action_NoiseInvestigation && Blackboard->GetValueAsVector("NoiseLocation").Equals(OtherNPCBlackboard->GetValueAsVector("NoiseLocation"),50))
        {
            Blackboard->ClearValue("NoiseLocation");
            Blackboard->SetValueAsEnum("ActionStatus", static_cast<uint8>(EActionStatus::Action_Idle));
        }

        //If both staff are going to search location
        else if (ActionStatus == EActionStatus::Action_Searching || ActionStatus == EActionStatus::Action_SearchInvestigation)
        {
            //If both search locations are properly set
            if (Blackboard->GetValueAsObject("TempObject") != nullptr && OtherNPCBlackboard->GetValueAsObject("TempObject") != nullptr)
            {
                //If both going to the same location
                if (Blackboard->GetValueAsObject("TempObject") == OtherNPCBlackboard->GetValueAsObject("TempObject"))
                {
                    AActor* ContestedLocation = static_cast<AActor*>(Blackboard->GetValueAsObject("TempObject"));
                    //If other npc is closer to search location
                    if (GetPawn()->GetDistanceTo(ContestedLocation) > CurrentActor->GetDistanceTo(ContestedLocation))
                    {
                        Blackboard->SetValueAsEnum("ActionStatus", static_cast<uint8>(EActionStatus::Action_Idle));
                        MarkSearchLocationSearched(Cast<ASearchLocation>(Blackboard->GetValueAsObject("TempObject")));
                    }
                }
                
                //If going to different locations and other location has not been searched
                else if (SearchedLocations.Find(Cast<ASearchLocation>(Blackboard->GetValueAsObject("TempObject"))) == -1)
                    //Take note of the other area that npc is searching
                    MarkSearchLocationSearched(Cast<ASearchLocation>(Blackboard->GetValueAsObject("TempObject")));
                
                //Mark last searched area of other staff if not already searched
                AStaffAIController* OtherStaff = Cast<AStaffAIController>(Cast<APawn>(CurrentActor)->GetController());
                if (OtherStaff->SearchedLocations.Num() != 0 && SearchedLocations.Find(OtherStaff->SearchedLocations[OtherStaff->SearchedLocations.Num()-1]) == -1)
                    MarkSearchLocationSearched(OtherStaff->SearchedLocations[OtherStaff->SearchedLocations.Num()-1]);
            }
        }
    }
}

bool AStaffAIController::HandleHearing(AActor* CurrentActor, FAIStimulus& CurrentStimulus)
{
    if (Super::HandleHearing(CurrentActor, CurrentStimulus))
    {
        if (CurrentStimulus.Tag == "Speech")
        {
            //UnconsciousNPC
            UObject* UnconsciousNPC = Cast<AAIControllerBase>(Cast<AAICharacterBase_CHARACTER>(CurrentActor)->GetController())->GetBlackboardComponent()->GetValueAsObject("UnconsciousNPC");
            if (UnconsciousNPC != nullptr)
                Blackboard->SetValueAsObject("UnconsciousNPC", UnconsciousNPC);           
        }

        //Movement
        else if (CurrentStimulus.Tag == "Movement" && Blackboard->GetValueAsFloat("Detection") < 90)
            Blackboard->SetValueAsVector("NoiseLocation", CurrentStimulus.StimulusLocation);
            
        //Something was perceived
        return true;
    }

    //Nothing could be perceived
    return false;
}
