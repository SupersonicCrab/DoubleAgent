// Fill out your copyright notice in the Description page of Project Settings.


#include "Camera.h"

// Sets default values
ACamera::ACamera()
{
}

//Base implementation if not overridden
void ACamera::GetPerceptionLocationRotation_Implementation(FVector& OutLocation, FRotator& OutRotation) const
{
    OutLocation = GetActorLocation();
    OutRotation = GetActorRotation();
}

//Use override function as new eyes view point
void ACamera::GetActorEyesViewPoint(FVector& OutLocation, FRotator& OutRotation) const
{
    GetPerceptionLocationRotation(OutLocation, OutRotation);
}

