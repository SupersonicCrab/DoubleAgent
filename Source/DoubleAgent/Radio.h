// Please don't steal

#pragma once

#include "CoreMinimal.h"

#include "AI/AICharacterBase_CHARACTER.h"
#include "GameFramework/Actor.h"
#include "Radio.generated.h"

UENUM(BlueprintType, meta = (ToolTip = "Type of radio event"))
enum class ERadioEvent : uint8
{
	//Normal conversations
	Radio_Chatter UMETA(DisplayName = "Chatter"),
	//Broadcasting alert state and possible location
    Radio_Alert UMETA(DisplayName = "Alert"),
	//Broadcasting engage state and possible location
    Radio_Engage UMETA(DisplayName = "Engage"),
};


USTRUCT()
struct FRadioEvent
{
	GENERATED_BODY()
	
	FRadioEvent(){};
	FRadioEvent(ERadioEvent RadioEvent_, FVector Location_, float Delay_, AAICharacterBase_CHARACTER* NPC_)
	{
		RadioEvent = RadioEvent_;
		Location = Location_;
		Delay = Delay_;
		NPC = NPC_;
	};
	
	//Type of event
	ERadioEvent RadioEvent;
	//Important location if any
	FVector Location;
	//Delay used for speech
	float Delay;
	//Character that sent the event
	AAICharacterBase_CHARACTER* NPC;
};

UCLASS()
class DOUBLEAGENT_API ARadio : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ARadio();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
