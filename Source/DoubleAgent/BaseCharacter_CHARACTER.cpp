// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseCharacter_CHARACTER.h"
#include "Engine/SkeletalMeshSocket.h"

// Sets default values
ABaseCharacter_CHARACTER::ABaseCharacter_CHARACTER()
{
    //Important locations on the characters body to check against
    Sockets.Add("spineSocket");
    Sockets.Add("headSocket");
    Sockets.Add("armLeftSocket");
    Sockets.Add("armRightSocket");
    Sockets.Add("calfLeftSocket");
    Sockets.Add("calfRightSocket");
}

bool ABaseCharacter_CHARACTER::CanBeSeenFrom(const FVector& ObserverLocation, FVector& OutSeenLocation, int32& NumberOfLoSChecksPerformed, float& OutSightStrength, const AActor* IgnoreActor) const
{
    if (!BCanBeSeen)
        return false;
        
    //Setup
    OutSeenLocation = FVector(0);
    OutSightStrength = 0;
    FHitResult SocketHitResult;
    
    for (int i = 0; i < Sockets.Num(); i++)
    {
        //Used for debug purposes
        NumberOfLoSChecksPerformed++;

        //If raycast hits nothing or itself
        if (!GetWorld()->LineTraceSingleByChannel(SocketHitResult, ObserverLocation, GetMesh()->GetSocketLocation(Sockets[i]), ECollisionChannel(ECC_Visibility), FCollisionQueryParams(FName(TEXT("SocketLineOfSight")), true, IgnoreActor)) || SocketHitResult.Actor->IsOwnedBy(this))
        {
            //Set location if there already isn't one
            if (OutSeenLocation.IsZero())
                OutSeenLocation = GetMesh()->GetSocketLocation(Sockets[i]);
            //Increment strength
            OutSightStrength++;
            //Return true if strength is at 2
            if (OutSightStrength == 2)
                return true;
        }
    }

    //Return true if above 0
    return OutSightStrength > 0;
}

