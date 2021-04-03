// Fill out your copyright notice in the Description page of Project Settings.

#include "BaseCharacter_CHARACTER.h"

#include "DialogueParser.h"
#include "FMODBlueprintStatics.h"
#include "FMODEvent.h"
#include "SpyGameInstance.h"
#include "Animation/AnimInstance.h"
#include "Components/LightComponent.h"
#include "Engine/DemoNetDriver.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Misc/OutputDeviceNull.h"
#include "Perception/AISense_Hearing.h"
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

void ABaseCharacter_CHARACTER::NetSpeak_Implementation(const FString& DialogueLine, int Line)
{
    //DialogueInfo DialogueInfo = DialogueParser::GetDialogueInfo(DialogueLine + FString::FromInt(Line));
    //USpyGameInstance* GameInstance = Cast<USpyGameInstance>(GetGameInstance());
    
    //FMOD programmer sound
    UFMODEvent* DialogueEvent = LoadObject<UFMODEvent>(NULL, TEXT("FMODEvent'/Game/FMOD/Events/NPC_Dialogue.NPC_Dialogue'"));
    VoiceComponent = UFMODBlueprintStatics::PlayEventAttached(DialogueEvent, RootComponent, "", FVector(0), EAttachLocation::SnapToTargetIncludingScale, true, false, true);
    VoiceComponent->SetProgrammerSoundName(DialogueLine + FString::FromInt(Line));
    VoiceComponent->Play();

    //Update animations if needed
    FOutputDeviceNull OutputDeviceNull;
    const TCHAR* CmdAndParams = TEXT("NetSpeaking True");
    CallFunctionByNameWithArguments(CmdAndParams, OutputDeviceNull, nullptr, true);    
}

void ABaseCharacter_CHARACTER::NetRequestSpeak_Implementation(ESpeechEvent NewSpeech)
{
    //Register speech event
    UAISense_Hearing::ReportNoiseEvent(GetWorld(), GetActorLocation(), 1, this, 0, FName("Speech"));

    FMOD::Studio::System* StudioSystem = IFMODStudioModule::Get().GetStudioSystem(EFMODSystemContext::Runtime);
    FMOD_STUDIO_SOUND_INFO SoundInfo = { 0 };

    FString DialogueLine;
    
    if (NewSpeech == ESpeechEvent::Idle)
    {
        DialogueLine = UEnum::GetDisplayValueAsText(VoiceActor).ToString() + UEnum::GetDisplayValueAsText(ESpeechEvent::Common).ToString();
        
        FMOD_RESULT Result = StudioSystem->getSoundInfo(TCHAR_TO_UTF8(*FString(DialogueLine + FString::FromInt(1))), &SoundInfo);

        if (Result != FMOD_OK || UKismetMathLibrary::RandomBoolWithWeight(0.5))
        {
            DialogueLine = UEnum::GetDisplayValueAsText(VoiceActor).ToString() + UEnum::GetDisplayValueAsText(NewSpeech).ToString();
        }
    }
    else
        DialogueLine = UEnum::GetDisplayValueAsText(VoiceActor).ToString() + UEnum::GetDisplayValueAsText(NewSpeech).ToString();
    
    //Find out how many lines there are to choose from
    int Lines;
    for (Lines = 1; Lines < 18; Lines++)
    {
        FMOD_RESULT Result = StudioSystem->getSoundInfo(TCHAR_TO_UTF8(*FString(DialogueLine + FString::FromInt(Lines))), &SoundInfo);
        if (Result != FMOD_OK)
        {
            if (Lines > 1)
                Lines--;
            break;
        }
    }

    //Play random line
    int Line = UKismetMathLibrary::RandomIntegerInRange(1, Lines);

    UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("%s spoke with status %s"),  *AActor::GetDebugName(GetOwner()), *FString(DialogueLine + FString::FromInt(Line))), true, true, FLinearColor::Red);
    
    CurrentSpeechEvent = NewSpeech;
    NetSpeak(DialogueLine, Line);
}

float ABaseCharacter_CHARACTER::GetSpeechTimeRemaining()
{
    //Return -1 if not speaking
    if (VoiceComponent == nullptr || !VoiceComponent->IsPlaying())
        return -1;
    
    return (VoiceComponent->GetLength() - VoiceComponent->GetTimelinePosition())/1000;
}

void ABaseCharacter_CHARACTER::StopSpeaking()
{
    if (VoiceComponent == nullptr)
        return;
    
    VoiceComponent->Stop();
    CurrentSpeechEvent = ESpeechEvent::Idle;
}

