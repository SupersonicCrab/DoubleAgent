// Please don't steal

#include "Door.h"
#include "AIController.h"
#include "Components/StaticMeshComponent.h"
#include "Components/TimelineComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Tasks/AITask_MoveTo.h"
#include "Kismet/KismetMathLibrary.h"

ADoor::ADoor()
{
    //Setup door mesh
    DoorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorMesh"));
    DoorMesh->SetupAttachment(RootComponent);

    //Setup open direction
    OpenDirection = CreateDefaultSubobject<UArrowComponent>(TEXT("OpenDirection"));
    OpenDirection->SetupAttachment(DoorMesh);

    //Adding NPCInteraction as delegate
    OnSmartLinkReached.AddDynamic(this, &ADoor::NPCInteraction);
    
    //Enable tick
    PrimaryActorTick.bCanEverTick = true; 
}

void ADoor::OpenAnimation()
{
    //Set door rotation to current to curve value
    DoorMesh->SetRelativeRotation(FQuat(FRotator(0.f, Direction*OpenCurve->GetFloatValue(DoorTimeline->GetPlaybackPosition()), 0.f)), false, NULL, ETeleportType::ResetPhysics);
}

void ADoor::CloseAnimation()
{
    //Set door rotation to current to curve value
    DoorMesh->SetRelativeRotation(FQuat(FRotator(0.f, Direction*CloseCurve->GetFloatValue(DoorTimeline->GetPlaybackPosition()), 0.f)), false, NULL, ETeleportType::ResetPhysics);
}

void ADoor::NPCInteraction(AActor* NPC, const FVector& Destination)
{
    //Save NPC
    InteractingNPC = FInteractingNPC(Cast<AAICharacterBase_CHARACTER>(NPC), Destination);
    
    //If door is open
    if (bDoorOpen)
    {
        ResumePathFollowing(NPC);
    }
    //If door is closed
    else
    {
        //Lock smart link
        bSmartLinkIsRelevant = false;
        SetSmartLinkEnabled(false);

        OpenDoor(NPC);
    }
}

void ADoor::UnlockNPC()
{
    //Unlock smart link
    bSmartLinkIsRelevant = true;
    SetSmartLinkEnabled(true);

    //Resume moving
    ResumePathFollowing(InteractingNPC.NPC);
}

void ADoor::OpenDoor(AActor* Interactor)
{
    //Get all overlapping characters
    TArray<AActor*> Characters;
    GetOverlappingActors(Characters, ABaseCharacter_CHARACTER::StaticClass());

    //If the door is actually closed, there isn't an animation playing, and there is only one overlapping character unless its an NPC opening the door
    if (!bDoorOpen && DoorTimeline == NULL && (Interactor->IsA(AAICharacterBase_CHARACTER::StaticClass()) || Characters.Num() == 1))
    {
        //Determine direction
        if (FVector().DotProduct(UKismetMathLibrary::FindLookAtRotation(Interactor->GetActorLocation(), GetActorLocation()).Vector(), OpenDirection->GetForwardVector()) > 0)
            Direction = 1;
        else
            Direction = -1;
        
        //Setup timeline
        FOnTimelineFloat TimelineCallback;
        TimelineCallback.BindUFunction(this, FName("OpenAnimation"));
        DoorTimeline = NewObject<UTimelineComponent>(this, FName("DoorAnimation"));
        DoorTimeline->AddInterpFloat(OpenCurve, TimelineCallback);

        //Set unlock npc as delegate if actor is an NPC
        if (Interactor->IsA(AAICharacterBase_CHARACTER::StaticClass()))
        {
            FOnTimelineEventStatic TimelineFinishedCallback;
            TimelineFinishedCallback.BindUFunction(this, FName("UnlockNPC"));
            DoorTimeline->SetTimelineFinishedFunc(TimelineFinishedCallback);
        }

        DoorTimeline->RegisterComponent();

        //Play timeline
        DoorTimeline->PlayFromStart();
        bDoorOpen = true;
    }
}

void ADoor::CloseDoor(AActor* Interactor)
{
    //Get all overlapping characters
    TArray<AActor*> Characters;
    GetOverlappingActors(Characters, ABaseCharacter_CHARACTER::StaticClass());
    
    //If door is actually open, there isn't an animation playing, and there is only one overlapping character
    if (bDoorOpen && DoorTimeline == NULL && Characters.Num() == 1)
    {
        //Determine direction
        if (FVector().DotProduct(UKismetMathLibrary::FindLookAtRotation(Interactor->GetActorLocation(), GetActorLocation()).Vector(), OpenDirection->GetForwardVector()) > 0)
            Direction = 1;
        else
            Direction = -1;
        
        //Setup timeline
        FOnTimelineFloat TimelineCallback;
        TimelineCallback.BindUFunction(this, FName("CloseAnimation"));
        DoorTimeline = NewObject<UTimelineComponent>(this, FName("DoorAnimation"));
        DoorTimeline->AddInterpFloat(CloseCurve, TimelineCallback);
        DoorTimeline->RegisterComponent();

        //Play timeline
        DoorTimeline->PlayFromStart();
        bDoorOpen = false;
    }
}

void ADoor::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    //If there is an animation to be played
    if (DoorTimeline != NULL)
    {
        DoorTimeline->TickComponent(DeltaTime, ELevelTick::LEVELTICK_TimeOnly, NULL);
        //Delete component after timeline has finished
        if (DoorTimeline->GetPlaybackPosition() == DoorTimeline->GetTimelineLength())
        {
            DoorTimeline->DestroyComponent();
            DoorTimeline = NULL;
        }
    }
}

//Raycasts to the root actor component first, if it cannot hit that it checks DoorHandle and Center socket for extra los checks
bool ADoor::CanBeSeenFrom(const FVector& ObserverLocation, FVector& OutSeenLocation, int32& NumberOfLoSChecksPerformed, float& OutSightStrength, const AActor* IgnoreActor) const
{
    //Return false if nothing was hit
    OutSightStrength = 0;
    return false;
}
