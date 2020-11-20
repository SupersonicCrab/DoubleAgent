// Please don't steal

#include "Door.h"
#include "AIController.h"
#include "Editor.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Builders/CubeBuilder.h"
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
    DoorMesh->SetRelativeRotation(FQuat(FRotator(0.f, Direction*OpenCurve->GetFloatValue(DoorTimeline->GetPlaybackPosition()), 0.f)), false, NULL, ETeleportType::TeleportPhysics);
}

void ADoor::CloseAnimation()
{
    //Set door rotation to current to curve value
    DoorMesh->SetRelativeRotation(FQuat(FRotator(0.f, Direction*CloseCurve->GetFloatValue(DoorTimeline->GetPlaybackPosition()), 0.f)), false, NULL, ETeleportType::TeleportPhysics);
}

void ADoor::NPCInteraction(AActor* NPC, const FVector& Destination)
{
    //If a player tries to interact with the door the same frame the NPC is
    if (DoorTimeline != NULL)
    {
        ForceOpenDoor(NPC);

        UBlackboardComponent* NPCBlackboard = UAIBlueprintHelperLibrary::GetBlackboard(NPC);
        NPCBlackboard->SetValueAsVector("NoiseLocation", GetActorLocation());
    }
    
    //If door is closed
    if (!bDoorOpen)
        OpenDoor(NPC);
}

void ADoor::Unlock()
{
    //Unlock smart link
    bSmartLinkIsRelevant = true;
    SetSmartLinkEnabled(true);
}

void ADoor::ForceOpenDoor(AActor* Interactor)
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
    FOnTimelineEventStatic TimelineFinishedCallback;
    TimelineFinishedCallback.BindUFunction(this, FName("Unlock"));
    DoorTimeline->SetTimelineFinishedFunc(TimelineFinishedCallback);
    DoorTimeline->RegisterComponent();

    //Lock smart link
    bSmartLinkIsRelevant = false;
    SetSmartLinkEnabled(false);
        
    //Play timeline
    DoorTimeline->PlayFromStart();
    bDoorOpen = true;
}

void ADoor::OpenDoor(AActor* Interactor)
{
    //Get all overlapping characters
    TArray<AActor*> Characters;
    GetOverlappingActors(Characters, ABaseCharacter_CHARACTER::StaticClass());

    //If the door is actually closed and there isn't an animation playing
    if (!bDoorOpen && DoorTimeline == NULL && (!HasMovingAgents() || Interactor->IsA(AAICharacterBase_CHARACTER::StaticClass())))
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
        FOnTimelineEventStatic TimelineFinishedCallback;
        TimelineFinishedCallback.BindUFunction(this, FName("Unlock"));
        DoorTimeline->SetTimelineFinishedFunc(TimelineFinishedCallback);
        DoorTimeline->RegisterComponent();

        //Lock smart link
        bSmartLinkIsRelevant = false;
        SetSmartLinkEnabled(false);
        
        //Play timeline
        DoorTimeline->PlayFromStart();
        bDoorOpen = true;
    }
}

void ADoor::CloseDoor(AActor* Interactor)
{
    //If door is actually open and there isn't an animation playing
    if (bDoorOpen && DoorTimeline == NULL && (!HasMovingAgents() || Interactor->IsA(AAICharacterBase_CHARACTER::StaticClass())))
    {
        //Setup timeline
        FOnTimelineFloat TimelineCallback;
        TimelineCallback.BindUFunction(this, FName("CloseAnimation"));
        DoorTimeline = NewObject<UTimelineComponent>(this, FName("DoorAnimation"));
        DoorTimeline->AddInterpFloat(CloseCurve, TimelineCallback);
        FOnTimelineEventStatic TimelineFinishedCallback;
        TimelineFinishedCallback.BindUFunction(this, FName("Unlock"));
        DoorTimeline->SetTimelineFinishedFunc(TimelineFinishedCallback);
        DoorTimeline->RegisterComponent();

        //Lock smart link
        bSmartLinkIsRelevant = false;
        SetSmartLinkEnabled(false);
        
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
