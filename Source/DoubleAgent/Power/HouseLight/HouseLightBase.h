// Please don't steal

#pragma once

#include "CoreMinimal.h"
#include "Components/PointLightComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Actor.h"
#include "HouseLightBase.generated.h"

UCLASS()
class DOUBLEAGENT_API AHouseLightBase : public AActor{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHouseLightBase();

	//Light component
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ULightComponent* Light;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* StaticMesh;

	//Sphere overlap component
	UPROPERTY(EditAnywhere)
	USphereComponent* Sphere;

	UPROPERTY(EditAnywhere)
	float BoundaryMultiplier = 1.4f;
	
	UFUNCTION(NetMulticast, Reliable)
	void TurnOff();
	
	UFUNCTION(NetMulticast, Reliable)
	void TurnOn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};