bool ABaseCharacter_CHARACTER::CanBeSeenFrom(const FVector& ObserverLocation, FVector& OutSeenLocation, int32& NumberOfLoSChecksPerformed, float& OutSightStrength, const AActor* IgnoreActor) const
{
    if (!bCanBeSeen)
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
            
            //Set strength to 1 if character is not lit and within 4 metres
            if (Visibility == EVisbilityLevel::Visibility_None && FVector().Dist(ObserverLocation, GetMesh()->GetSocketLocation(Sockets[i])) <= 600.0f)
            {
                OutSightStrength = 0.16666666666f;
                return true;
            }
            else
            {
                if (i > 2)
                    return true;
                //Increment strength based on current visibility
                OutSightStrength = OutSightStrength + (static_cast<float>(Visibility)/6);
            }
            
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

    
    //Run on server only
    if (UKismetSystemLibrary::IsServer(GetWorld()))
    {
        //Get all overlapping lights
        TArray<AActor*> Lights;
        GetOverlappingActors(Lights, ALightBase::StaticClass());

        EVisbilityLevel Temp = EVisbilityLevel::Visibility_None;

        if (bSelfLit)
            Temp = EVisbilityLevel::Visibility_3;

        //Iterate through lights
        for (int i = 0; i < Lights.Num(); i++)
        {
            //Iterate through all sockets
            for (int a = 0; a < Sockets.Num(); a++)
            {
                int VisibilityIncrease = 1;
                
                //Special socket scoring
                if (Sockets[a] == "spineSocket")
                    VisibilityIncrease = 2;
                if (Sockets[a] == "headSocket")
                    VisibilityIncrease = 3;

                //Save light cast
                ALightBase* Light = Cast<ALightBase>(Lights[i]);

                //If light is owned by this actor
                if (Light->GetOwner() == this)
                    continue;

                //Error checking
                if (!IsValid(Light->Light))
                    continue;
                
                //Raycast from light to sockets
                FHitResult SocketHitResult;
                GetWorld()->LineTraceSingleByChannel(SocketHitResult, Light->Light->GetComponentLocation(), GetMesh()->GetSocketLocation(Sockets[a]), ECollisionChannel::ECC_Visibility, FCollisionQueryParams(FName(TEXT("SocketLineOfSight")), true, Light->GetOwner()));

                //If light was on and socket was hit
                if (Light->Light->GetVisibleFlag() && SocketHitResult.Actor == this)
                {
                    //If socket is within attenuation radius
                    if (FVector::Dist(GetActorLocation(), Light->Light->GetComponentLocation()) <= Light->Light->GetBoundingSphere().W)
                        Temp = static_cast<EVisbilityLevel>(static_cast<uint8>(Temp) + VisibilityIncrease);
                    //If socket is just within sphere overlap
                    else if (Temp == EVisbilityLevel::Visibility_None)
                        Temp = EVisbilityLevel::Visibility_1;
                }

                //Break loop if visibility is already 6
                if (Temp >= EVisbilityLevel::Visibility_6)
                {
                    Temp = EVisbilityLevel::Visibility_6;
                    i = Lights.Num();
                    break;
                }
            }
        }

        //Update visibility
        Visibility = Temp;
    }
}

void ABaseCharacter_CHARACTER::GetLifetimeReplicatedProps(::TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ABaseCharacter_CHARACTER, Visibility);
    DOREPLIFETIME(ABaseCharacter_CHARACTER, bSelfLit);
    DOREPLIFETIME(ABaseCharacter_CHARACTER, Gender);
    DOREPLIFETIME(ABaseCharacter_CHARACTER, VoiceActor);
}

void ABaseCharacter_CHARACTER::NetStopAnimationClient_Implementation()
{
    //If not server
    if (!UKismetSystemLibrary::IsServer(GetWorld()))
        GetMesh()->GetAnimInstance()->Montage_Stop(0);
}


void ABaseCharacter_CHARACTER::NetPlayAnimationClient_Implementation(UAnimSequence* AnimationSequence, float BlendInTime, float BlendOutTime)
{
    //If not server
    if (!UKismetSystemLibrary::IsServer(GetWorld()))
        //Play montage with default parameters
        GetMesh()->GetAnimInstance()->PlaySlotAnimationAsDynamicMontage(AnimationSequence, "DefaultSlot", BlendInTime, BlendOutTime);
}

void ABaseCharacter_CHARACTER::NetPauseAnimation_Implementation()
{
    GetMesh()->GetAnimInstance()->Montage_Pause();
}

void ABaseCharacter_CHARACTER::NetResumeAnimation_Implementation()
{
    GetMesh()->GetAnimInstance()->Montage_Resume(NULL);
}

void ABaseCharacter_CHARACTER::NetPlayAnimation_Implementation(UAnimSequence* AnimationSequence, float BlendInTime, float BlendOutTime, float InTimeToStartMontageAt)
{
    //Play montage with default parameters
    GetMesh()->GetAnimInstance()->PlaySlotAnimationAsDynamicMontage(AnimationSequence, "DefaultSlot", BlendInTime, BlendOutTime,1,1,-1, InTimeToStartMontageAt);
}