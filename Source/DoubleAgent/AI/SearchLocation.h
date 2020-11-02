// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SearchLocation.generated.h"

class ARoomVolume;

UCLASS()
class DOUBLEAGENT_API ASearchLocation : public AActor
{
	GENERATED_BODY()
	
public:	
	//Constructor
	ASearchLocation();

	//Room reference
	TArray<ARoomVolume*> RoomVolumes;
};
