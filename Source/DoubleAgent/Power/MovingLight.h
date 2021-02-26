// Please don't steal

#pragma once

#include "CoreMinimal.h"
#include "LightBase.h"
#include "MovingLight.generated.h"

/**
 * 
 */
UCLASS()
class DOUBLEAGENT_API AMovingLight : public ALightBase
{
	GENERATED_BODY()

	AMovingLight();

	//Base overrides
	virtual void Tick(float DeltaSeconds) override;
};
