// Please don't steal

#include "LightBase.h"
#include "Components/LightComponent.h"
#include "Components/SphereComponent.h"

ALightBase::ALightBase()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	
	//Setup sphere
	Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	Sphere->SetupAttachment(RootComponent);
	Sphere->SetVisibility(false);
}

void ALightBase::UpdateSphere()
{
	//Get bounding sphere of attenuation radius
	if (Light == nullptr)
		return;
		
	FSphere BoundingSphere = Light->GetBoundingSphere();

	//Set collision sphere to bounding sphere
	Sphere->SetWorldLocation(BoundingSphere.Center);
	Sphere->SetSphereRadius(BoundingSphere.W * BoundaryMultiplier);
	Sphere->SetVisibility(false);
}

void ALightBase::BeginPlay()
{
	Super::BeginPlay();

	UpdateSphere();
}
