// Please don't steal

#include "MovingLight.h"
#include "Components/SpotLightComponent.h"

AMovingLight::AMovingLight()
{
	Light = CreateDefaultSubobject<USpotLightComponent>(TEXT("SpotLight"));
	Light->SetupAttachment(RootComponent);
}

void AMovingLight::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	UpdateSphere();
}
