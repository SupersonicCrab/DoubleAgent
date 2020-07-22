// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerVolume.h"
#include "LightSwitch.h"
#include "SearchLocation.h"
#include "RoomVolume.generated.h"

//Room type enum
UENUM(BlueprintType, meta = (DisplayName="RoomType", ToolTip="The room type of the selected room volume"))
enum class ERoomType : uint8
{
	//Miscellaneous
	Room_Other       UMETA(DisplayName="Other"),
	//Used for stash and AI interactions
    Room_Bathroom       UMETA(DisplayName="Bathroom"),
	//Used for AI interactions
    Room_Bedroom		UMETA(DisplayName="Bedroom"),
	//Used for AI interactions
    Room_Kitchen		UMETA(DisplayName="Kitchen"),
	//Used for AI interactions
    Room_DiningRoom		UMETA(DisplayName="Dining room"),
	//Used for AI interactions
    Room_Ballroom		UMETA(DisplayName="Ballroom"),
	//Used for AI interactions
    Room_Cellar		UMETA(DisplayName="Wine cellar"),
};

/**
 * Used to dictate "logical rooms" meaning any areas that should be assumed to be cleared after a guard sees another guard clearing such area
 */
UCLASS()
class PROJECTSPY_API ARoomVolume : public ATriggerVolume
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay() override;

public:
	//Constructor
	ARoomVolume();
	
	//Variables
	UPROPERTY(BlueprintReadWrite, Category = "Default", EditAnywhere = "True")
	bool playerSafe;
	
	UPROPERTY(BlueprintReadWrite, Category = "Default", EditAnywhere = "True")
	bool roomLit = true;
	
	UPROPERTY(BlueprintReadWrite, Category = "Default")
	ALightSwitch* lightSwitch = nullptr;

	UPROPERTY(BlueprintReadWrite, Category = "Default", EditAnywhere = "True")
	ERoomType roomType;

	UPROPERTY(BlueprintReadWrite, Category = "Default")
	TArray<ASearchLocation*> searchLocations;
	
	FTimerHandle delayHandle;

	//Overlap functions
	UFUNCTION()
	void OnOverlapBegin(class AActor* OverlappedActor, class AActor* OtherActor);

	UFUNCTION()
	void OnOverlapEnd(class AActor* OverlappedActor, class AActor* OtherActor);
	
	UFUNCTION()
	void UpdateOverlappingActors();

	UFUNCTION(BlueprintCallable)
	void UpdateLight(bool roomLit_);

	UFUNCTION(BlueprintCallable)
	void UpdateNPC(class AActor* NPC);
};
