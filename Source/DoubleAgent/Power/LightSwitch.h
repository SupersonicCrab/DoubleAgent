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
	UPROPERTY(EditAnywhere) //This allows the unreal editor to read and edit the property
	ARoomVolume* AssociatedRoom; //This is the room that the light switch is found in
	UPROPERTY(EditAnywhere)
	int LightGroup; //This is the light group that is associated with all the lights in the room and the light switch
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bLightGroupOn = true; //The boolean that tracks if this light group is currently on
	UPROPERTY(EditAnywhere)
	APowerBox* PowerBox; //The pointer to the power box
	UPROPERTY(EditAnywhere, Replicated)
	TArray<AHouseLight*> Lights; //The array of pointers to all the lights in the associated light group
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Interaction") //Setting up the Interact interface to use it's functionality in C++
    void Interact(AActor* Interactor);
	virtual void Interact_Implementation(AActor* Interactor) override;
	
	//Constructor
	ALightSwitch();
	
	UFUNCTION(BlueprintCallable, Server, Reliable)
	void EnableLightGroup(); //This is the function that turns on all the lights in the associated light group

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void DisableLightGroup(bool  bFromPowerBox); //This is the function that turns off all the lights in the associated light group

	UFUNCTION(NetMulticast, Reliable, BlueprintCallable)
	void MulticastEnableLights(AHouseLight* Light);

	UFUNCTION(NetMulticast, Reliable, BlueprintCallable)
    void MulticastDisableLights(AHouseLight* Light);

	UFUNCTION(NetMulticast, Unreliable)
	void PlaySound(float Pitch);
	
	UFUNCTION(Server, Reliable)
	void RestoreLights(); //This is the function that gets called when the power to the lights was shut off and then turned back on - Instead of just turning all lights back on, it'll check if that light group was on when the power went out
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	template<typename T>
	UFUNCTION()
	void FindAllActors(UWorld* World, TArray<T>& Out);

	UFUNCTION(Server, Reliable)
	void HandleAssociatedRoom();
protected:
        // Called when the game starts or when spawned
        virtual void BeginPlay() override;
};
