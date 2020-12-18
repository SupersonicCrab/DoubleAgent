// Please don't steal

#pragma once

#include "CoreMinimal.h"
#include "AI/AICharacterBase_CHARACTER.h"
#include "Components/ArrowComponent.h"
#include "Components/TimelineComponent.h"
#include "Perception/AISightTargetInterface.h"
#include "Navigation/NavLinkProxy.h"
#include "Door.generated.h"

/**
 * 
 */
UCLASS()
class DOUBLEAGENT_API ADoor : public ANavLinkProxy
{
	GENERATED_BODY()

	ADoor();

	//Functions used by timeline to animate door opening and closing
	UFUNCTION()
	void OpenAnimation();
	UFUNCTION()
	void CloseAnimation();

	//Used to determine animation direction
	float Direction = 1;

	//Timeline for animation
	UTimelineComponent* DoorTimeline;
	
	//NPC opening interaction
	UFUNCTION()
	void NPCInteraction(AActor* NPC, const FVector& Destination);

	//Unlock navmesh access
	UFUNCTION()
    void Unlock();

	//Used to prevent NPCs from breaking 
	void ForceOpenDoor(AActor* Interactor);
	
public:
	//Animation data
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UCurveFloat* OpenCurve;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UCurveFloat* CloseCurve;
	
	//Door mesh reference
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* DoorMesh;

	//Used to determine direction
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UArrowComponent* OpenDirection;

	UFUNCTION(BlueprintCallable, NetMulticast, Reliable)
	void OpenDoor(AActor* Interactor);
	UFUNCTION(BlueprintCallable, NetMulticast, Reliable)
	void CloseDoor(AActor* Interactor);
	
	//Whether or not the door is open
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bDoorOpen = false;

	//Override base function to add timeline functionality
	virtual void Tick(float DeltaTime) override;
};
