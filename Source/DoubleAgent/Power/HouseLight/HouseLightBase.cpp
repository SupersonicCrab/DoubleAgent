// Please don't steal


#include "HouseLightBase.h"

// Sets default values
AHouseLightBase::AHouseLightBase(){
	//This is constructing and attaching the collision component to the light component
	Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	Sphere->SetVisibility(false);
	RootComponent = Sphere;
	Sphere->SetMobility(EComponentMobility::Stationary);

	//Setup static mesh
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	StaticMesh->CastShadow = false;
	
	//Network replication
	bReplicates = true;
}

// Called when the game starts or when spawned
void AHouseLightBase::BeginPlay(){
	Super::BeginPlay();

	//Get bounding sphere of attenuation radius
	FSphere BoundingSphere = Light->GetBoundingSphere();

	//Set collision sphere to bounding sphere
	Sphere->SetWorldLocation(BoundingSphere.Center);
	Sphere->SetSphereRadius(BoundingSphere.W*BoundaryMultiplier);
}

void AHouseLightBase::TurnOn_Implementation(){
	Light->SetVisibility(true);
}

void AHouseLightBase::TurnOff_Implementation()
{
	Light->SetVisibility(false);
}