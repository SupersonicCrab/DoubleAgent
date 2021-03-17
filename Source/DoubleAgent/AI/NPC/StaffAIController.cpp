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
    if (Cast<ADoor>(CurrentActor) != nullptr)
        DoorVisionUpdate(CurrentActor, CurrentStimulus);

    //If actor is the camera hub
    if (Cast<ACameraHub>(CurrentActor) != nullptr)
        CameraHubVisionUpdate(CurrentActor, CurrentStimulus);
        
}

void AStaffAIController::StaffVisionUpdate(AActor* CurrentActor, FAIStimulus& CurrentStimulus)
{
    InheritPlayerMemory(Cast<AStaffAIController>(Cast<APawn>(CurrentActor)->GetController()));
}

FTrackedCharacter::FTrackedCharacter(AActor* Actor_, FVector Location_, float Detection_)
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
    int PlayerMemoryReference = -1;
    
    //Update last stimulus
    for (int i = 0; i < Memory.Players.Num(); i++)
    {
        if (Memory.Players[i].Actor == CurrentPlayer)
        {
            Memory.Players[i].LastSensedStimuli = CurrentStimulus;
            PlayerMemoryReference = i;
            break;
        }
    }
    
    //Setup
    float DetectionStep = 0;

    //Set last player if there is no tracked player
    if (!IsValid(Blackboard->GetValueAsObject("LastPlayer")))
    {
        Blackboard->SetValueAsObject("LastPlayer", CurrentPlayer);
    }

    //Skip detection calculations if player is doing nothing wrong
    APlayer_Character* Player = Cast<APlayer_Character>(CurrentPlayer);
    if (!Player->IsThreat() && (PlayerMemoryReference == -1 || Memory.Players[PlayerMemoryReference].Detection < 40))
        return;
    
    //If no players have been seen before
    if (Memory.Players.Num() == 0)
    {
        //Detection calculations
        if (Blackboard->GetValueAsFloat("Detection") >= 40)
            DetectionStep = 40 + DetectionRate * CurrentStimulus.Strength * DeltaTime;
        else
            DetectionStep = DetectionRate * CurrentStimulus.Strength * DeltaTime;

        //Add player to memory
        Memory.Players.Add(FTrackedCharacter(CurrentPlayer, CurrentStimulus.StimulusLocation, DetectionStep));

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
                    SightRadius / FVector().Dist(CurrentStimulus.StimulusLocation, CurrentStimulus.ReceiverLocation));

                float AngleModifier = 1;

                TArray<AActor*> temp;
                PerceptionComponent->GetCurrentlyPerceivedActors(UAISense_Sight::StaticClass(), temp);
                //If player is being seen from self and not cameras
                if (temp.Contains(CurrentPlayer))
                {
                    //Angle modifier               
                    AngleModifier = 1 - UKismetMathLibrary::Abs(FVector().DotProduct(
                        Cast<ACharacter>(GetPawn())->GetMesh()->GetSocketLocation("headSocket").ForwardVector,
                        UKismetMathLibrary::FindLookAtRotation(CurrentStimulus.ReceiverLocation,
                                                               CurrentStimulus.StimulusLocation).Vector()));
                }
                
                //Detection calculations
                DetectionStep = Memory.Players[i].Detection + DetectionRate * CurrentStimulus.Strength * (AngleModifier
                    + DistanceModifier) / 2 * DeltaTime;
                
                //Clamp if needed
                if (DetectionStep < 0)
                    DetectionStep = 0;

                //Update memory
                Memory.Players[i].Detection = DetectionStep;
                Memory.Players[i].Location = CurrentStimulus.StimulusLocation;

                //Set player last seen
                if (Blackboard->GetValueAsObject("LastPlayer") == CurrentPlayer && Blackboard->GetValueAsFloat("Detection") > 90) 
                    Blackboard->SetValueAsVector("PlayerLastSeen", CurrentStimulus.StimulusLocation);
                
                //Clamp if needed
                if (Memory.Players[i].Detection > 100)
                {
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
            if (Blackboard->GetValueAsFloat("Detection") >= 90)
                DetectionStep = 90 + DetectionRate * CurrentStimulus.Strength * DeltaTime;
            else if (Blackboard->GetValueAsFloat("Detection") >= 40)
                DetectionStep = 40 + DetectionRate * CurrentStimulus.Strength * DeltaTime;
            else
                DetectionStep = DetectionRate * CurrentStimulus.Strength * DeltaTime;

            //Add player to memory
            Memory.Players.Add(FTrackedCharacter(CurrentPlayer, CurrentStimulus.StimulusLocation, DetectionStep));

            //Update blackboard detection
            RaiseDetection(Memory.Players[0].Detection);
        }
    }
}

