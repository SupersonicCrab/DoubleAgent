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
class DOUBLEAGENT_API ADoor : public ANavLinkProxy, public IAISightTargetInterface
{
	GENERATED_BODY()

	ADoor();

	//Functions used by timeline to animate door opening and closing
	UFUNCTION()
	void OpenAnimation();
	UFUNCTION()
	void CloseAnimation();
	UFUNCTION(Unreliable, NetMulticast)
	void UpdateRotation(float Yaw);

	//Used to determine animation direction
	float Direction = 1;

	//Timeline for animation
	UTimelineComponent* DoorTimeline;
	
	//NPC interaction
	UFUNCTION()
	void NPCInteraction(AActor* NPC, const FVector& Destination);
	
	//Unlock navmesh access
	UFUNCTION()
    void Unlock();

	//Used to prevent NPCs from breaking 
	void ForceOpenDoor(AActor* Interactor);

	UFUNCTION()
	void OnOverlapEnd(class AActor* OverlappedActor, class AActor* OtherActor);
	
public:
	//Animation data
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UCurveFloat* OpenCurve;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UCurveFloat* CloseCurve;
	
	//Door mesh reference
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	UStaticMeshComponent* DoorMesh;
	
	//Used to determine direction
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UArrowComponent* OpenDirection;

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void OpenDoor(AActor* Interactor);
	UFUNCTION(BlueprintCallable, Server, Reliable)
	void CloseDoor(AActor* Interactor);

	UFUNCTION(NetMulticast, Reliable)
	void PlaySound(USoundBase* Sound);
	
	//NPC interaction
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AAICharacterBase_CHARACTER* InteractingNPC = nullptr;
	
	//Create a bsp brush cutout the size of the doors bounding box
	UFUNCTION(CallInEditor)
	void UpdateCutout();
	UPROPERTY(EditAnywhere)
	ABrush* DoorCutout = nullptr;
	UFUNCTION(BlueprintImplementableEvent)
	FBoxSphereBounds GetMeshBounds();
	
	//Whether or not the door is open
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	bool bDoorOpen = false;

	//Override base functions
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(::TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual bool CanBeSeenFrom(const FVector& ObserverLocation, FVector& OutSeenLocation, int32& NumberOfLoSChecksPerformed, float& OutSightStrength, const AActor* IgnoreActor = nullptr) const override;
};
