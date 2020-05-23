// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseCharacter_CHARACTER_CPP.h"
#include "Engine/SkeletalMeshSocket.h"

// Sets default values
ABaseCharacter_CHARACTER_CPP::ABaseCharacter_CHARACTER_CPP()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ABaseCharacter_CHARACTER_CPP::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABaseCharacter_CHARACTER_CPP::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ABaseCharacter_CHARACTER_CPP::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

bool ABaseCharacter_CHARACTER_CPP::CanBeSeenFrom(const FVector& ObserverLocation, FVector& OutSeenLocation, int32& NumberOfLoSChecksPerformed, float& OutSightStrength, const AActor* IgnoreActor) const
{
    //Shorthand for FName value
    static const FName NAME_AILineOfSight = FName(TEXT("TestPawnLineOfSight"));

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
    TArray<FName> sockets;
    sockets.Add("headSocket");
    sockets.Add("armLeftSocket");
    sockets.Add("armRightSocket");
    sockets.Add("Foot_LSocket");
    sockets.Add("Foot_RSocket");

    //Iterate through all sockets to check LOS
    for (int i = 0; i < sockets.Num(); i++)
    {
        //Shorthand for FVector
        FVector socketLocation = GetMesh()->GetSocketLocation(sockets[i]);

        //Raycast to socket location
        const bool bHitSocket = GetWorld()->LineTraceSingleByChannel(HitResult, ObserverLocation, socketLocation, ECollisionChannel(ECC_Visibility), FCollisionQueryParams(NAME_AILineOfSight, true, IgnoreActor));

        NumberOfLoSChecksPerformed++;

        //Return true if raycast hit actor
        if (bHitSocket == false || (HitResult.Actor.IsValid() && HitResult.Actor->IsOwnedBy(this))) {
            OutSeenLocation = socketLocation;
            OutSightStrength = 1;

            return true;
        }
    }

    //Return false if nothing was hit
    OutSightStrength = 0;
    return false;
}

