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
	MaleCivilian1 UMETA(DisplayName="MaleCivilian1"),
	MaleCivilian2 UMETA(DisplayName="MaleCivilian2"),
	FemaleCivilian1 UMETA(DisplayName="FemaleCivilian1"),
	FemaleCivilian2 UMETA(DisplayName="FemaleCivilian2"),
	MaleWaiter1 UMETA(DisplayName="MaleWaiter1"),
	FemaleWaiter1 UMETA(DisplayName="FemaleWaiter1"),
	MaleGuard1 UMETA(DisplayName="MaleGuard1"),
	MaleGuard2 UMETA(DisplayName="MaleGuard2"),
	FemaleGuard1 UMETA(DisplayName="FemaleGuard1"),
	FemaleGuard2 UMETA(DisplayName="FemaleGuard2"),
	MobBoss UMETA(DisplayName="MobBoss"),
	Butler UMETA(DisplayName="Butler"),
};

UENUM(BlueprintType)
enum class ESpeechEvent : uint8
{
	Common UMETA(DisplayName="Common"),
	Idle UMETA(DisplayName="Idle"),
	Patrol UMETA(DisplayName="Patrol"),
	RadioChatter UMETA(DisplayName="RadioChatter"),
	Cleaning UMETA(DisplayName="Cleaning"),
	BathroomBreak UMETA(DisplayName="BathroomBreak"),
	IdleTransition UMETA(DisplayName="IdleTransition"),
	Cautious UMETA(DisplayName="Cautious"),
	BodySearch UMETA(DisplayName="BodySearch"),
	NoiseInvestigation UMETA(DisplayName="NoiseInvestigation"),
	Powerbox UMETA(DisplayName="Powerbox"),
	CameraHub UMETA(DisplayName="CameraHub"),
	Alert UMETA(DisplayName="Alert"),
	RadioAlert UMETA(DisplayName="RadioAlert"),
	Subduing UMETA(DisplayName="Subduing"),
	Searching UMETA(DisplayName="Searching"),
	Engaging UMETA(DisplayName="Engaging"),
	RadioEngage UMETA(DisplayName="RadioEngage"),
	Restocking UMETA(DisplayName="Restocking"),
	Backup UMETA(DisplayName="Backup"),
	Reload UMETA(DisplayName="Reload"),
	Revive UMETA(DisplayName="Revive"),
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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	EGender Gender;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	EVoiceActor VoiceActor;
	UPROPERTY()
	ESpeechEvent CurrentSpeechEvent;

	UFUNCTION(BlueprintCallable, Server, Reliable)
	virtual void NetRequestSpeak(ESpeechEvent NewSpeech);
	UFUNCTION(BlueprintCallable, NetMulticast, Reliable)
	void NetSpeak(const FString& DialogueLine, int Line);
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
