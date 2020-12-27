// Please don't steal

#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Actor.h"
#include "HouseLight.generated.h"

UENUM()
enum class ELightType : uint8
{
	//Should be used in large rooms on the ceiling
	Light_Ceiling_Large UMETA(DisplayName="LightCeilingLarge"),
	//Spotlight variant of large light
	Light_Ceiling_Large_Spotlight UMETA(DisplayName="LightCeilingLargeSpot"),
    //Should be used on the ceiling
    Light_Ceiling UMETA(DisplayName="LightCeiling"),
    //Should be used on wall
    Light_Wall UMETA(DisplayName="LightWall"),
};

UCLASS()
class DOUBLEAGENT_API AHouseLight : public AActor{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHouseLight();
	
	//Light component
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ULightComponent* Light;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* StaticMesh;
	//Sphere overlap component
	
	UPROPERTY(EditAnywhere)
	USphereComponent* Sphere;
	
	UPROPERTY(EditAnywhere)
	float BoundaryMultiplier = 1.1f;

	UFUNCTION(CallInEditor)
	void SetupSphere();

	UFUNCTION(CallInEditor)
	void ChangeLight();
	
	UFUNCTION(NetMulticast, Reliable)
    void TurnOff();
	
	UFUNCTION(NetMulticast, Reliable)
    void TurnOn();

	UPROPERTY(EditAnywhere)
	ELightType LightType;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};