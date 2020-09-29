// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseCharacter_CHARACTER.h"
#include "Engine/SkeletalMeshSocket.h"

// Sets default values
ABaseCharacter_CHARACTER::ABaseCharacter_CHARACTER()
{
}

bool ABaseCharacter_CHARACTER::CanBeSeenFrom(const FVector& ObserverLocation, FVector& OutSeenLocation, int32& NumberOfLoSChecksPerformed, float& OutSightStrength, const AActor* IgnoreActor) const
{
    //Shorthand for FName value
    static const FName NAME_AILineOfSight = FName(TEXT("SocketLineOfSight"));

    FHitResult HitResult;

    //Raycast to actor location
    const bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, ObserverLocation, GetActorLocation() , ECollisionChannel(ECC_Visibility), FCollisionQueryParams(NAME_AILineOfSight, true, IgnoreActor));

    NumberOfLoSChecksPerformed++;

    //Return true if raycast hit actor
    if (bHit == false || (HitResult.Actor.IsValid() && HitResult.Actor->IsOwnedBy(this)))
    {
        OutSeenLocation = GetActorLocation();
        OutSightStrength = 1;

        return true;
    }

    //Array of sockets to check
    TArray<FName> Sockets;
    Sockets.Add("headSocket");
    Sockets.Add("armLeftSocket");
    Sockets.Add("armRightSocket");
    Sockets.Add("calfLeftSocket");
    Sockets.Add("calfRightSocket");

    //Iterate through all sockets to check LOS
    for (int i = 0; i < Sockets.Num(); i++)
    {
        //Shorthand for FVector
        FVector SocketLocation = GetMesh()->GetSocketLocation(Sockets[i]);

        //Raycast to socket location
        const bool bHitSocket = GetWorld()->LineTraceSingleByChannel(HitResult, ObserverLocation, SocketLocation, ECollisionChannel(ECC_Visibility), FCollisionQueryParams(NAME_AILineOfSight, true, IgnoreActor));

        NumberOfLoSChecksPerformed++;

        //Return true if raycast hit actor
        if (bHitSocket == false || (HitResult.Actor.IsValid() && HitResult.Actor->IsOwnedBy(this))) {
            OutSeenLocation = SocketLocation;
            OutSightStrength = 1;

            return true;
        }
    }

    //Return false if nothing was hit
    OutSightStrength = 0;
    return false;
}

