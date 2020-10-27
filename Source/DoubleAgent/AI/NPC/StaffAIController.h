// Please don't steal

#pragma once

#include "CoreMinimal.h"
#include "AIControllerBase.h"
#include "StaffAIController.generated.h"

//A tracked player is any player that has been considered a threat
USTRUCT()
struct FTrackedPlayer
{
	GENERATED_BODY()

	FTrackedPlayer(){}
	FTrackedPlayer(AActor* Actor_, FVector Location_, float Detection_);
	UPROPERTY(EditAnywhere)
	AActor* Actor;
	UPROPERTY(EditAnywhere)
	FVector Location;
	UPROPERTY(EditAnywhere)
	float Detection;
};

USTRUCT()
struct FStaffMemory
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TArray<FTrackedPlayer> Players;
};

UCLASS()
class DOUBLEAGENT_API AStaffAIController : public AAIControllerBase
{
	GENERATED_BODY()

public:
	//NPC spotted tick
	virtual void NPCVisionTick(AActor* CurrentActor, FAIStimulus& CurrentStimulus) override;

	//Process hearing perception
	virtual bool HandleHearing(AActor* CurrentActor, FAIStimulus& CurrentStimulus) override;

	//Process sight perception
	virtual void HandleSightTick(AActor* CurrentActor, FAIStimulus& CurrentStimulus, float DeltaTime) override;
	virtual void HandleSight(AActor* CurrentActor, FAIStimulus& CurrentStimulus) override;
	
	//Player spotted 
	virtual void PlayerVisionTick(AActor* CurrentPlayer, FAIStimulus& CurrentStimulus, float DeltaTime);
	virtual void PlayerVisionUpdate(AActor* CurrentPlayer, FAIStimulus& CurrentStimulus);

	//Percent change per second
	UPROPERTY(EditAnywhere)
	float DetectionRate;

private:
	UPROPERTY(EditAnywhere)
	FStaffMemory Memory;

	//Override
	virtual void Tick(float DeltaTime) override;

	//Detection fall off
	void DetectionDecay(float DeltaTime);
};
