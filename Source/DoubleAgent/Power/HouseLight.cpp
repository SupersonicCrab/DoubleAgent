// Please don't steal

#include "HouseLight.h"
#include "Components/PointLightComponent.h"
#include "Components/RectLightComponent.h"
#include "Components/SpotLightComponent.h"
#include "Engine/EngineTypes.h"

// Sets default values
AHouseLight::AHouseLight()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));;
	RootComponent->SetMobility(EComponentMobility::Stationary);
	
	//Setup sphere
	Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	Sphere->SetupAttachment(RootComponent);
	Sphere->SetVisibility(false);

	//Setup static mesh
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	StaticMesh->SetMobility(EComponentMobility::Stationary);
	StaticMesh->CastShadow = false;
	StaticMesh->SetupAttachment(RootComponent);

	//Network replication
	bReplicates = true;
}

void AHouseLight::UpdateSphere()
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

void AHouseLight::UpdateLight()
{
	//Update mesh
	UStaticMesh* Mesh;
	switch (MeshType)
	{
		case ELightMesh::Mesh_Ceiling:
			{
				Mesh = LoadObject<UStaticMesh>(NULL, TEXT("StaticMesh'/Game/Art/Meshes/Building/HouseLights/Combined/SM_House_Light_002.SM_House_Light_002'"));
				StaticMesh->SetStaticMesh(Mesh);
				StaticMesh->SetRelativeRotation(FRotator(-180, 0, 0), false);
				StaticMesh->SetRelativeLocation(FVector(0, 0, -1), false);
			}
		break;
		case ELightMesh::Mesh_Ceiling_Large:
			{
				Mesh = LoadObject<UStaticMesh>(NULL, TEXT("StaticMesh'/Game/Art/Meshes/Building/HouseLights/Combined/SM_House_Light_001.SM_House_Light_001'"));
				StaticMesh->SetStaticMesh(Mesh);
				StaticMesh->SetRelativeRotation(FRotator(-180, 0, 0), false);
				StaticMesh->SetRelativeLocation(FVector(0, 0, -9), false);
			}
		break;
		case ELightMesh::Mesh_Wall:
			{
				Mesh = LoadObject<UStaticMesh>(NULL, TEXT("StaticMesh'/Game/Art/Meshes/Building/HouseLights/Combined/SM_House_Light_003.SM_House_Light_003'"));
				StaticMesh->SetStaticMesh(Mesh);
				StaticMesh->SetRelativeRotation(FRotator(0, 180, 0), false);	
			}
		break;
	}

	//Cleanup old light if any
	if (Light != nullptr)
	{
		Light->DestroyComponent();
		Light = nullptr;
	}
	
	//Update light
	switch (LightType)
	{
		case ELightType::Light_Point:
			{
				Light = NewObject<UPointLightComponent>(this, UPointLightComponent::StaticClass());
				Light->RegisterComponent();
				Light->AttachToComponent(RootComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
				UPointLightComponent* PointLight = Cast<UPointLightComponent>(Light);
				PointLight->Intensity = 2.0f;
				PointLight->AttenuationRadius = 700.0f;
				PointLight->bUseInverseSquaredFalloff = false;
				PointLight->LightFalloffExponent = 1.0;
				PointLight->LightmassSettings.ShadowExponent = 1.0f;
				PointLight->IndirectLightingIntensity = 6.0f;
				PointLight->VolumetricScatteringIntensity = 4.0f;
				Light->SetMobility(EComponentMobility::Stationary);
			}
		break;
		case ELightType::Light_Rect:
			{
				Light = NewObject<URectLightComponent>(this, URectLightComponent::StaticClass());
				Light->RegisterComponent();
				Light->AttachToComponent(RootComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
				if (MeshType != ELightMesh::Mesh_Wall)
				{
					Light->SetRelativeRotation(FRotator(-90, 0, 0), false);
					Light->SetRelativeLocation(FVector(0, 0, -1), false);
				}
				URectLightComponent* RectLight = Cast<URectLightComponent>(Light);
				RectLight->Intensity = 22500.0f;
				RectLight->AttenuationRadius = 750.0f;
				RectLight->SourceWidth = RectLight->SourceHeight = 100.0f;
				RectLight->BarnDoorAngle = RectLight->BarnDoorLength = 0.0f;
				RectLight->LightmassSettings.ShadowExponent = 1.0f;
				RectLight->IndirectLightingIntensity = 6.0f;
				RectLight->VolumetricScatteringIntensity = 4.0f;
				Light->SetMobility(EComponentMobility::Stationary);
			}
		break;
		case ELightType::Light_Spot:
			{
				Light = NewObject<USpotLightComponent>(this, USpotLightComponent::StaticClass());
				Light->RegisterComponent();
				Light->AttachToComponent(RootComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
				if (MeshType != ELightMesh::Mesh_Wall)
					Light->SetRelativeRotation(FRotator(-90, 0, 0), false);
				USpotLightComponent* SpotLight = Cast<USpotLightComponent>(Light);
				SpotLight->Intensity = 3.0;
				SpotLight->AttenuationRadius = 700.0f;
				SpotLight->InnerConeAngle = 60.0f;
				SpotLight->OuterConeAngle = 90.0f;
				SpotLight->bUseInverseSquaredFalloff = false;
				SpotLight->LightFalloffExponent = 1.0;
				SpotLight->LightmassSettings.ShadowExponent = 1.0f;
				SpotLight->IndirectLightingIntensity = 6.0f;
				SpotLight->VolumetricScatteringIntensity = 4.0f;
				Light->SetMobility(EComponentMobility::Stationary);
			}
		break;
	}

	UpdateSphere();
}

// Called when the game starts or when spawned
void AHouseLight::BeginPlay()
{
	Super::BeginPlay();

	UpdateSphere();
}

void AHouseLight::TurnOn_Implementation()
{
	Light->SetVisibility(true);
}

void AHouseLight::TurnOff_Implementation()
{
	Light->SetVisibility(false);
}
