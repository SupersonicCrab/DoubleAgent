// Please don't steal

#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Actor.h"
#include "HouseLight.generated.h"

UENUM()
enum class ELightMesh : uint8
{
	//Should be used in large rooms on the ceiling
	Mesh_Ceiling_Large UMETA(DisplayName="Large ceiling"),
    //Should be used on the ceiling
    Mesh_Ceiling UMETA(DisplayName="Ceiling"),
    //Should be used on wall
    Mesh_Wall UMETA(DisplayName="Wall"),
};

UENUM()
enum class ELightType : uint8
{
	//Space efficient
	Light_Spot UMETA(DisplayName="Spot"),
	//Good overall coverage
	Light_Point UMETA(DisplayName="Point"),
	//One direction
	Light_Rect UMETA(DisplayName="Rectangle"),
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

	//Static mesh
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* StaticMesh;
	
	//Sphere overlap component
	UPROPERTY(EditAnywhere)
	USphereComponent* Sphere;
	
	UPROPERTY(EditAnywhere)
	float BoundaryMultiplier = 1.1f;

	UFUNCTION(CallInEditor)
	void UpdateSphere();

	UFUNCTION(CallInEditor)
	void UpdateLight();
	
	UFUNCTION(NetMulticast, Reliable)
    void TurnOff();
	
	UFUNCTION(NetMulticast, Reliable)
    void TurnOn();

	UPROPERTY(EditAnywhere)
	ELightType LightType;

	UPROPERTY(EditAnywhere)
	ELightMesh MeshType;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};