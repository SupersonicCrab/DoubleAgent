// Please don't steal

#include "Camera.h"
#include "CameraHub.h"
#include "Components/LightComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "DoubleAgent/AI/NPC/StaffAIController.h"
#include "DoubleAgent/Power/HouseLight.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values
ACamera::ACamera(){
    //Setup root
    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

    //Attach mesh's
    UStaticMesh* Mesh = LoadObject<UStaticMesh>(NULL, TEXT("StaticMesh'/Game/Art/Meshes/Security/Camera/S_CameraArm.S_CameraArm'"));
    CameraArmStaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CameraArmStaticMesh"));
    CameraArmStaticMesh->SetStaticMesh(Mesh);
    CameraArmStaticMesh->SetupAttachment(RootComponent);

    Mesh = LoadObject<UStaticMesh>(NULL, TEXT("StaticMesh'/Game/Art/Meshes/Security/Camera/S_Camera.S_Camera'"));
    CameraStaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CameraStaticMesh"));
    CameraStaticMesh->SetStaticMesh(Mesh);
    CameraStaticMesh->SetupAttachment(CameraArmStaticMesh);
    CameraStaticMesh->SetRelativeLocation(FVector(-20,0,6));

    //Setup capture component
    CaptureComponent = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("CaptureComponent"));
    CaptureComponent->SetupAttachment(CameraStaticMesh);
    CaptureComponent->SetRelativeScale3D(FVector(0.25, 0.25, 0.25));
    CaptureComponent->SetRelativeRotation(FRotator(0,180,0));
    CaptureComponent->SetRelativeLocation(FVector(-10, 0, 7));
    CaptureComponent->FOVAngle = 90.0f;
    CaptureComponent->bCaptureEveryFrame = false;
    CaptureComponent->bCaptureOnMovement = false;

    //Network replication
    bReplicates = true;

    //Create default sense config
    SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("Sight Config"));
    
    //Create perception component
    PerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("Perception Component"));
    
    //Setup sight
    SightConfig->SightRadius = 3000.0f;
    SightConfig->LoseSightRadius = 3500.0f;
    SightConfig->PeripheralVisionAngleDegrees = CaptureComponent->FOVAngle/2;
    SightConfig->SetMaxAge(1.0f);
    SightConfig->DetectionByAffiliation.bDetectEnemies = true;
    SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
    SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
    PerceptionComponent->ConfigureSense(*SightConfig);

    //Assigning perception delegate
    PerceptionComponent->OnPerceptionUpdated.AddDynamic(this, &ACamera::OnPerceptionUpdated);

    PerceptionStimuliSource = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(TEXT("Perception stimuliSource"));
}

//Use override function as new eyes view point
void ACamera::GetActorEyesViewPoint(FVector& OutLocation, FRotator& OutRotation) const{
    OutLocation = CaptureComponent->GetComponentLocation();
    OutRotation = CaptureComponent->GetComponentRotation();
}

bool ACamera::CanBeSeenFrom(const FVector& ObserverLocation, FVector& OutSeenLocation, int32& NumberOfLoSChecksPerformed, float& OutSightStrength, const AActor* IgnoreActor) const
{
    //Setup
    FHitResult HitResult;
    NumberOfLoSChecksPerformed++;
    
    //Get all overlapping lights
    TArray<AActor*> Lights;
    GetOverlappingActors(Lights, AHouseLight::StaticClass());

    //Iterate through all lights
    for (int i = 0; i < Lights.Num(); i++)
    {
        AHouseLight* Light = Cast<AHouseLight>(Lights[i]);
        //If light is not lit
        if (!Light->Light->IsVisible())
            continue;
        
        //If camera is lit
        if (!GetWorld()->LineTraceSingleByChannel(HitResult, Light->Light->GetComponentLocation(), CameraStaticMesh->GetComponentLocation() , ECollisionChannel(ECC_Visibility), FCollisionQueryParams(FName(TEXT("Visibility")), true, Lights[i]))  || HitResult.Actor->IsOwnedBy(this))
        {
            //If camera is visible
            if (!GetWorld()->LineTraceSingleByChannel(HitResult, ObserverLocation, CameraStaticMesh->GetComponentLocation() , ECollisionChannel(ECC_Visibility), FCollisionQueryParams(FName(TEXT("CenterLOS")), true, IgnoreActor)) || HitResult.Actor->IsOwnedBy(this))
            {
                OutSeenLocation = CameraStaticMesh->GetComponentLocation();
                OutSightStrength = 1;
                return true;
            }
        }
    }

    //Return false if nothing was hit
    OutSightStrength = 0;
    return false;
}

void ACamera::SetCaptureEnabled(bool CaptureOn){
    UpdatingCapture = CaptureOn;
    if(CaptureOn == true){
        CaptureTick();
    }
}

void ACamera::CaptureTick(){
    if(bPowerOn && UpdatingCapture){ //If the power is on and the cameras are set to update
        CaptureComponent->CaptureSceneDeferred(); //Then update the scene capture
        UKismetSystemLibrary::K2_SetTimer(this, "CaptureTick", UGameplayStatics::GetWorldDeltaSeconds(GetWorld()) * 30, false); //Then call this function again according to the CaptureFPS
    }
}

