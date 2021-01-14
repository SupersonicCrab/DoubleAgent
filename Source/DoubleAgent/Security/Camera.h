// Please don't steal

#pragma once

#include "CoreMinimal.h"
#include "Perception/AISightTargetInterface.h"
#include "Components/SceneCaptureComponent2D.h"
#include "../Power/PowerComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Camera.generated.h"

//Forward declaration 
class AStaffAIController;

UCLASS()
class DOUBLEAGENT_API ACamera : public APowerComponent, public IAISightTargetInterface{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACamera();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Components")
    UStaticMeshComponent* CameraStaticMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Components")
	UStaticMeshComponent* CameraArmStaticMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Components")
	USceneCaptureComponent2D* CaptureComponent;

	UPROPERTY(Category="Scene Capture", EditAnywhere, BlueprintReadWrite)
	bool UpdatingCapture;
	UPROPERTY(Category="Scene Capture", EditAnywhere, BlueprintReadWrite)
    float CaptureFPS = 7.5;
	UPROPERTY(Category="Rotate", EditAnywhere, BlueprintReadWrite)
	float RightYawLimit = 30;
	UPROPERTY(Category="Rotate", EditAnywhere, BlueprintReadWrite)
	float LeftYawLimit = 30;
	UPROPERTY(Category="Rotate", EditAnywhere, BlueprintReadWrite)
	float CurrentYaw = 0;
	UPROPERTY(Category="Rotate", EditAnywhere, BlueprintReadWrite)
	float TargetYaw;
	UPROPERTY(Category="Rotate", EditAnywhere, BlueprintReadWrite)
	float CurrentRotationSpeed;
	UPROPERTY(Category="Rotate", EditAnywhere)
	float RotationSpeed = 10;
	UPROPERTY(Category="Rotate", EditAnywhere, BlueprintReadWrite)
	float RotateAmount = 1;
	UPROPERTY(Category="Rotate", EditAnywhere, BlueprintReadWrite)
	bool bAutoRotate;
	UPROPERTY(Category="Rotate", EditAnywhere, BlueprintReadWrite)
	float AutoRotateDelay = 1;

	//Overriding base function for perception
	virtual void GetActorEyesViewPoint(FVector& OutLocation, FRotator& OutRotation) const override;
	
	//Override base function to add socket locations to raycast
	UFUNCTION(BlueprintCallable)
    virtual bool CanBeSeenFrom(const FVector& ObserverLocation, FVector& OutSeenLocation, int32& NumberOfLoSChecksPerformed, float& OutSightStrength, const AActor* IgnoreActor = NULL) const override;

	UFUNCTION(Category="Scene Capture", BlueprintCallable)
	void SetCaptureEnabled(bool CaptureOn);
	UFUNCTION(Category="Scene Capture")
	void CaptureTick();
	UFUNCTION(Category="Rotate")
	void Rotate(int Direction_);
	UFUNCTION(Category="Rotate")
	void RotationTick(float DeltaTime_);
	UFUNCTION(Category="Rotate")
	void HandleAutoRotate();
	UFUNCTION(Category="Rotate")
	void DisableAutoRotate();
	UFUNCTION(Category="Rotate", CallInEditor)
	void SetTargetRotation();

	FTimerHandle AutoRotate;
	
	UFUNCTION(Server, BlueprintCallable, Reliable)
	void NetRequestRotate(int Direction_);
	UFUNCTION(NetMulticast, BlueprintCallable, Reliable)
	void NetRotate(int Direction_);

	//Called when perceived actors is updated
	UFUNCTION()
    void OnPerceptionUpdated(const TArray<AActor*>& DetectedActors);

	//Perception configurations
	UAISenseConfig_Sight* SightConfig;
	
	//Perception component
	UAIPerceptionComponent* PerceptionComponent;

	//OperatorNPC
	AStaffAIController* OperatorNPC;

	void PerceptionTick(float DeltaTime);
	
	//Base overrides
	virtual void Tick(float DeltaTime) override;
    virtual void BeginPlay() override;
};
