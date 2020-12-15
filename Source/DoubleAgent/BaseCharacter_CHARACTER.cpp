// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseCharacter_CHARACTER.h"
#include "Animation/AnimInstance.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Power/HouseLight.h"

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
            if (Visibility == EVisbilityLevel::Visibility_None && FVector().Dist(ObserverLocation, GetMesh()->GetSocketLocation(Sockets[i])) <= 400.0f)
                OutSightStrength = 1;
            else
                OutSightStrength = OutSightStrength + 1 * static_cast<uint8>(Visibility);
            
            //Return true if strength is at 2
            if (OutSightStrength >= 2)
            {
                OutSightStrength = 2;
                return true;
            }
        }
    }

    //Return true if above 0
    return OutSightStrength > 0;
}

void ABaseCharacter_CHARACTER::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    if (UKismetSystemLibrary::IsServer(GetWorld()))
    {
        TArray<AActor*> Lights;
        //TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
        //ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldDynamic));
        //TArray<AActor*> ActorsToIgnore;
        
        //UKismetSystemLibrary::SphereOverlapActors(GetWorld(), GetActorLocation(), 3000.0f, ObjectTypes, AHouseLight::StaticClass(), ActorsToIgnore, Lights);
        GetOverlappingActors(Lights, AHouseLight::StaticClass());

        EVisbilityLevel Temp = EVisbilityLevel::Visibility_None;
        for (int i = 0; i < Lights.Num(); i++)
        {
            for (int a = 0; a < Sockets.Num(); a++)
            {
                int VisibilityIncrease = 1;
                if (a == 0)
                    VisibilityIncrease = 2;
                if (a == 1)
                    VisibilityIncrease = 3;
                
                FHitResult SocketHitResult;
                GetWorld()->LineTraceSingleByChannel(SocketHitResult, Lights[i]->GetActorLocation(), GetMesh()->GetSocketLocation(Sockets[a]), ECollisionChannel::ECC_Visibility, FCollisionQueryParams(FName(TEXT("SocketLineOfSight")), true, Lights[i]));
                AHouseLight* Light = Cast<AHouseLight>(Lights[i]);
                if (SocketHitResult.Actor == this && Light->Light->GetVisibleFlag())
                {
                    if (Lights[i]->GetDistanceTo(this) <= Light->Light->AttenuationRadius)
                        Temp = static_cast<EVisbilityLevel>(static_cast<uint8>(Temp) + VisibilityIncrease);
                    else if (Temp == EVisbilityLevel::Visibility_None)
                        Temp = EVisbilityLevel::Visibility_1;
                }
                if (Temp > EVisbilityLevel::Visibility_6)
                    Temp = EVisbilityLevel::Visibility_6;
                if (Temp == EVisbilityLevel::Visibility_6)
                {
                    i = Lights.Num();
                    break;
                }
            }
        }

        Visibility = Temp;
    }
}

void ABaseCharacter_CHARACTER::NetStopAnimationClient_Implementation()
{
    //If not server
    if (!UKismetSystemLibrary::IsServer(GetWorld()))
        GetMesh()->GetAnimInstance()->Montage_Stop(NULL);
}


void ABaseCharacter_CHARACTER::NetPlayAnimationClient_Implementation(UAnimSequence* AnimationSequence)
{
    //If not server
    if (!UKismetSystemLibrary::IsServer(GetWorld()))
        //Play montage with default parameters
        GetMesh()->GetAnimInstance()->PlaySlotAnimationAsDynamicMontage(AnimationSequence, "DefaultSlot");
}

void ABaseCharacter_CHARACTER::NetPauseAnimation_Implementation()
{
    GetMesh()->GetAnimInstance()->Montage_Pause();
}

void ABaseCharacter_CHARACTER::NetResumeAnimation_Implementation()
{
    GetMesh()->GetAnimInstance()->Montage_Resume(NULL);
}

void ABaseCharacter_CHARACTER::NetPlayAnimation_Implementation(UAnimSequence* AnimationSequence)
{
    //Play montage with default parameters
    GetMesh()->GetAnimInstance()->PlaySlotAnimationAsDynamicMontage(AnimationSequence, "DefaultSlot");
}

