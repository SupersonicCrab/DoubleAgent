// Please don't steal


#include "Camera.h"

// Sets default values
ACamera::ACamera()
{
}

//Base implementation if not overridden
void ACamera::GetPerceptionLocationRotation_Implementation(FVector& OutLocation, FRotator& OutRotation) const
{
    OutLocation = GetActorLocation();
    OutRotation = GetActorRotation();
}

//Use override function as new eyes view point
void ACamera::GetActorEyesViewPoint(FVector& OutLocation, FRotator& OutRotation) const
{
    GetPerceptionLocationRotation(OutLocation, OutRotation);
}

bool ACamera::CanBeSeenFrom(const FVector& ObserverLocation, FVector& OutSeenLocation,
    int32& NumberOfLoSChecksPerformed, float& OutSightStrength, const AActor* IgnoreActor) const
{
    //Raycast to actor location
    FHitResult HitResult;
    NumberOfLoSChecksPerformed++;
    //Return true if raycast hit actor
    if (!GetWorld()->LineTraceSingleByChannel(HitResult, ObserverLocation, GetActorLocation() , ECollisionChannel(ECC_Visibility), FCollisionQueryParams( FName(TEXT("ActorLOS")), true, IgnoreActor)) || HitResult.Actor->IsOwnedBy(this))
    {
        OutSeenLocation = GetActorLocation();
        OutSightStrength = 1;

        return true;
    }

    //Raycast to center of camera
    NumberOfLoSChecksPerformed++;
    //Return true if raycast hit actor
    if (!GetWorld()->LineTraceSingleByChannel(HitResult, ObserverLocation, GetCenterLocation() , ECollisionChannel(ECC_Visibility), FCollisionQueryParams(FName(TEXT("CenterLOS")), true, IgnoreActor)) || HitResult.Actor->IsOwnedBy(this))
    {
        OutSeenLocation = GetActorLocation();
        OutSightStrength = 1;

        return true;
    }

    //Return false if nothing was hit
    OutSightStrength = 0;
    return false;
}

FVector ACamera::GetCenterLocation_Implementation() const
{
    return GetActorLocation();
}

