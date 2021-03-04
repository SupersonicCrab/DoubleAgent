// Please don't steal

#pragma once

#include "CoreMinimal.h"

#include "FMODAudioComponent.h"
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
	VoiceActor_MaleCivilian1 UMETA(DisplayName="MaleCivilian1"),
	VoiceActor_MaleCivilian2 UMETA(DisplayName="MaleCivilian2"),
	VoiceActor_FemaleCivilian1 UMETA(DisplayName="FemaleCivilian1"),
	VoiceActor_FemaleCivilian2 UMETA(DisplayName="FemaleCivilian2"),
	VoiceActor_MaleWaiter UMETA(DisplayName="MaleWaiter"),
	VoiceActor_FemaleWaiter UMETA(DisplayName="FemaleWaiter"),
	VoiceActor_MaleGuard1 UMETA(DisplayName="MaleGuard1"),
	VoiceActor_MaleGuard2 UMETA(DisplayName="MaleGuard2"),
	VoiceActor_FemaleGuard1 UMETA(DisplayName="FemaleGuard1"),
	VoiceActor_FemaleGuard2 UMETA(DisplayName="FemaleGuard2"),
	VoiceActor_MobBoss UMETA(DisplayName="MobBoss"),
	VoiceActor_Butler UMETA(DisplayName="Butler"),
};

UENUM(BlueprintType)
enum class ESpeechEvent : uint8
{
	SpeechEvent_Idle UMETA(DisplayName="Idle"),
	SpeechEvent_Patrol UMETA(DisplayName="Patrol"),
	SpeechEvent_Radio_Chatter UMETA(DisplayName="RadioChatter"),
	SpeechEvent_Action_Cleaning UMETA(DisplayName="Cleaning"),
	SpeechEvent_Action_BathroomBreak UMETA(DisplayName="BathroomBreak"),
	SpeechEvent_Action_IdleTransition UMETA(DisplayName="IdleTransition"),
	SpeechEvent_Cautious UMETA(DisplayName="Cautious"),
	SpeechEvent_Action_BodySearch UMETA(DisplayName="BodySearch"),
	SpeechEvent_Action_NoiseInvestigation UMETA(DisplayName="NoiseInvestigation"),
	SpeechEvent_Action_PowerBox UMETA(DisplayName="PowerBox"),
	SpeechEvent_Action_CameraHub UMETA(DisplayName="CameraHub"),
	SpeechEvent_Alert UMETA(DisplayName="Alert"),
	SpeechEvent_Radio_Alert UMETA(DisplayName="RadioAlert"),
	SpeechEvent_Subduing UMETA(DisplayName="Subduing"),
	SpeechEvent_Searching UMETA(DisplayName="Searching"),
	SpeechEvent_Engaging UMETA(DisplayName="Engaging"),
	SpeechEvent_Radio_Engage UMETA(DisplayName="RadioEngage"),
	SpeechEvent_Action_Restocking UMETA(DisplayName="Restocking"),
	SpeechEvent_Action_Backup UMETA(DisplayName="Backup"),
	SpeechEvent_Action_Reloading UMETA(DisplayName="Reload"),
	SpeechEvent_Action_Revive UMETA(DisplayName="Revive"),
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
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UFMODAudioComponent* VoiceComponent = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EGender Gender;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EVoiceActor VoiceActor;
	UPROPERTY()
	ESpeechEvent CurrentSpeechEvent;

	UFUNCTION(BlueprintCallable)
	virtual void Speak(ESpeechEvent NewSpeech);
	UFUNCTION(BlueprintCallable)
	float GetSpeechTimeRemaining();
	UFUNCTION(BlueprintCallable)
	void StopSpeaking();	
	
	//Animation rpc
	UFUNCTION(BlueprintCallable, NetMulticast, Reliable)
	void NetPlayAnimation(UAnimSequence* AnimationSequence, float BlendInTime = 0.25, float BlendOutTime = 0.25, float InTimeToStartMontageAt = 0);
	UFUNCTION(BlueprintCallable, NetMulticast, Reliable)
	void NetResumeAnimation();
	UFUNCTION(BlueprintCallable, NetMulticast, Reliable)
	void NetPauseAnimation();

	//Client only animation rpc
	UFUNCTION(BlueprintCallable, NetMulticast, Reliable)
	void NetPlayAnimationClient(UAnimSequence* AnimationSequence, float BlendInTime = 0.25, float BlendOutTime = 0.25);
	UFUNCTION(BlueprintCallable, NetMulticast, Reliable)
	void NetStopAnimationClient();

	//Base overrides
	virtual void Tick(float DeltaSeconds) override;
	virtual void GetLifetimeReplicatedProps(::TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	UFUNCTION(BlueprintCallable)
	virtual bool CanBeSeenFrom(const FVector& ObserverLocation, FVector& OutSeenLocation, int32& NumberOfLoSChecksPerformed, float& OutSightStrength, const AActor* IgnoreActor = nullptr) const override;
};
