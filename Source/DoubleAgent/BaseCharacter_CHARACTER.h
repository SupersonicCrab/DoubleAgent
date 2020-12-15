// Please don't steal

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Perception/AISightTargetInterface.h"
#include "Sound/DialogueVoice.h"
#include "BaseCharacter_CHARACTER.generated.h"

UENUM(BlueprintType, meta = (DisplayName = "Visibilty Level", ToolTip = "Amount of light visible on character"))
enum class EVisbilityLevel : uint8
{
	//No light cast on character
	Visibility_None UMETA(DisplayName="None"),
	//One body part visible
	Visibility_1 UMETA(DisplayName="Level1"),
	//Two body parts visible
	Visibility_2 UMETA(DisplayName="Level2"),
	//Three body parts visible
	Visibility_3 UMETA(DisplayName="Level3"),
	//Four body parts visible
	Visibility_4 UMETA(DisplayName="Level4"),
	//Five body parts visible
	Visibility_5 UMETA(DisplayName="Level5"),
	//Six body parts visible
	Visibility_6 UMETA(DisplayName="Level6")
};

UCLASS()
class DOUBLEAGENT_API ABaseCharacter_CHARACTER : public ACharacter, public IAISightTargetInterface
{
	GENERATED_BODY()

	TArray<FName> Sockets;

public:
	// Sets default values for this character's properties
	ABaseCharacter_CHARACTER();

	//Disables CanBeSeenFrom()
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Debug")
	bool BCanBeSeen = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Debug", Replicated)
	EVisbilityLevel Visibility;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default")
	UDialogueVoice* Voice;
	
	//Override base function to add socket locations to raycast
	UFUNCTION(BlueprintCallable)
	virtual bool CanBeSeenFrom(const FVector& ObserverLocation, FVector& OutSeenLocation, int32& NumberOfLoSChecksPerformed, float& OutSightStrength, const AActor* IgnoreActor = nullptr) const override;

	//Animation rpc
	UFUNCTION(BlueprintCallable, NetMulticast, Reliable)
	void NetPlayAnimation(UAnimSequence* AnimationSequence);

	UFUNCTION(BlueprintCallable, NetMulticast, Reliable)
	void NetResumeAnimation();

	UFUNCTION(BlueprintCallable, NetMulticast, Reliable)
	void NetPauseAnimation();

	//Client only rpc
	UFUNCTION(BlueprintCallable, NetMulticast, Reliable)
	void NetPlayAnimationClient(UAnimSequence* AnimationSequence);

	UFUNCTION(BlueprintCallable, NetMulticast, Reliable)
	void NetStopAnimationClient();

	//Base overrides
	virtual void Tick(float DeltaSeconds) override;
};
