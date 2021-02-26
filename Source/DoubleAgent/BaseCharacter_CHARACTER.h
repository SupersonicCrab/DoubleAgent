// Please don't steal

#pragma once

#include "CoreMinimal.h"
#include "Chaos/AABBTree.h"
#include "GameFramework/Character.h"
#include "Perception/AISightTargetInterface.h"
#include "BaseCharacter_CHARACTER.generated.h"

UENUM(BlueprintType)
enum class EGender : uint8
{
	Gender_Masculine UMETA(DisplayName="Masculine"),
	Gender_Feminine UMETA(DisplayName="Feminine"),
};

UENUM(BlueprintType)
enum class EVoiceActor : uint8
{
	VoiceActor_MaleCivilian1,
	VoiceActor_MaleCivilian2,
	VoiceActor_FemaleCivilian1,
	VoiceActor_FemaleCivilian2,
	VoiceActor_MaleWaiter,
	VoiceActor_FemaleWaiter,
	VoiceActor_MaleGuard1,
	VoiceActor_MaleGuard2,
	VoiceActor_FemaleGuard1,
	VoiceActor_FemaleGuard2,
	VoiceActor_MobBoss,
	VoiceActor_Butler,
};

UENUM(BlueprintType)
enum class ESpeechEvent : uint8
{
	SpeechEvent_Idle,
	SpeechEvent_Patrol,
	SpeechEvent_Radio_Chatter,
	SpeechEvent_Action_Cleaning,
	SpeechEvent_Action_BathroomBreak,
	SpeechEvent_Action_IdleTransition,
	SpeechEvent_Cautious,
	SpeechEvent_Action_BodySearch,
	SpeechEvent_Action_NoiseInvestigation,
	SpeechEvent_Action_PowerBox,
	SpeechEvent_Action_CameraHub,
	SpeechEvent_Alert,
	SpeechEvent_Radio_Alert,
	SpeechEvent_Subduing,
	SpeechEvent_Searching,
	SpeechEvent_Engaging,
	SpeechEvent_Radio_Engage,
	SpeechEvent_Action_Restocking,
	SpeechEvent_Action_Backup,
	SpeechEvent_Action_Reloading,
	SpeechEvent_Action_Revive,
};

UENUM(BlueprintType, meta = (DisplayName = "Visibilty Level", ToolTip = "Amount of light visible on character"))
enum class EVisbilityLevel : uint8
{
	//No light cast on character
	Visibility_None UMETA(DisplayName="None"),
	//One body part visible
	Visibility_1 UMETA(DisplayName="Level1"),
	//Two body parts visible
	Visibility_2 UMETA(DisplayName="Level2"),
	//Three body parts visible or just spine
	Visibility_3 UMETA(DisplayName="Level3"),
	//Four body parts visible or just head
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
	bool bCanBeSeen = true;

	//How many sockets are visible from any given light
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Debug", Replicated)
	EVisbilityLevel Visibility;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	bool bSelfLit = false;

	//Speaker
	UPROPERTY(BlueprintReadOnly)
	bool bIsSpeaking = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EGender Gender;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EVoiceActor VoiceActor;

	UFUNCTION(BlueprintCallable)
	virtual void Speak(ESpeechEvent NewSpeech);
	UFUNCTION(BlueprintCallable)
	float GetSpeechTimeRemaining();
	UFUNCTION(BlueprintCallable)
	ESpeechEvent GetCurrentSpeechEvent();
	UFUNCTION(BlueprintCallable)
	void StopSpeaking();	
	
	//Animation rpc
	UFUNCTION(BlueprintCallable, NetMulticast, Reliable)
	void NetPlayAnimation(UAnimSequence* AnimationSequence);
	UFUNCTION(BlueprintCallable, NetMulticast, Reliable)
	void NetResumeAnimation();
	UFUNCTION(BlueprintCallable, NetMulticast, Reliable)
	void NetPauseAnimation();

	//Client only animation rpc
	UFUNCTION(BlueprintCallable, NetMulticast, Reliable)
	void NetPlayAnimationClient(UAnimSequence* AnimationSequence);
	UFUNCTION(BlueprintCallable, NetMulticast, Reliable)
	void NetStopAnimationClient();

	//Base overrides
	virtual void Tick(float DeltaSeconds) override;
	virtual void GetLifetimeReplicatedProps(::TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	UFUNCTION(BlueprintCallable)
	virtual bool CanBeSeenFrom(const FVector& ObserverLocation, FVector& OutSeenLocation, int32& NumberOfLoSChecksPerformed, float& OutSightStrength, const AActor* IgnoreActor = nullptr) const override;
};
