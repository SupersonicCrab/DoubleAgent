// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Camera.generated.h"

UCLASS()
class PROJECTSPY_API ACamera : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACamera();

	//Setter for blueprints
	UFUNCTION(BlueprintNativeEvent)
	void GetPerceptionLocationRotation(FVector& OutLocation, FRotator& OutRotation) const;

	//Overriding base function for perception
	virtual void GetActorEyesViewPoint(FVector& OutLocation, FRotator& OutRotation) const override;
};
