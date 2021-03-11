// Please don't steal

#include "Door.h"
#include "AIController.h"
#include "Editor.h"
#include "FMODBlueprintStatics.h"
#include "Player_Character.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Builders/CubeBuilder.h"
#include "Components/BrushComponent.h"
#include "Components/LightComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/TimelineComponent.h"
#include "Engine/DemoNetDriver.h"
#include "Engine/Polys.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/GameplayStatics.h"
#include "Tasks/AITask_MoveTo.h"
#include "Kismet/KismetMathLibrary.h"
#include "Power/HouseLight.h"
#include "Sound/SoundBase.h"

ADoor::ADoor()
{
    //Setup door mesh
    DoorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorMesh"));
    DoorMesh->SetupAttachment(RootComponent);
    DoorMesh->SetIsReplicated(true);
    DoorMesh->bReplicatePhysicsToAutonomousProxy = false;

    //Setup open direction
    OpenDirection = CreateDefaultSubobject<UArrowComponent>(TEXT("OpenDirection"));
    OpenDirection->SetupAttachment(RootComponent);

    //Adding NPCInteraction as delegate
    OnSmartLinkReached.AddDynamic(this, &ADoor::NPCInteraction);
    
    //Enable tick
    PrimaryActorTick.bCanEverTick = true;

    OnActorEndOverlap.AddDynamic(this, &ADoor::OnOverlapEnd);
}

void ADoor::OpenAnimation()
{
    UpdateRotation(Direction*OpenCurve->GetFloatValue(DoorTimeline->GetPlaybackPosition()));
}

void ADoor::CloseAnimation()
{
    UpdateRotation(Direction*CloseCurve->GetFloatValue(DoorTimeline->GetPlaybackPosition()));
}

void ADoor::UpdateRotation_Implementation(float Yaw)
{
    DoorMesh->SetRelativeRotation(FQuat(FRotator(0.f, Yaw, 0.f)), false, NULL, ETeleportType::TeleportPhysics);
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

    //Clear opened door if self
    UBlackboardComponent* InteractingNPCBlackboard = Cast<AAIController>(InteractingNPC->GetController())->GetBlackboardComponent();
    if (Cast<ADoor>(InteractingNPCBlackboard->GetValueAsObject("OpenedDoor")) == this)
        InteractingNPCBlackboard->ClearValue("OpenedDoor");

    //Clear interactingNPC
    InteractingNPC = nullptr;
    
    //If there is players near the door
    if (Players.Num() > 0)
        return;
    
    CloseDoor(OtherActor);
}

void ADoor::NPCInteraction_Implementation(AActor* NPC, const FVector& Destination)
{
    InteractingNPC = Cast<AAICharacterBase_CHARACTER>(NPC);

    UBlackboardComponent* NPCBlackboard = UAIBlueprintHelperLibrary::GetBlackboard(InteractingNPC);
    if (NPCBlackboard->GetValueAsObject("ClosedDoor") != nullptr && NPCBlackboard->GetValueAsObject("ClosedDoor") == this)
        NPCBlackboard->ClearValue("ClosedDoor");
        
    //If a player tries to interact with the door the same frame the NPC is
    if (DoorTimeline != NULL)
    {
        ForceOpenDoor(NPC);

        NPCBlackboard->SetValueAsVector("NoiseLocation", GetActorLocation());
    }
    
    //If door is closed
    if (!bDoorOpen)
        OpenDoor(NPC);
}

void ADoor::OpenDoor_Implementation(AActor* Interactor)
{
    //Get all overlapping characters
    TArray<AActor*> Characters;
    GetOverlappingActors(Characters, ABaseCharacter_CHARACTER::StaticClass());

    //If the door is actually closed, there isn't an animation playing, and there is no interacting NPC or is an NPC
    if (!bDoorOpen && DoorTimeline == NULL && (InteractingNPC == nullptr || Interactor->IsA(AAICharacterBase_CHARACTER::StaticClass())))
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
        PlaySound(false);
    }
}

void ADoor::CloseDoor_Implementation(AActor* Interactor)
{
    //If door is actually open and there isn't an animation playing
    if (bDoorOpen && DoorTimeline == NULL && (InteractingNPC == nullptr || (Interactor != nullptr && Interactor->IsA(AAICharacterBase_CHARACTER::StaticClass()))))
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
        PlaySound(true);
    }
}

void ADoor::PlaySound_Implementation(bool bOpen)
{
    FFMODEventInstance EventInstance = UFMODBlueprintStatics::PlayEventAtLocation(GetWorld(), DoorEvent, GetActorTransform(), false);
    EventInstance.Instance->setParameterByName(TCHAR_TO_ANSI(*SoundParameter), bOpen, false);
    EventInstance.Instance->start();
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

void ADoor::GetLifetimeReplicatedProps(::TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ADoor, bDoorOpen);
}

bool ADoor::CanBeSeenFrom(const FVector& ObserverLocation, FVector& OutSeenLocation, int32& NumberOfLoSChecksPerformed, float& OutSightStrength, const AActor* IgnoreActor) const
{
    //Setup
    FHitResult HitResult;
    NumberOfLoSChecksPerformed++;
    
    //Get all overlapping lights
    TArray<AActor*> Lights;
    DoorMesh->GetOverlappingActors(Lights, AHouseLight::StaticClass());

    //Iterate through all lights
    for (int i = 0; i < Lights.Num(); i++)
    {
        AHouseLight* Light = Cast<AHouseLight>(Lights[i]);
        //If light is not lit
        if (!Light->Light->IsVisible())
            continue;
        
        //If door is lit
        if (!GetWorld()->LineTraceSingleByChannel(HitResult, Light->Light->GetComponentLocation(), OpenDirection->GetComponentLocation(), ECollisionChannel(ECC_Visibility), FCollisionQueryParams(FName(TEXT("Visibility")), true, Lights[i]))  || HitResult.Actor->IsOwnedBy(this))
        {
            //If door is visible
            if (!GetWorld()->LineTraceSingleByChannel(HitResult, ObserverLocation, OpenDirection->GetComponentLocation(), ECollisionChannel(ECC_Visibility), FCollisionQueryParams(FName(TEXT("CenterLOS")), true, IgnoreActor)) || HitResult.Actor->IsOwnedBy(this))
            {
                OutSeenLocation = OpenDirection->GetComponentLocation();
                OutSightStrength = 1;
                return true;
            }
        }
    }

    //Return false if nothing was hit
    OutSightStrength = 0;
    return false;
}
