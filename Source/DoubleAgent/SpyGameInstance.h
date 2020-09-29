// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Online.h"
#include "OnlineSubsystem.h"
#include "AdvancedFriendsGameInstance.h"
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

    UFUNCTION(BlueprintCallable)
    static EOnlineStatus GetPlayerOnline(class APlayerController* PlayerController);
};
