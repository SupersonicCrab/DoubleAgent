// Please don't steal

#pragma once

#include "CoreMinimal.h"
#include "../BaseCharacter_CHARACTER.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AICharacterBase_CHARACTER.generated.h"

UCLASS()
class DOUBLEAGENT_API AAICharacterBase_CHARACTER : public ABaseCharacter_CHARACTER
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AAICharacterBase_CHARACTER();

	//Speaker
	virtual void NetRequestSpeak(ESpeechEvent NewSpeech) override;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bTranquilized;
	
	//Checks if anyone is speaking within hearing distance
	UFUNCTION(BlueprintCallable)
	ABaseCharacter_CHARACTER* GetNearbySpeaker();
	
	//Disable NPC behaviour and perceptions
	UFUNCTION(CallInEditor, BlueprintCallable, Category = "Debug")
	virtual void DisableNPC();

	//Enable NPC behaviour and perceptions
	UFUNCTION(CallInEditor, BlueprintCallable, Category = "Debug")
	virtual void EnableNPC();
	
	// Override base function to set NPC vision to headSocket
	virtual void GetActorEyesViewPoint(FVector& Location, FRotator& Rotation) const override;
};
