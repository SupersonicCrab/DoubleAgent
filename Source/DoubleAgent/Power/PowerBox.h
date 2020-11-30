// Please don't steal

#pragma once

#include "CoreMinimal.h"
#include "DoubleAgent/InteractInterface.h"
#include "GameFramework/Actor.h"
#include "PowerBox.generated.h"

UCLASS()
class DOUBLEAGENT_API APowerBox : public AActor, public IInteractInterface{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APowerBox();

	UPROPERTY(Category="Power States", BlueprintReadWrite)
	bool bLightsOn = true;
	// UPROPERTY()
	// bool bLandlinesOn;
	// UPROPERTY()
	// bool bRadiosOn;
	// UPROPERTY()
	// bool bCamerasOn;
	//HEY STINKY MORON, make sure you come back here and create/include the camera hub
	UPROPERTY(Category="Power States", BlueprintReadWrite)
	bool bPowerCut = false;

	UFUNCTION(BlueprintCallable)
	void TurnLightsOn();
	UFUNCTION(BlueprintCallable)
	void TurnLightsOff();
	// UFUNCTION()
	// void TurnCamerasOff();
	// UFUNCTION()
	// void TurnCamerasOn();
	// UFUNCTION()
	// void TurnLandlinesOn();
	// UFUNCTION()
	// void TurnLandlinesOff();
	// UFUNCTION()
	// void TurnRadiosOn();
	// UFUNCTION()
	// void TurnRadiosOff();
	UFUNCTION(BlueprintCallable)
	void CutPower();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Interaction") //Setting up the Interact interface to use it's functionality in C++
    void Interact(AActor* Interactor);
	virtual void Interact_Implementation(AActor* Interactor) override;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};
