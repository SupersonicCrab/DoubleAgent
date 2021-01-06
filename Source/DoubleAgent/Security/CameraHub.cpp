// Please don't steal


#include "CameraHub.h"
#include "DoubleAgent/Player_Character.h"

#include "Kismet/GameplayStatics.h"

// Sets default values
ACameraHub::ACameraHub(){
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
	BoxCollision->SetRelativeLocation(FVector(0,150,80));
	BoxCollision->SetRelativeScale3D(FVector(7.0, 5.75, 4.25));
	BoxCollision->OnComponentBeginOverlap.AddDynamic(this, &ACameraHub::OnBoxBeginOverlap);
	BoxCollision->OnComponentEndOverlap.AddDynamic(this, &ACameraHub::OnComponentOverlapEnd);
}

// Called when the game starts or when spawned
void ACameraHub::BeginPlay(){
	Super::BeginPlay();
	TArray<AActor*> tempCameras;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACamera::StaticClass(), tempCameras); // Get all camera actors
	for(auto c : tempCameras){
		Cameras.Add(dynamic_cast<ACamera*>(c)); //Cast all camera actors into camera pointers and store them in an array
	}
	for(int i = 0; i < 6; i++){
		CameraAutoDefault.Add(Cameras[i]->bAutoRotate);
		Cameras[i]->CaptureComponent->TextureTarget = TextureTargets[i]; //Setting each camera to send their texture target to the associated target texture
		Cameras[i]->CaptureComponent->CaptureScene();
	}
}

// Called every frame
void ACameraHub::Tick(float DeltaTime){
	Super::Tick(DeltaTime);
	
}

void ACameraHub::OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult){
	if(OtherActor->GetClass() == APlayer_Character::StaticClass()){
		if(bPowerOn){
			EnableCapture();
		}
	}
}

void ACameraHub::OnComponentOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex){
	if(OtherActor->GetClass() == APlayer_Character::StaticClass()){
		DisableCapture();
	} else{
		//Chris AI stuff
		//Operator AI leaves CameraHub
	}
}

void ACameraHub::PowerEnabled(){
	for(auto c: Cameras){
		c->bPowerOn = true;
	}
	bPowerOn = true;
}

void ACameraHub::PowerDisabled(){
	for(auto c: Cameras){
		c->bPowerOn = false;
	}
	bPowerOn = false;
}

void ACameraHub::EnableCapture(){
	for(auto c: Cameras){
		c->SetCaptureEnabled(true);
	}
}

void ACameraHub::DisableCapture(){
	for(auto c: Cameras){
		c->SetCaptureEnabled(false);
	}
}
