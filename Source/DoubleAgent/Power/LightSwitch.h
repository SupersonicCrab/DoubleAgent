// Please don't steal

#pragma once

#include "CoreMinimal.h"
#include "PowerBox.h"
#include "PowerComponent.h"
#include "Kismet/GameplayStatics.h"
#include "../InteractInterface.h"
#include "LightSwitch.generated.h"

//Forward declaration for RoomVolume.h and HouseLight.cpp
class ARoomVolume;
class AHouseLight;

UCLASS()
class DOUBLEAGENT_API ALightSwitch : public APowerComponent, public IInteractInterface {
	GENERATED_BODY()
	
public:
	//Room that light switch is found in
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ARoomVolume* AssociatedRoom;

	//Whether or not the lights are on in attached room
	UPROPERTY(BlueprintReadWrite)
	bool bLightGroupOn = true;

	//All lights this lightswitch has control over
	UPROPERTY(EditAnywhere)
	TArray<AHouseLight*> Lights;
	
	//Constructor
	ALightSwitch();
	
	UFUNCTION(BlueprintCallable, Server, Reliable)
	void EnableLightGroup(); //This is the function that turns on all the lights in the associated light group

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void DisableLightGroup(bool  bFromPowerBox); //This is the function that turns off all the lights in the associated light group

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastPlaySound(float Pitch);
	
	UFUNCTION(Server, Reliable)
	void RestoreLights(); //This is the function that gets called when the power to the lights was shut off and then turned back on - Instead of just turning all lights back on, it'll check if that light group was on when the power went out

	//Find overlapping room and overlapping lights in room
	UFUNCTION(CallInEditor)
	void FindRoomAndLights();
	
protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;
	
};
