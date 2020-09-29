// Fill out your copyright notice in the Description page of Project Settings.


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
    //Shorthand for FName value
    static const FName NAME_AILineOfSight = FName(TEXT("SocketLineOfSight"));

    FHitResult HitResult;

    //Raycast to actor location
    bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, ObserverLocation, GetActorLocation() , ECollisionChannel(ECC_Visibility), FCollisionQueryParams(NAME_AILineOfSight, true, IgnoreActor));

    NumberOfLoSChecksPerformed++;

    //Return true if raycast hit actor
    if (bHit == false || (HitResult.Actor.IsValid() && HitResult.Actor->IsOwnedBy(this)))
    {
        OutSeenLocation = GetActorLocation();
        OutSightStrength = 1;

        return true;
    }

    //Raycast to center of camera
    bHit = GetWorld()->LineTraceSingleByChannel(HitResult, ObserverLocation, GetCenterLocation() , ECollisionChannel(ECC_Visibility), FCollisionQueryParams(NAME_AILineOfSight, true, IgnoreActor));

    NumberOfLoSChecksPerformed++;

    //Return true if raycast hit actor
    if (bHit == false || (HitResult.Actor.IsValid() && HitResult.Actor->IsOwnedBy(this)))
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
    return FVector();
}

