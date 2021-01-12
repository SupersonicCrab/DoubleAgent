// Please don't steal

#include "Door.h"
#include "AIController.h"
#include "Editor.h"
#include "Player_Character.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Builders/CubeBuilder.h"
#include "Components/BrushComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/TimelineComponent.h"
#include "Engine/Polys.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/GameplayStatics.h"
#include "Tasks/AITask_MoveTo.h"
#include "Kismet/KismetMathLibrary.h"
#include "Sound/SoundBase.h"

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

    OnActorEndOverlap.AddDynamic(this, &ADoor::OnOverlapEnd);
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
    InteractingNPC = NPC;
    
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

void ADoor::OnOverlapEnd(AActor* OverlappedActor, AActor* OtherActor)
{
    //If this wasn't the interacting NPC
    if (InteractingNPC == nullptr || OtherActor != InteractingNPC)
        return;
    
    //Get NPC blackboard
    UBlackboardComponent* Blackboard = UAIBlueprintHelperLibrary::GetBlackboard(OtherActor);
    
    //If blackboard is invalid or detection is above or at 90
    if (Blackboard == nullptr || Blackboard->GetValueAsFloat("Detection") >= 90)
        return;

    //Get overlapping players
    TArray<AActor*> Players;
    GetOverlappingActors(Players, APlayer_Character::StaticClass());

    //If there is players near the door
    if (Players.Num() > 0)
        return;
    
    CloseDoor(OtherActor);
    InteractingNPC = nullptr;
}

void ADoor::OpenDoor_Implementation(AActor* Interactor)
{
    //Get all overlapping characters
    TArray<AActor*> Characters;
    GetOverlappingActors(Characters, ABaseCharacter_CHARACTER::StaticClass());

    //If the door is actually closed and there isn't an animation playing
    if (!bDoorOpen && DoorTimeline == NULL && (!HasMovingAgents() || Interactor->IsA(AAICharacterBase_CHARACTER::StaticClass())))
    {
        //Lock smart link
        SetSmartLinkEnabled(false);
        
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
        
        //Play timeline
        DoorTimeline->PlayFromStart();
        bDoorOpen = true;

        //Play sound
        USoundBase* SoundCue = LoadObject<USoundBase>(NULL, TEXT("SoundCue'/Game/Audio/SoundEffects/Door/Wooden/WoodenDoorOpen_Cue.WoodenDoorOpen_Cue'"));
        UGameplayStatics::PlaySoundAtLocation(GetWorld(), SoundCue, GetActorLocation(), GetActorRotation(), FMath::RandRange(0.9f, 1.0f), 1.0f, 0.0f, nullptr, nullptr, this);
    }
}

void ADoor::CloseDoor_Implementation(AActor* Interactor)
{
    //If door is actually open and there isn't an animation playing
    if (bDoorOpen && DoorTimeline == NULL && (!HasMovingAgents() || (Interactor != nullptr && Interactor->IsA(AAICharacterBase_CHARACTER::StaticClass()))))
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
        SetSmartLinkEnabled(false);
        
        //Play timeline
        DoorTimeline->PlayFromStart();
        bDoorOpen = false;

        //Play sound
        USoundBase* SoundCue = LoadObject<USoundBase>(NULL, TEXT("SoundCue'/Game/Audio/SoundEffects/Door/Wooden/WoodenDoorClose_Cue.WoodenDoorClose_Cue'"));
        UGameplayStatics::PlaySoundAtLocation(GetWorld(), SoundCue, GetActorLocation(), GetActorRotation(), FMath::RandRange(0.9f, 1.0f), 1.0f, 0.0f, nullptr, nullptr, this);
    }
}

void ADoor::UpdateCutout()
{
    //If cutout for this door has not been created
    if (DoorCutout == nullptr)
    {
        //Spawn default brush
        DoorCutout = GEditor->GetEditorWorldContext().World()->SpawnBrush();

        //Set builder to be of type cube builder
        DoorCutout->BrushBuilder = NewObject<UBrushBuilder>(DoorCutout, UCubeBuilder::StaticClass(), NAME_None, RF_Transactional);

        //Setup brush mesh
        DoorCutout->Brush = NewObject<UModel>(DoorCutout, NAME_None, RF_Transactional);
        DoorCutout->Brush->Initialize(DoorCutout, true);
        DoorCutout->GetBrushComponent()->Brush = DoorCutout->Brush;
        DoorCutout->BrushType = EBrushType::Brush_Subtract;

        //Rename brush
        DoorCutout->SetActorLabel("DoorCutout");
    }

    //Get door bounding box
    FBoxSphereBounds Bounds = GetMeshBounds();

    //Set cube to bounding box
    DoorCutout->SetActorLocation(Bounds.Origin);
    UCubeBuilder* CubeBuilder = Cast<UCubeBuilder>(DoorCutout->BrushBuilder);
    CubeBuilder->X = Bounds.BoxExtent.X*2;
    CubeBuilder->Y = Bounds.BoxExtent.Y*2;
    CubeBuilder->Z = Bounds.BoxExtent.Z*2;

    //Build brush
    CubeBuilder->Build(DoorCutout->GetWorld(), DoorCutout);
    DoorCutout->SetNeedRebuild(DoorCutout->GetLevel());
    GEditor->RebuildAlteredBSP();
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

bool ADoor::CanBeSeenFrom(const FVector& ObserverLocation, FVector& OutSeenLocation, int32& NumberOfLoSChecksPerformed,
    float& OutSightStrength, const AActor* IgnoreActor) const
{
    return false;
}
