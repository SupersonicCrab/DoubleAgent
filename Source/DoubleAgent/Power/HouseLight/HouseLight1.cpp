// Please don't steal


#include "HouseLight1.h"
#include "Components/SpotLightComponent.h"

AHouseLight1::AHouseLight1()
{	
	Light = CreateDefaultSubobject<USpotLightComponent>(TEXT("SpotLight"));
	Light->SetRelativeRotation(FRotator(-90, 0, 0), false);
	Light->SetRelativeLocation(FVector(0,0,10));
	Light->SetupAttachment(RootComponent);
	Light->SetMobility(EComponentMobility::Stationary);

	UStaticMesh* Mesh = LoadObject<UStaticMesh>(NULL, TEXT("StaticMesh'/Game/Art/Meshes/Building/HouseLights/Combined/SM_House_Light_001.SM_House_Light_001'"));
	StaticMesh->SetStaticMesh(Mesh);
	StaticMesh->SetRelativeRotation(FRotator(-180,0,0), false);
	StaticMesh->SetMobility(EComponentMobility::Stationary);
	StaticMesh->SetupAttachment(RootComponent);
}
