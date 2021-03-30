// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Online.h"
#include "OnlineSubsystem.h"
#include "AdvancedFriendsGameInstance.h"
#include "../../Plugins/Developer/FMODStudio/Source/FMODStudio/Public/FMOD/fmod_studio.hpp"
#include "UObject/ObjectMacros.h"
#include "SpyGameInstance.generated.h"

/**
 * 
 */
//Displays the online status of the player
UENUM(BlueprintType)
enum class EOnlineStatus : uint8{
    eoLoggedIn = 0 UMETA(DisplayName="Not Logged In"),
    eoNotLoggedIn = 1 UMETA(DisplayName = "Local Player"),
    eoLocal = 2 UMETA(DisplayName = "Logged In")
};

UCLASS()
class DOUBLEAGENT_API USpyGameInstance : public UAdvancedFriendsGameInstance{
	GENERATED_BODY()
public:
    UFUNCTION()
    virtual void Init();
    
    UFUNCTION(BlueprintCallable)
    static EOnlineStatus GetPlayerOnline(class APlayerController* PlayerController);

    UFUNCTION(BlueprintCallable)
    void UpdateVCAs();

    UFUNCTION(BlueprintCallable)
    void UpdateVCA(float NewVolume, int VolumeType);

    FMOD::Studio::VCA* FXVca;    
    FMOD::Studio::VCA* MasterVCA;
    FMOD::Studio::VCA* MusicVCA;
    FMOD::Studio::VCA* VoiceVCA;
    FMOD::Studio::VCA* UIVCA;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bCensorMode;

    UPROPERTY(Category="Audio", meta = (ClampMin = "-80", ClampMax = "10"), EditAnywhere, BlueprintReadWrite)
    float FXVolume;
    UPROPERTY(Category="Audio", meta = (ClampMin = "-80", ClampMax = "10"), EditAnywhere, BlueprintReadWrite)
    float MusicVolume;
    UPROPERTY(Category="Audio", meta = (ClampMin = "-80", ClampMax = "10"), EditAnywhere, BlueprintReadWrite)
    float VoiceVolume;
    UPROPERTY(Category="Audio", meta = (ClampMin = "-80", ClampMax = "10"), EditAnywhere, BlueprintReadWrite)
    float UIVolume;
    UPROPERTY(Category="Audio", meta = (ClampMin = "-80", ClampMax = "10"), EditAnywhere, BlueprintReadWrite)
    float MasterVolume;
};
