// Please don't steal

#pragma once

#include "CoreMinimal.h"
#include "Camera.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "CameraHub.generated.h"

UCLASS()
class DOUBLEAGENT_API ACameraHub : public AActor{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACameraHub();
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY()
	UBoxComponent* BoxCollision;
	UPROPERTY(BlueprintReadWrite)
	TArray<ACamera*> Cameras;
	UPROPERTY()
	TArray<bool> CameraAutoDefault;
	UPROPERTY(BlueprintReadWrite)
	bool bPowerOn = true;
	UPROPERTY(BlueprintReadWrite)
	bool bDisplayOn;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<UTextureRenderTarget2D*> TextureTargets;
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
	UFUNCTION(Category="Scene Capture")
	void EnableCapture();
	UFUNCTION(Category="Scene Capture")
    void DisableCapture();
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};
