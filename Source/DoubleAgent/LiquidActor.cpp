// Please don't steal


#include "LiquidActor.h"

#include <string>
#include "Math/UnitConversion.h"

// Sets default values


float ALiquidActor::PingPong(float t, float length)
{
	t = Repeat(t, length * 2);
	return length - FMath::Abs(t - length);
}

void ALiquidActor::UpdateLiquid(const bool& init, const float& VelocityFalloff, const float& FacingScale, const float& VelocitySpeed, const float& MinVelocity, const USceneComponent *Plane, FRotator& PlaneRotator, FVector& LastVelocityOut, FVector& ppFacingOut, FVector& Facing, float& StopElapsedTime, float &MoveElapsedTime, const float deltaTime)
{
	float StopAngularElapsedTime = 0;
	float MoveAngularElapsedTime = 0;
	

	if (init)
	{
		Facing = FVector(0);
		Pos = Plane->GetComponentLocation();
		LastPos = GetActorLocation();
		AngularVelocity = (LastRot - GetActorTransform().GetRotation().Euler());
		AngularVelocity = AngularVelocity.GetClampedToSize(AngularVelocity.Size(),1.0f);
	}
	

	if(FMath::Abs(Velocity.Size()) <= MinVelocity)
	{
		MoveElapsedTime = 0;
		StopElapsedTime += deltaTime;
		Facing = (LastVelocity * (UKismetMathLibrary::Cos(StopElapsedTime * VelocitySpeed)) * FacingScale);
		LastVelocity -= LastVelocity * (VelocityFalloff * 0.01);
		if (LastVelocity.Size() <= 0.05f)
		{
			LastVelocity = FVector(0);
		}
	}
	
	else
	{
		StopElapsedTime = 0;
		MoveElapsedTime += deltaTime;

		//if (MoveElapsedTime * VelocitySpeed < VelocitySpeed)
			//Facing = UKismetMathLibrary::VLerp(Facing, Velocity.GetClampedToSize(Velocity.Size(), 1.0f) * UKismetMathLibrary::FClamp(MoveElapsedTime,0,1) * LastVelocity.Size(), MoveElapsedTime * VelocitySpeed);
		
		//else
			Facing = Velocity.GetClampedToSize(Velocity.Size(), 1.0f) * UKismetMathLibrary::FClamp(MoveElapsedTime * FacingScale,0,1) * LastVelocity.GetClampedToSize(LastVelocity.Size(),1).Size() / FMath::Max(MoveElapsedTime, 1.0f);
			
		LastVelocity = Velocity.GetClampedToSize(Velocity.Size(),1.0f) * FMath::Clamp((MoveElapsedTime * FacingScale) * VelocitySpeed, 0.1f, 1.0f);
		AngularVelocity = FVector(0);
		LastAngularVelocity = FVector(0);
	}

	if(AngularVelocity.Size() <= MinVelocity)
	{
		MoveAngularElapsedTime = 0;
		StopAngularElapsedTime += deltaTime;
		const float t = PingPong(StopAngularElapsedTime * VelocityFalloff,1);
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

	Velocity = FacingScale * ((LastPos - GetActorLocation()) / deltaTime);
	//Velocity = Velocity.GetSafeNormal();
	
	AngularVelocity = (LastRot - GetActorRotation().Euler() / deltaTime);
	AngularVelocity = AngularVelocity.GetClampedToSize(AngularVelocity.Size(), 1.0f);

	
	LastPos = GetActorLocation();
	LastRot = GetActorRotation().Euler();

	LastVelocityOut = LastVelocity;

	//PlaneRotator = UKismetMathLibrary::FindLookAtRotation(Plane->GetComponentLocation(), Plane->GetComponentLocation() - FVector::UpVector * 2 + Facing) + FRotator(90,0,0);
	PlaneRotator = UKismetMathLibrary::FindLookAtRotation(Plane->GetComponentTransform().GetLocation(), Plane->GetComponentTransform().GetLocation() + FVector::UpVector + Facing)+ FRotator(90,0,0);
}
