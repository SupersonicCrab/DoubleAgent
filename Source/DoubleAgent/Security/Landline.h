// Please don't steal

#pragma once

#include "CoreMinimal.h"
#include "DoubleAgent/InteractInterface.h"
#include "GameFramework/Actor.h"
#include "DoubleAgent/Power/PowerComponent.h"
#include "Landline.generated.h"

UCLASS()
class DOUBLEAGENT_API ALandline : public APowerComponent, public IInteractInterface{
	GENERATED_BODY()
public:	
	// Sets default values for this actor's properties
	ALandline();

	UStaticMeshComponent* StaticMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bFunctional = true; //This variable is linked to 2 things: When the landline has been used, and when the player has sabotaged the landline. If either of those have happened, the landline will not be functional
	UFUNCTION(BlueprintCallable, CallInEditor, Category="Debugging")
	bool CallBackup();
	UFUNCTION(BlueprintCallable)
	void EnableLandline();
	UFUNCTION(BlueprintCallable)
	void DisableLandline();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Interaction") //Setting up the Interact interface to use it's functionality in C++
    void Interact(AActor* Interactor);
	virtual void Interact_Implementation(AActor* Interactor) override;
};
