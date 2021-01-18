// Please don't steal

#pragma once

#include "CoreMinimal.h"
#include "Camera.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "DoubleAgent/AI/AICharacterBase_CHARACTER.h"
#include "CameraHub.generated.h"

//Forward declarations
class AStaffAIController;

UCLASS()
class DOUBLEAGENT_API ACameraHub : public APowerComponent, public IAISightTargetInterface{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACameraHub();

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UBoxComponent* BoxCollision;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<ACamera*> Cameras;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<bool> CameraAutoDefault;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bDisplayOn;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<UTextureRenderTarget2D*> TextureTargets;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	USceneComponent* OperatorPosition;
	UPROPERTY()
	USceneComponent* Root;
	UPROPERTY()
	TArray<UStaticMeshComponent*> ScreenMeshes;
	UPROPERTY()
	UStaticMeshComponent* TableMesh;

	UFUNCTION()
    void OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
    void OnComponentOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
	UFUNCTION(Category="Power System")
	void PowerEnabled();
	UFUNCTION(Category="Power System")
	void PowerDisabled();
	UFUNCTION(Category="Scene Capture", CallInEditor)
	void EnableCapture();
	UFUNCTION(Category="Scene Capture", CallInEditor)
    void DisableCapture();

	UFUNCTION(Category="Display", BlueprintCallable)
    void EnableDisplay();
	UFUNCTION(Category="Display", BlueprintCallable)
	void DisableDisplay();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AStaffAIController* OperatorNPC;
	
	UPROPERTY(EditAnywhere)
	UAIPerceptionStimuliSourceComponent* PerceptionStimuliSource;
	
	//Base overrides
	virtual void BeginPlay() override;
	virtual bool CanBeSeenFrom(const FVector& ObserverLocation, FVector& OutSeenLocation, int32& NumberOfLoSChecksPerformed, float& OutSightStrength, const AActor* IgnoreActor = nullptr) const override;
};
