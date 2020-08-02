// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Perception/AISightTargetInterface.h"
#include "Navigation/NavLinkProxy.h"
#include "Door.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTSPY_API ADoor : public ANavLinkProxy, public IAISightTargetInterface
{
	GENERATED_BODY()

public:
	//Blueprint function to set socket locations
	UFUNCTION(BlueprintNativeEvent)
	FVector GetSocketLocation(FName Socket) const;
	
	//Blueprint function to set center location
	UFUNCTION(BlueprintNativeEvent)
    FVector GetCenterLocation() const;
	
	// Overide base function to add socket locations to raycast
	UFUNCTION(BlueprintCallable)
    virtual bool CanBeSeenFrom(const FVector& ObserverLocation, FVector& OutSeenLocation, int32& NumberOfLoSChecksPerformed, float& OutSightStrength, const AActor* IgnoreActor = NULL) const override;
};