void AStaffAIController::PlayerVisionUpdate(AActor* CurrentPlayer, FAIStimulus& CurrentStimulus)
{
    //Update last stimulus
    for (int i = 0; i < Memory.Players.Num(); i++)
    {
        if (Memory.Players[i].Actor == CurrentPlayer)
        {
            Memory.Players[i].LastSensedStimuli = CurrentStimulus;
            break;
        }
    }
    
    //If tracked player was lost
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

void AStaffAIController::CameraHubVisionUpdate(AActor* CurrentActor, FAIStimulus& CurrentStimulus)
{   
    ACameraHub* CameraHub = Cast<ACameraHub>(CurrentActor);
    Blackboard->SetValueAsBool("CamerasActive", CameraHub->OperatorNPC != nullptr);
    if (CameraHub->bDisplayOn && CameraHub->OperatorNPC == nullptr)
        RaiseDetection(40);
}

void AStaffAIController::DoorVisionUpdate(AActor* CurrentActor, FAIStimulus& CurrentStimulus)
{
    ADoor* Door = Cast<ADoor>(CurrentActor);

    //If door is being used by an NPC or public
    if (Door->InteractingNPC != nullptr || Door->bPublic)
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
        //If alert or greater and door closed
        if (Blackboard->GetValueAsFloat("Detection") >= 90 && !Door->bDoorOpen && !Door->bImportant && Blackboard->GetValueAsObject("ClosedDoor") == nullptr)
        {
            Blackboard->SetValueAsObject("ClosedDoor", Door);

            TArray<AActor*> RoomVolumes;
            GetOverlappingActors(RoomVolumes, ARoomVolume::StaticClass());

            for (int i = 0; i < RoomVolumes.Num(); i++)
            {
                RemoveRoomSearchLocations(Cast<ARoomVolume>(RoomVolumes[i]));
            }
        }
        
        //If door was open and previously closed or important
        if (Door->bDoorOpen && (!Memory.Doors[SearchResult].bDoorOpen || Door->bImportant) && Blackboard->GetValueAsObject("OpenedDoor") == nullptr)
        {
            Blackboard->SetValueAsObject("OpenedDoor", Door);
            RaiseDetection(40);
        }
    }

    //Door was not found in memory
    else
        Memory.Doors.Add(Door);
}

void AStaffAIController::HandleRadioEvent(FRadioEvent* RadioEvent)
{
    switch (RadioEvent->RadioEvent)
    {
        //Alert
    case ERadioEvent::Radio_Alert:
        Blackboard->SetValueAsVector("LoudNoiseLocation", RadioEvent->Location);
        RaiseVocalStatus(EVocalStatus::Vocal_Alert);
        InheritPlayerMemory(Cast<AStaffAIController>(RadioEvent->NPC->GetController()));
        break;

        //Engage
    case ERadioEvent::Radio_Engage:
        Blackboard->SetValueAsVector("LoudNoiseLocation", RadioEvent->Location);
        Blackboard->ClearValue("InitialLastSeen");
        Blackboard->ClearValue("PlayerLastSeen");
        RaiseVocalStatus(EVocalStatus::Vocal_Engaging);
        InheritPlayerMemory(Cast<AStaffAIController>(RadioEvent->NPC->GetController()));
        break;
    }
}

