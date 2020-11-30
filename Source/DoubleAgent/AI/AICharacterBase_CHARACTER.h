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
	AAICharacterBase_CHARACTER(){};

	//Disable NPC behaviour and perceptions
	UFUNCTION(CallInEditor, BlueprintCallable, Category = "Debug")
	virtual void DisableNPC();

	//Enable NPC behaviour and perceptions
	UFUNCTION(CallInEditor, BlueprintCallable, Category = "Debug")
	virtual void EnableNPC();
	
	// Override base function to set NPC vision to headSocket
	virtual void GetActorEyesViewPoint(FVector& Location, FRotator& Rotation) const override;
};
