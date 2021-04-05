// Please don't steal

#pragma once

#include "CoreMinimal.h"
#include "DialogueParser.h"
#include "GameFramework/PlayerController.h"
#include "SpyPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class DOUBLEAGENT_API ASpyPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent)
	void StartDialogueEvent(FDialogueInfo DialogueInfo);
};
