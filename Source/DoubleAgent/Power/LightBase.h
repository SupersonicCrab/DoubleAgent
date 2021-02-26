// Please don't steal

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LightBase.generated.h"

class USphereComponent;
UCLASS()
class DOUBLEAGENT_API ALightBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALightBase();

	//Light component
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ULightComponent* Light;
	
	//Sphere overlap component
	UPROPERTY(EditAnywhere)
	USphereComponent* Sphere;
	
	UPROPERTY(EditAnywhere)
	float BoundaryMultiplier = 1.1f;

	UFUNCTION(CallInEditor)
	void UpdateSphere();

	//Base overrides
	virtual void BeginPlay() override;
};
