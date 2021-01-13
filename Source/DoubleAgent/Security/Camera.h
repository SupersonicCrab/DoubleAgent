// Please don't steal

#pragma once

#include "CoreMinimal.h"
#include "Perception/AISightTargetInterface.h"
#include "Components/SceneCaptureComponent2D.h"
#include "../Power/PowerComponent.h"
#include "Camera.generated.h"

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
    float CaptureFPS;
	UPROPERTY(Category="Rotate", EditAnywhere, BlueprintReadWrite)
	float RightYawLimit;
	UPROPERTY(Category="Rotate", EditAnywhere, BlueprintReadWrite)
	float LeftYawLimit;
	UPROPERTY(Category="Rotate", EditAnywhere, BlueprintReadWrite)
	float CurrentYaw;
	UPROPERTY(Category="Rotate", EditAnywhere, BlueprintReadWrite)
	float TargetYaw;
	UPROPERTY(Category="Rotate", EditAnywhere, BlueprintReadWrite)
	float CurrentRotationSpeed;
	UPROPERTY(Category="Rotate", EditAnywhere)
	float RotationSpeed;
	UPROPERTY(Category="Rotate", EditAnywhere, BlueprintReadWrite)
	float RotateAmount;
	UPROPERTY(Category="Rotate", EditAnywhere, BlueprintReadWrite)
	bool bAutoRotateDefault;
	UPROPERTY(Category="Rotate", EditAnywhere, BlueprintReadWrite)
	bool bAutoRotate;
	UPROPERTY(Category="Rotate", EditAnywhere, BlueprintReadWrite)
	float AutoRotateDelay;
	
	//Setter for blueprints
	UFUNCTION(BlueprintNativeEvent)
	void GetPerceptionLocationRotation(FVector& OutLocation, FRotator& OutRotation) const;

	//Overriding base function for perception
	virtual void GetActorEyesViewPoint(FVector& OutLocation, FRotator& OutRotation) const override;

	//Blueprint function to set center location
	UFUNCTION(BlueprintNativeEvent)
    FVector GetCenterLocation() const;
	
	// Overide base function to add socket locations to raycast
	UFUNCTION(BlueprintCallable)
    virtual bool CanBeSeenFrom(const FVector& ObserverLocation, FVector& OutSeenLocation, int32& NumberOfLoSChecksPerformed, float& OutSightStrength, const AActor* IgnoreActor = NULL) const override;

	UFUNCTION(Category="Scene Capture", BlueprintCallable)
	void SetCaptureEnabled(bool CaptureOn);
	UFUNCTION(Category="Scene Capture")
	void CaptureTick();
	UFUNCTION(Category="Rotate")
	void Rotate(int Direction);
	UFUNCTION(Category="Rotate")
	void RotationTick(float DeltaTime_);
	UFUNCTION(Category="Rotate")
	void HandleAutoRotate();
	UFUNCTION(Category="Rotate")
	void DisableAutoRotate();
	UFUNCTION(Category="Rotate")
	void SetTargetRotation();

	UFUNCTION(Server, BlueprintCallable, Reliable)
	void NetRequestRotate(int Direction);
	UFUNCTION(NetMulticast, BlueprintCallable, Reliable)
	void NetRotate(int Direction);

	UPROPERTY(Category="Rotate", BlueprintReadWrite, EditAnywhere)
	int Direction;
	UFUNCTION(Category="Rotate", CallInEditor)
    void TestRotate();

	virtual void Tick(float DeltaTime) override;
	
	protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;
};
