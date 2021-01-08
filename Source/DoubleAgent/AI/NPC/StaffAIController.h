// Please don't steal

#pragma once

#include "CoreMinimal.h"
#include "AIControllerBase.h"
#include "DoubleAgent/Radio.h"
#include "DoubleAgent/AI/RoomVolume.h"
#include "StaffAIController.generated.h"

UENUM(BlueprintType, meta = (DisplayName = "ActionStatus", ToolTip = "Special actions staff can preform, most actions have a limit on the amount of staff that can be performing said action"))
enum class EActionStatus : uint8
{
	//No actions are being performed
	Action_Idle		UMETA(DisplayName = "Idle"),
	//Going to turn back on an lightswitch
	Action_LightSwitch		UMETA(DisplayName = "LightSwitch"),
	//Going to man the camera hub
    Action_CameraHub		UMETA(DisplayName = "CameraHub"),
	//Going to turn back on the power box
    Action_PowerBox		UMETA(DisplayName = "PowerBox"),
	//Going to revive an npc
    Action_Revive		UMETA(DisplayName = "Revive"),
	//Calling for backup on landline
    Action_Backup		UMETA(DisplayName = "Backup"),
	//Patting down player due to suspicious clothing
    Action_BodySearch		UMETA(DisplayName = "BodySearch"),
	//Investigating source of noise that was heard (at or above 40)
    Action_NoiseInvestigation		UMETA(DisplayName = "NoiseInvestigation"),
	//Searching building for player (above or at 90)
    Action_Searching		UMETA(DisplayName = "Searching"),
	//Searching the closest unreached search location (below 40)
    Action_SearchInvestigation		UMETA(DisplayName = "SearchInvestigation"),
	//Generic cleaning task
	Action_Cleaning UMETA(DisplayName = "Cleaning"),
	//Getting ammo from armory
	Action_Restock UMETA(DisplayName = "Restocking"),
	//Reloading
	Action_Reloading UMETA(DisplayName = "Reloading"),
	//Using the washroom
	Action_Washroom UMETA(DisplayName = "Washroom")
};

//A tracked player is any player that has been considered a threat
USTRUCT()
struct FTrackedActor
{
	GENERATED_BODY()

	FTrackedActor(){};
	FTrackedActor(AActor* Actor_, FVector Location_, float Detection_);
	UPROPERTY(EditAnywhere)
	AActor* Actor;
	UPROPERTY(EditAnywhere)
	FVector Location;
	UPROPERTY(EditAnywhere)
	float Detection;
};

USTRUCT()
struct FTrackedDoor
{
	GENERATED_BODY()
	
	FTrackedDoor(){};	
};

//Memory structure used to hold all important actors
USTRUCT()
struct FStaffMemory
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TArray<FTrackedActor> Players;
};

UCLASS()
class DOUBLEAGENT_API AStaffAIController : public AAIControllerBase
{
	GENERATED_BODY()

	//Staff memory of specific actors
	UPROPERTY(EditAnywhere)
	FStaffMemory Memory;

	//Override
	virtual void Tick(float DeltaTime) override;
	void OnPossess(APawn* InPawn) override;
	
	//Detection fall off
	void DetectionDecay(float DeltaTime);
	
public:
	//NPC spotted tick
	virtual void NPCVisionTick(AActor* CurrentActor, FAIStimulus& CurrentStimulus) override;
	void StaffVisionTick(AActor* CurrentActor, FAIStimulus& CurrentStimulus);
	void StaffVisionUpdate(AActor* CurrentActor, FAIStimulus& CurrentStimulus);

	//Process hearing perception
	virtual bool HandleHearing(AActor* CurrentActor, FAIStimulus& CurrentStimulus) override;

	//Process sight perception
	virtual void HandleSightTick(AActor* CurrentActor, FAIStimulus& CurrentStimulus, float DeltaTime) override;
	virtual void HandleSight(AActor* CurrentActor, FAIStimulus& CurrentStimulus) override;
	
	//Player spotted 
	virtual void PlayerVisionTick(AActor* CurrentPlayer, FAIStimulus& CurrentStimulus, float DeltaTime);
	virtual void PlayerVisionUpdate(AActor* CurrentPlayer, FAIStimulus& CurrentStimulus);
	/*Todo
	 *Camera hub
	 *Camera vision
	*/

	//Radio event
	void HandleRadioEvent(FRadioEvent RadioEvent);
	
	//Percent change per second
	UPROPERTY(EditAnywhere)
	float DetectionRate;

	//Searched locations
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<ASearchLocation*> SearchedLocations;

	//Helper functions
	UFUNCTION(BlueprintCallable)
	void MarkSearchLocationSearched(ASearchLocation* SearchLocation);
};
