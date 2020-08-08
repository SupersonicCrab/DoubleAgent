// Fill out your copyright notice in the Description page of Project Settings.


#include "Door.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/SkeletalMeshSocket.h"

FVector ADoor::GetSocketLocation_Implementation(FName Socket) const
{
    return FVector();
}


FVector ADoor::GetCenterLocation_Implementation() const
{
    return FVector();
}

//Raycasts to the root actor component first, if it cannot hit that it checks DoorHandle and Center socket for extra los checks
bool ADoor::CanBeSeenFrom(const FVector& ObserverLocation, FVector& OutSeenLocation, int32& NumberOfLoSChecksPerformed,
                          float& OutSightStrength, const AActor* IgnoreActor) const
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
    
    //Raycast to center of door frame
    bHit = GetWorld()->LineTraceSingleByChannel(HitResult, ObserverLocation, GetCenterLocation() , ECollisionChannel(ECC_Visibility), FCollisionQueryParams(NAME_AILineOfSight, true, IgnoreActor));
    
    NumberOfLoSChecksPerformed++;
    
    //Return true if raycast hit actor or nothing
    if (bHit==false || (HitResult.Actor.IsValid() && HitResult.Actor->IsOwnedBy(this)))
    {
        OutSeenLocation = GetCenterLocation();
        OutSightStrength = 1;
    
        return true;
    }

    //Array of sockets to check
    TArray<FName> Sockets;
    Sockets.Add("DoorHandle");
    Sockets.Add("Center");

    //Iterate through all sockets to check LOS
    for (int i = 0; i < Sockets.Num(); i++)
    {
        //Shorthand for FVector
        FVector SocketLocation = GetSocketLocation(Sockets[i]);

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
