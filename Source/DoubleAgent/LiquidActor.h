// Please don't steal

#pragma once

#include "CoreMinimal.h"

#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/Actor.h"
#include "LiquidActor.generated.h"

UCLASS()
class DOUBLEAGENT_API ALiquidActor : public AActor
{
	GENERATED_BODY()

	FVector Normal = FVector(0);
	FVector Pos = FVector(0);
	FVector LastPos = FVector(0);
	FVector Velocity = FVector(0);
	FVector LastVelocity = FVector(0);
	FVector LastRot = FVector(0);
	FVector AngularVelocity = FVector(0);
	FVector LastAngularVelocity= FVector(0);
		
public:	

	static float Repeat(float t, float length)
	{
		return FMath::Clamp(t - FMath::Floor(t / length) * length, 0.0f, length);
	}

	UFUNCTION(BlueprintPure)
	float PingPong(float t, float length);

	UFUNCTION(BlueprintCallable)
	void UpdateLiquid(const bool& init, const float& VelocityFalloff, const float& FacingScale, const float& VelocitySpeed, const float& MinVelocity, const USceneComponent *Plane, FRotator& PlaneRotator, FVector& LastVelocityOut, FVector& ppFacingOut, FVector& Facing, float& StopElapsedTime, float &MoveElapsedTime, const float deltaTime);
};
