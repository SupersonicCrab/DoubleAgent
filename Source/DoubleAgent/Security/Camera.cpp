// Please don't steal


#include "Camera.h"

#include <string>

#include "CameraHub.h"

#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values
ACamera::ACamera(){
    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
    
    UStaticMesh* Mesh = LoadObject<UStaticMesh>(NULL, TEXT("StaticMesh'/Game/Art/Meshes/Security/Camera/S_CameraArm.S_CameraArm'"));
    CameraArmStaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CameraArmStaticMesh"));
    CameraArmStaticMesh->SetStaticMesh(Mesh);
    CameraArmStaticMesh->SetupAttachment(RootComponent);

    Mesh = LoadObject<UStaticMesh>(NULL, TEXT("StaticMesh'/Game/Art/Meshes/Security/Camera/S_Camera.S_Camera'"));
    CameraStaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CameraStaticMesh"));
    CameraStaticMesh->SetStaticMesh(Mesh);
    CameraStaticMesh->SetupAttachment(CameraArmStaticMesh);
    CameraStaticMesh->SetRelativeLocation(FVector(-20,0,6));

    CaptureComponent = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("CaptureComponent"));
    CaptureComponent->SetupAttachment(CameraStaticMesh);
    CaptureComponent->SetRelativeScale3D(FVector(0.25, 0.25, 0.25));
    CaptureComponent->SetRelativeRotation(FRotator(0,180,0));
    CaptureComponent->SetRelativeLocation(FVector(-10, 0, 7));
    CaptureComponent->FOVAngle = 90.0f;
    CaptureComponent->bCaptureEveryFrame = false;

    bReplicates = true;
}

//Base implementation if not overridden
void ACamera::GetPerceptionLocationRotation_Implementation(FVector& OutLocation, FRotator& OutRotation) const{
    OutLocation = GetActorLocation();
    OutRotation = GetActorRotation();
}

//Use override function as new eyes view point
void ACamera::GetActorEyesViewPoint(FVector& OutLocation, FRotator& OutRotation) const{
    GetPerceptionLocationRotation(OutLocation, OutRotation);
}

bool ACamera::CanBeSeenFrom(const FVector& ObserverLocation, FVector& OutSeenLocation,
    int32& NumberOfLoSChecksPerformed, float& OutSightStrength, const AActor* IgnoreActor) const{
    //Raycast to actor location
    FHitResult HitResult;
    NumberOfLoSChecksPerformed++;
    //Return true if raycast hit actor
    if (!GetWorld()->LineTraceSingleByChannel(HitResult, ObserverLocation, GetActorLocation() , ECollisionChannel(ECC_Visibility), FCollisionQueryParams( FName(TEXT("ActorLOS")), true, IgnoreActor)) || HitResult.Actor->IsOwnedBy(this)){
        OutSeenLocation = GetActorLocation();
        OutSightStrength = 1;

        return true;
    }

    //Raycast to center of camera
    NumberOfLoSChecksPerformed++;
    //Return true if raycast hit actor
    if (!GetWorld()->LineTraceSingleByChannel(HitResult, ObserverLocation, GetCenterLocation() , ECollisionChannel(ECC_Visibility), FCollisionQueryParams(FName(TEXT("CenterLOS")), true, IgnoreActor)) || HitResult.Actor->IsOwnedBy(this)){
        OutSeenLocation = GetActorLocation();
        OutSightStrength = 1;

        return true;
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
        CaptureComponent->CaptureScene(); //Then update the scene capture
        UKismetSystemLibrary::K2_SetTimer(this, "CaptureTick", (1.0f/CaptureFPS), false); //Then call this function again according to the CaptureFPS
    }
}

void ACamera::Rotate(int Direction){ //Takes in a direction of either -1 (Left) or +1 (Right)
    int Temp = static_cast<int>(CameraStaticMesh->GetRelativeRotation().Yaw / RotateAmount); //Gets the current rotation of the camera and divide it by the direction times the rotation amount
    Temp += Direction;
    Temp *= RotateAmount;
    if(Direction > 0){ //Checks if the direction is to the right
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
        if(!UKismetSystemLibrary::K2_IsTimerActive(nullptr, "HandleAutoRotate") && bAutoRotate){
            UKismetSystemLibrary::K2_SetTimer(nullptr, "HandleAutoRotate", AutoRotateDelay, false);
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
            //CHRIS FINISH THIS CODE, IT'S YOUR AI STUFF <3 Luv ya ;*
        }
    }
}

void ACamera::SetTargetRotation(){ //Helper Function
    TargetYaw = static_cast<int>(CameraStaticMesh->GetRelativeRotation().Yaw); //Set the target rotation to the current rotation
    if(RightYawLimit == 0 && LeftYawLimit == 0){ // If both the right limit and the left limit are 0
        RightYawLimit = LeftYawLimit = TargetYaw; //Set them both to the target rotation (which is the current rotation)
    }
}

void ACamera::NetRotate_Implementation(int Direction){
    Rotate(Direction);
}

void ACamera::NetRequestRotate_Implementation(int Direction){
    NetRotate(Direction);
}

FVector ACamera::GetCenterLocation_Implementation() const{
    return GetActorLocation();
}

void ACamera::TestRotate(){
    int Temp = static_cast<int>(CameraStaticMesh->GetRelativeRotation().Yaw / RotateAmount); //Gets the current rotation of the camera and divide it by the direction times the rotation amount
    Temp = Temp+Directionn;
    Temp = Temp*RotateAmount;
    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Magenta, FString::Printf(TEXT("Temp: %i"), Temp));
    if(Directionn > 0){ //Checks if the direction is to the right
        if(Temp <= RightYawLimit){ 
            TargetYaw = static_cast<float>(Temp); //If the new target rotation is less than the right limit, then set the target rotation to the new one
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

void ACamera::Tick(float DeltaTime){
    Super::Tick(DeltaTime);
    RotationTick(DeltaTime);
}

void ACamera::BeginPlay(){
    Super::BeginPlay();
    AActor* CameraHub = UGameplayStatics::GetActorOfClass(GetWorld(), ACameraHub::StaticClass());
    AutoRotateDelay = FMath::RandRange(0.5, 10) + AutoRotateDelay;
}