void ACamera::Rotate(int Direction_){ //Takes in a direction of either -1 (Left) or +1 (Right)
    int Temp = static_cast<int>(CameraStaticMesh->GetRelativeRotation().Yaw / RotateAmount); //Gets the current rotation of the camera and divide it by the direction times the rotation amount
    Temp += Direction_;
    Temp *= RotateAmount;
    if(Direction_ > 0){ //Checks if the direction is to the right
        if(Temp <= RightYawLimit){ 
            TargetYaw = Temp; //If the new target rotation is less than the right limit, then set the target rotation to the new one
        } else{
            TargetYaw = RightYawLimit; //If it isn't less than the right limit, set the target rotation to the right limit
        }
    } else{ //If the direction is not going right, then it's going left
        if(Temp >= LeftYawLimit){ 
            TargetYaw = Temp; //If the new target rotation is greater than the left limit, then set the target rotation to the new one
        } else{
            TargetYaw = LeftYawLimit; //If it isn't greater than the left limit, set the target rotation to the left limit
        }
    }
    DisableAutoRotate(); //This function is only called by the player, so it disables auto rotation
}

void ACamera::RotationTick(float DeltaTime_){
    CurrentYaw =  static_cast<int>(CameraStaticMesh->GetRelativeRotation().Yaw); //Get the current rotation of the camera
    if(CurrentYaw != TargetYaw){ //If the Current rotation is not equal to the target rotation
        if(bAutoRotate){ //Check if auto rotate is on
            CurrentRotationSpeed = (RotationSpeed/2); // If it's rotating automatically, the rotation speed is half
        } else{
            CurrentRotationSpeed = RotationSpeed; //If it isn't rotating automatically, use regular rotation speed
        }
        if(CurrentYaw > TargetYaw){ //Rotate right
            CameraStaticMesh->AddRelativeRotation(FRotator(0,(CurrentRotationSpeed*-1.0),0) * DeltaTime_);
        } else{ //Rotate left
            CameraStaticMesh->AddRelativeRotation(FRotator(0,(CurrentRotationSpeed*1),0) * DeltaTime_);
        }
    } else{ //Handle Auto Rotation
        if(!UKismetSystemLibrary::K2_IsTimerActiveHandle(GetWorld(), AutoRotate) && bAutoRotate){
           AutoRotate = UKismetSystemLibrary::K2_SetTimer(this, "HandleAutoRotate", AutoRotateDelay, false); 
        }
    }
}

void ACamera::HandleAutoRotate(){
   CurrentYaw =  static_cast<int>(CameraStaticMesh->GetRelativeRotation().Yaw); //Get the current rotation of the camera
    if(CurrentYaw != RightYawLimit){ //If the current rotation isn't at the right limit
        TargetYaw = RightYawLimit; //Set the target to the right limit
    } else if (CurrentYaw != LeftYawLimit){ //If it isn't equal to the left limit
        TargetYaw = LeftYawLimit; //Set the target to the left limit
    }
}

void ACamera::DisableAutoRotate(){
    if(&UWorld::IsServer){ //Is this the server?
        if(bAutoRotate){ //Is auto rotate enabled?
            bAutoRotate = false; //Set Auto rotate to be disabled
            if (!GetWorldTimerManager().IsTimerActive(CameraHub->OperatorReactionTimer))
                GetWorldTimerManager().SetTimer(CameraHub->OperatorReactionTimer, CameraHub, &ACameraHub::AutoRotateDisableReaction, 10);
        }
    }
}

void ACamera::SetTargetRotation(){ //Helper Function
    TargetYaw = static_cast<int>(CameraStaticMesh->GetRelativeRotation().Yaw); //Set the target rotation to the current rotation
    if(RightYawLimit == 0 && LeftYawLimit == 0){ // If both the right limit and the left limit are 0
        RightYawLimit = LeftYawLimit = TargetYaw; //Set them both to the target rotation (which is the current rotation)
    }
}

void ACamera::NetRotate_Implementation(int Direction_){
    Rotate(Direction_);
}

void ACamera::NetRequestRotate_Implementation(int Direction_){
    NetRotate(Direction_);
}

void ACamera::OnPerceptionUpdated(const TArray<AActor*>& DetectedActors)
{
    if (CameraHub == nullptr || CameraHub->OperatorNPC == nullptr)
        return;

    TArray<AActor*> temp;
    CameraHub->OperatorNPC->PerceptionComponent->GetCurrentlyPerceivedActors(UAISense_Sight::StaticClass(), temp);
    if (!temp.Contains(CameraHub))
        return;

    //Iterate through detected actors
    for (int a = 0; a < DetectedActors.Num(); a++)
    {
        //Get last sensed stimuli for each sense
        TArray<FAIStimulus> Stimuli = PerceptionComponent->GetActorInfo(*DetectedActors[a])->LastSensedStimuli;
        //Iterate through sense
        for (int i = 0; i < Stimuli.Num(); i++)
        {
            //If stimuli just happened
            if (Stimuli[i].GetAge() == 0 && Stimuli[i].Type.Name == "Default__AISense_Sight")
            {
                CameraHub->OperatorNPC->HandleSight(DetectedActors[a], Stimuli[i]);
            }
        }
    }
}

void ACamera::PerceptionTick(float DeltaTime)
{
    if (CameraHub->OperatorNPC == nullptr)
        return;

    TArray<AActor*> temp;
    CameraHub->OperatorNPC->PerceptionComponent->GetCurrentlyPerceivedActors(UAISense_Sight::StaticClass(), temp);
    if (!temp.Contains(CameraHub))
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
                CameraHub->OperatorNPC->HandleSightTick(DetectedActors[a], Stimuli[i], DeltaTime);
            }
        }      
    }
}

void ACamera::Tick(float DeltaTime){
    Super::Tick(DeltaTime);
    RotationTick(DeltaTime);
    
    //If OperatorNPC is valid
    if (CameraHub != nullptr && CameraHub->OperatorNPC != nullptr)
        PerceptionTick(DeltaTime);
}

void ACamera::BeginPlay(){
    Super::BeginPlay();
    AutoRotateDelay = FMath::RandRange(0, 1) + AutoRotateDelay;

    PerceptionStimuliSource->RegisterForSense(UAISense_Sight::StaticClass());
}
