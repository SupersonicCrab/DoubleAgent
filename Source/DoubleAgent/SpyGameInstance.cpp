// Fill out your copyright notice in the Description page of Project Settings.


#include "SpyGameInstance.h"

#include "../../Plugins/Developer/FMODStudio/Source/FMODStudio/Public/FMODStudioModule.h"

void USpyGameInstance::Init(){
    Super::Init();
    if (IFMODStudioModule::IsAvailable()){
        FMOD::Studio::System* StudioSystem = IFMODStudioModule::Get().GetStudioSystem(EFMODSystemContext::Runtime);
        if (StudioSystem){
            StudioSystem->getVCA(TCHAR_TO_UTF8(*FName("vca:/SFX").ToString()), &FXVca);
            StudioSystem->getVCA(TCHAR_TO_UTF8(*FName("vca:/Music").ToString()), &MusicVCA);
            StudioSystem->getVCA(TCHAR_TO_UTF8(*FName("vca:/Voice").ToString()), &VoiceVCA);
            StudioSystem->getVCA(TCHAR_TO_UTF8(*FName("vca:/UI").ToString()), &UIVCA);
            StudioSystem->getVCA(TCHAR_TO_UTF8(*FName("vca:/Master").ToString()), &MasterVCA);
        }
    }
}

EOnlineStatus USpyGameInstance::GetPlayerOnline(APlayerController* PlayerController){
    IOnlineSubsystem* OSystem = IOnlineSubsystem::Get(FName("Steam"));
    if(OSystem == nullptr){
        return EOnlineStatus::eoNotLoggedIn;
    }
    return EOnlineStatus::eoLoggedIn;
    // if(SteamAPI_IsSteamRunning()){
    // return EOnlineStatus::eoLoggedIn;
    // }else
    // {
    //     return EOnlineStatus::eoNotLoggedIn;
    // }
    //ELoginStatus::Type e = OSystem->GetIdentityInterface()->GetLoginStatus(PlayerController->Player->GetUniqueID());
}

void USpyGameInstance::UpdateVCAs(){
    FXVca->setVolume(FXVolume);  
    MusicVCA->setVolume(MusicVolume);
    UIVCA->setVolume(UIVolume);
    VoiceVCA->setVolume(VoiceVolume);
    MasterVCA->setVolume(MasterVolume);
}

void USpyGameInstance::UpdateVCA(float NewVolume, int VolumeType){
    switch(VolumeType){
        case 1:
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Magenta, FString::Printf(TEXT("FXVolume: %f"), NewVolume));
        FXVolume = NewVolume;
        break;
        case 2:
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Magenta, FString::Printf(TEXT("MusicVolume: %f"), NewVolume));
        MusicVolume = NewVolume;
        break;
        case 3:
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Magenta, FString::Printf(TEXT("UIVolume: %f"), NewVolume));
        UIVolume = NewVolume;
        break;
        case 4:
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Magenta, FString::Printf(TEXT("VoiceVolume: %f"), NewVolume));
        VoiceVolume = NewVolume;
        break;
        case 5:
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Magenta, FString::Printf(TEXT(" MasterVolume: %f"), NewVolume));
        MasterVolume = NewVolume;
        break;
        default:
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Magenta, FString::Printf(TEXT("No VolumeType included to alter")));
        break;
    }
    
}