void AStaffAIController::MarkSearchLocationSearched(ASearchLocation* SearchLocation)
{
    //Error handling
    if (SearchLocation == nullptr)
        return;
    
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

void AStaffAIController::RemoveRoomSearchLocations(ARoomVolume* RoomVolume)
{
    TArray<AActor*> SearchLocations;
    RoomVolume->GetOverlappingActors(SearchLocations, ASearchLocation::StaticClass());

    for (int i = 0; i < SearchLocations.Num(); i++)
    {
        SearchedLocations.Remove(Cast<ASearchLocation>(SearchLocations[i]));
    }
}

void AStaffAIController::InheritPlayerMemory(AStaffAIController* OtherNPC)
{
    //Iterate through OtherNPC memory
    for (int i = 0; i < OtherNPC->Memory.Players.Num(); i++)
    {
        //If player detection is above cautious
        if (OtherNPC->Memory.Players[i].Detection >= 40.0f)
        {
            bool bFound = false;
            //Iterate through memory
            for (int a = 0; a < Memory.Players.Num(); a++)
            {
                if (OtherNPC->Memory.Players[i].Actor == Memory.Players[a].Actor)
                {
                    bFound = true;
                    if (OtherNPC->Memory.Players[i].Detection > Memory.Players[a].Detection)
                    {
                        Memory.Players[a].Detection = OtherNPC->Memory.Players[i].Detection;
                    }
                    break;
                }
            }

            if (!bFound)
                Memory.Players.Add(FTrackedCharacter(OtherNPC->Memory.Players[i].Actor, OtherNPC->Memory.Players[i].Location, OtherNPC->Memory.Players[i].Detection));
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
            //If detection is above 40 and not 90
            if (MemoryDetection > 40 && MemoryDetection != 90)
            {
                //Decay calculations
                DecayStep = DetectionRate * DeltaTime;

                //Update memory detection
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
            //If stimuli is not tracked
            if (!Memory.Players[i].LastSensedStimuli.WasSuccessfullySensed())
            {
                //Remove blackboard tracker
                if (Memory.Players[i].Actor == Blackboard->GetValueAsObject("LastPlayer"))
                    Blackboard->ClearValue("LastPlayer");
                const int MemoryDetection = round(Memory.Players[i].Detection);
                //If detection is above 40 and not 90
                if (MemoryDetection > 40 && MemoryDetection != 90)
                {
                    //Decay calculations
                    DecayStep = DetectionRate * DeltaTime;

                    //Update memory detection
                    Memory.Players[i].Detection -= DecayStep;
                    //Clamp if needed
                    if (Memory.Players[i].Detection < 0)
                        Memory.Players[i].Detection = 0;
                }
                break;
            }
        }
    }
    //If decay step is not zero and no player is being tracked
    if (DecayStep != 0 && Blackboard->GetValueAsObject("LastPlayer") == nullptr)
    {
        //If detection is above 40 and not 90
        float BlackboardDetection = round(Blackboard->GetValueAsFloat("Detection"));
        if (BlackboardDetection > 40 && BlackboardDetection != 90)
        {
            //Update blackboard detection
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

    //Get all cameras
    TArray<AActor*> Cameras;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACamera::StaticClass(), Cameras);

    //Add cameras to memory
    for (int i = 0; i < Cameras.Num(); i++)
    {
        Memory.Cameras.Add(Cast<ACamera>(Cameras[i]));
    }

    //Iterate through all doors
    TArray<AActor*> Doors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADoor::StaticClass(), Doors);

    for (int i = 0; i < Doors.Num(); i++)
    {
        //If door is important
        if (Cast<ADoor>(Doors[i])->bImportant)
            //Save door to memory
            Memory.Doors.Add(FTrackedDoor(Cast<ADoor>(Doors[i])));
    }
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

    //Disable backup if not set
    if (Blackboard->GetValueAsBool("BackupAvailable") && !OtherNPCBlackboard->GetValueAsBool("BackupAvailable"))
    {
        Blackboard->SetValueAsBool("BackupAvailable", false);
    }
    
    //If other staff is using cameras
    if (OtherNPCBlackboard->GetValueAsBool("UsingCameras"))
        Blackboard->SetValueAsBool("CamerasActive", true);

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

        //If both staff is going to man the cameras
        else if (ActionStatus == EActionStatus::Action_CameraHub)
        {
            Blackboard->SetValueAsBool("CamerasActive", false);
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
                        MarkSearchLocationSearched(Cast<ASearchLocation>(Blackboard->GetValueAsObject("TempObject")));
                        Blackboard->ClearValue("TempObject");
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
            {
                Blackboard->SetValueAsObject("UnconsciousNPC", UnconsciousNPC);
                return true;
            }

            if (Cast<AStaffAIController>(Cast<APawn>(CurrentActor)->GetController()) != nullptr)
                InheritPlayerMemory(Cast<AStaffAIController>(Cast<APawn>(CurrentActor)->GetController()));
        }

        //Movement
        if (CurrentStimulus.Tag == "Movement" && Blackboard->GetValueAsFloat("Detection") < 90)
        {
            //Get which rooms actor is inside
            TArray<AActor*> Rooms;
            CurrentActor->GetOverlappingActors(Rooms, ARoomVolume::StaticClass());

            //If there is no room
            if (Rooms.Num() == 0)
            {
                Blackboard->SetValueAsVector("NoiseLocation", CurrentStimulus.StimulusLocation);
                return true;
            }
            
            for (int i = 0; i < Rooms.Num(); i++)
            {
                //If the room isn't public
                if (!Cast<ARoomVolume>(Rooms[i])->bPublic)
                {
                    Blackboard->SetValueAsVector("NoiseLocation", CurrentStimulus.StimulusLocation);
                    return true;
                }
            }
        }
    }

    //Nothing could be perceived
    return false;
}
