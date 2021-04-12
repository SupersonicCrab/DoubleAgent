// Please don't steal

#pragma once

#include "CoreMinimal.h"

#include "LightBase.h"
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
	Light_Point UMETA(DisplayName="Point"),
	Light_Spot UMETA(DisplayName="Spot"),
	Light_Rect UMETA(DisplayName="Rectangle"),
};

UCLASS()
class DOUBLEAGENT_API AHouseLight : public ALightBase{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHouseLight();

	//Static mesh
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* StaticMesh;

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
};