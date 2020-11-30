// Please don't steal

#pragma once

#include "CoreMinimal.h"
#include "LightSwitch.h"
#include "Components/PointLightComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Actor.h"
#include "../AI/RoomVolume.h"
#include "HouseLight.generated.h"

UCLASS()
class DOUBLEAGENT_API AHouseLight : public AActor{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHouseLight();

	UPROPERTY() //If this stops working for some random reason, just set this to visible anywhere
	UPointLightComponent* Light; //The reference to the actual point light component
	UPROPERTY()
	USphereComponent* Sphere;
	UPROPERTY(EditAnywhere)
	int LightGroup;
	UPROPERTY()
	ARoomVolume* Room = nullptr;

	UFUNCTION(CallInEditor)
	void TurnOff();
	UFUNCTION(CallInEditor)
	void TurnOn();
	UFUNCTION()
	void CheckNPC(AActor* NPC);
	UFUNCTION()
	void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
