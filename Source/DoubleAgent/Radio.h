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


USTRUCT(BlueprintType, meta = (ToolTip = "Structure of radio event"))
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
	UPROPERTY(EditAnywhere)
	ERadioEvent RadioEvent;
	//Important location if any
	FVector Location;
	//Delay used for speech
	float Delay = 1.0f;
	//Character that sent the event
	AAICharacterBase_CHARACTER* NPC = nullptr;
};

UCLASS()
class DOUBLEAGENT_API ARadio : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ARadio(){};
};
