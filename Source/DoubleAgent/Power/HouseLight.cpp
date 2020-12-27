// Please don't steal

#include "HouseLight.h"
#include "Components/PointLightComponent.h"
#include "Components/RectLightComponent.h"
#include "Components/SpotLightComponent.h"

// Sets default values
AHouseLight::AHouseLight()
{
	//Setup sphere
	Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	Sphere->SetVisibility(false);

	//Setup static mesh
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	StaticMesh->SetMobility(EComponentMobility::Stationary);
	StaticMesh->CastShadow = false;
	RootComponent = StaticMesh;

	//Network replication
	bReplicates = true;
}

void AHouseLight::SetupSphere()
{
	//Get bounding sphere of attenuation radius
	FSphere BoundingSphere = Light->GetBoundingSphere();

	//Set collision sphere to bounding sphere
	Sphere->SetWorldLocation(BoundingSphere.Center);
	Sphere->SetSphereRadius(BoundingSphere.W * BoundaryMultiplier);
	Sphere->SetVisibility(false);
}

void AHouseLight::ChangeLight()
{
	//Remove old light component
	if (Light != nullptr)
	{
		Light->DestroyComponent();
		Light = nullptr;
	}

	UStaticMesh* Mesh;

	switch (LightType)
	{
	case ELightType::Light_Ceiling_Large:
		{
			//Setup static mesh
			Mesh = LoadObject<UStaticMesh>(
				NULL, TEXT(
					"StaticMesh'/Game/Art/Meshes/Building/HouseLights/Combined/SM_House_Light_001.SM_House_Light_001'"));
			StaticMesh->SetStaticMesh(Mesh);
			StaticMesh->SetRelativeRotation(FRotator(-180, 0, 0), false);
			StaticMesh->SetWorldLocation(FVector(0, 0, -9) + GetActorLocation(), false);

			//Setup light
			Light = NewObject<UPointLightComponent>(this, UPointLightComponent::StaticClass());
			Light->RegisterComponent();
			Light->AttachToComponent(RootComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
			Light->SetMobility(EComponentMobility::Stationary);
			Light->SetRelativeLocation(FVector(0, 0, 10), false);

			UPointLightComponent* PointLight = Cast<UPointLightComponent>(Light);
			PointLight->Intensity = 2.0f;
			PointLight->AttenuationRadius = 700.0f;
			PointLight->bUseInverseSquaredFalloff = false;
			PointLight->LightFalloffExponent = 1.0;
		}
		break;

	case ELightType::Light_Ceiling_Large_Spotlight:
		{
			//Setup static mesh
			Mesh = LoadObject<UStaticMesh>(
                NULL, TEXT(
                    "StaticMesh'/Game/Art/Meshes/Building/HouseLights/Combined/SM_House_Light_001.SM_House_Light_001'"));
			StaticMesh->SetStaticMesh(Mesh);
			StaticMesh->SetRelativeRotation(FRotator(-180, 0, 0), false);
			StaticMesh->SetWorldLocation(FVector(0, 0, -9) + GetActorLocation(), false);

			//Setup Light
			Light = NewObject<USpotLightComponent>(this, USpotLightComponent::StaticClass());
			Light->RegisterComponent();
			Light->AttachToComponent(RootComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
			Light->SetMobility(EComponentMobility::Stationary);
			Light->SetRelativeRotation(FRotator(90, 0, 0), false);
			Light->SetRelativeLocation(FVector(0, 0, 10), false);

			USpotLightComponent* SpotLight = Cast<USpotLightComponent>(Light);
			SpotLight->Intensity = 3.0;
			SpotLight->AttenuationRadius = 700.0f;
			SpotLight->InnerConeAngle = 60.0f;
			SpotLight->OuterConeAngle = 90.0f;
			SpotLight->bUseInverseSquaredFalloff = false;
			SpotLight->LightFalloffExponent = 1.0;
		}
		break;
		
	case ELightType::Light_Ceiling:
		{
			//Setup static mesh
			Mesh = LoadObject<UStaticMesh>(
				NULL, TEXT(
					"StaticMesh'/Game/Art/Meshes/Building/HouseLights/Combined/SM_House_Light_002.SM_House_Light_002'"));
			StaticMesh->SetStaticMesh(Mesh);
			StaticMesh->SetRelativeRotation(FRotator(-180, 0, 0), false);
			StaticMesh->SetWorldLocation(FVector(0, 0, -1) + GetActorLocation(), false);

			//Setup light
			Light = NewObject<UPointLightComponent>(this, UPointLightComponent::StaticClass());
			Light->RegisterComponent();
			Light->AttachToComponent(RootComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
			Light->SetMobility(EComponentMobility::Stationary);

			UPointLightComponent* PointLight = Cast<UPointLightComponent>(Light);
			PointLight->Intensity = 3.0f;
			PointLight->AttenuationRadius = 500.0f;
			PointLight->bUseInverseSquaredFalloff = false;
			PointLight->LightFalloffExponent = 1.0;
		}
		break;

	case ELightType::Light_Wall:
		{
			//Setup static mesh
			Mesh = LoadObject<UStaticMesh>(
				NULL, TEXT(
					"StaticMesh'/Game/Art/Meshes/Building/HouseLights/Combined/SM_House_Light_003.SM_House_Light_003'"));
			StaticMesh->SetStaticMesh(Mesh);
			StaticMesh->SetRelativeRotation(FRotator(0, 180, 0), false);

			//Setup light
			Light = NewObject<URectLightComponent>(this, URectLightComponent::StaticClass());
			Light->RegisterComponent();
			Light->AttachToComponent(RootComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
			Light->SetRelativeRotation(FRotator(180, 0, 0), false);
			Light->SetRelativeLocation(FVector(-1, 0, 0), false);
			Light->SetMobility(EComponentMobility::Stationary);

			URectLightComponent* RectLight = Cast<URectLightComponent>(Light);
			RectLight->Intensity = 22500.0f;
			RectLight->AttenuationRadius = 750.0f;
			RectLight->SourceWidth = RectLight->SourceHeight = 100.0f;
			RectLight->BarnDoorAngle = RectLight->BarnDoorLength = 0.0f;
		}
		break;
	}

	SetupSphere();
}

// Called when the game starts or when spawned
void AHouseLight::BeginPlay()
{
	Super::BeginPlay();

	SetupSphere();
}

void AHouseLight::TurnOn_Implementation()
{
	Light->SetVisibility(true);
}

void AHouseLight::TurnOff_Implementation()
{
	Light->SetVisibility(false);
}
