// Please don't steal


#include "LiquidActor.h"

#include <string>
#include "Kismet/KismetSystemLibrary.h"
#include "Math/UnitConversion.h"

// Sets default values


float ALiquidActor::PingPong(float t, float length)
{
	t = Repeat(t, length * 2);
	return length - FMath::Abs(t - length);
}

void ALiquidActor::UpdateLiquid(const bool& init, const float& RecoveryTime, const float& FacingScale, const float& VelocitySpeed, const float& MinVelocity, const USceneComponent *Plane, FRotator& PlaneRotator, FVector& LastVelocityOut, FVector& ppFacingOut, FVector& Facing, float& StopElapsedTime, float &MoveElapsedTime, const float deltaTime)
{
	float StopAngularElapsedTime = 0;
	float MoveAngularElapsedTime = 0;
	

	if (init)
	{
		Facing = FVector(0);
		Pos = Plane->GetComponentLocation();
		LastPos = GetActorLocation();
		Velocity = (LastPos - GetActorLocation()) / deltaTime;
		Velocity = Velocity.GetClampedToSize(Velocity.Size(), 1.0f);
		AngularVelocity = (LastRot - GetActorTransform().GetRotation().Euler());
		AngularVelocity = AngularVelocity.GetClampedToSize(AngularVelocity.Size(),1.0f);
	}
	

	if(Velocity.Size() <= MinVelocity)
	{
		MoveElapsedTime = 0;
		StopElapsedTime += deltaTime;
		const float t = PingPong(StopElapsedTime * RecoveryTime, 1);
		FVector PPFacing = UKismetMathLibrary::VLerp(-LastVelocity, LastVelocity, t);
		Facing = UKismetMathLibrary::VLerp(PPFacing, FVector(0), UKismetMathLibrary::MapRangeClamped(StopElapsedTime / LastVelocity.Size(),0,1,0,1));
	}
	
	else
	{
		StopElapsedTime = 0;
		MoveElapsedTime += deltaTime;
		Facing = FacingScale * Velocity * MoveElapsedTime * LastVelocity.Size();
		LastVelocity = Velocity * FMath::Clamp(MoveElapsedTime * VelocitySpeed, 0.1f, 1.5f);
		AngularVelocity = FVector(0);
		LastAngularVelocity = FVector(0);
	}

	if(AngularVelocity.Size() <= MinVelocity)
	{
		MoveAngularElapsedTime = 0;
		StopAngularElapsedTime += deltaTime;
		const float t = PingPong(StopAngularElapsedTime * RecoveryTime,1);
		FVector PPFacing = UKismetMathLibrary::VLerp(-LastAngularVelocity,LastAngularVelocity, t);
		Facing += UKismetMathLibrary::VLerp(PPFacing, FVector(0), UKismetMathLibrary::Clamp(StopAngularElapsedTime / LastAngularVelocity.Size(),0,1 ));
	}
	else
	{
		StopAngularElapsedTime = 0;
		MoveAngularElapsedTime += deltaTime;
		LastAngularVelocity = AngularVelocity * FMath::Clamp(MoveAngularElapsedTime * VelocitySpeed, .2f, 1.5f);
	}

	if(LastVelocity.Size() > 0)
	{
		Facing = FVector(
            Facing.X + Facing.Z,
            Facing.Y + Facing.Z,
            0);
	}

	if(LastAngularVelocity.Size() > 0)
	{
		Facing = Facing.RotateAngleAxis(GetActorRotation().Yaw + 90, FVector::UpVector) * Facing;
		Facing = FVector(
            Facing.X + Facing.Z/2,
            Facing.Y + Facing.Z/2,
            0
            );
	}

	Velocity = (LastPos - GetActorLocation()) / deltaTime;
	Velocity = Velocity.GetClampedToSize(Velocity.Size(),1.0f);
	
	AngularVelocity = (LastRot - GetActorRotation().Euler() / deltaTime);
	AngularVelocity = AngularVelocity.GetClampedToSize(AngularVelocity.Size(), 1.0f);

	
	LastPos = GetActorLocation();
	LastRot = GetActorRotation().Euler();

	LastVelocityOut = LastVelocity;

	PlaneRotator = UKismetMathLibrary::FindLookAtRotation(Plane->GetComponentLocation(), Plane->GetComponentLocation() - FVector::UpVector + Facing) + FRotator(90,0,0);

}
