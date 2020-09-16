// Fill out your copyright notice in the Description page of Project Settings.


#include "SpyGameInstance.h"

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
