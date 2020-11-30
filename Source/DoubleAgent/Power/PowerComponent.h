// Please don't steal

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PowerComponent.generated.h"

UCLASS()
class DOUBLEAGENT_API APowerComponent : public AActor{
	GENERATED_BODY()
public:	
	// Sets default values for this actor's properties
	APowerComponent();

	UPROPERTY(BlueprintReadWrite)
	bool bPowerOn = true; //This stores whether or not the power for the PowerComponent is on - Defaults to on
};
