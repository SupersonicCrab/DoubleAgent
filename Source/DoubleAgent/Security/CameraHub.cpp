// Please don't steal


#include "CameraHub.h"
#include "DoubleAgent/Player_Character.h"
#include "DoubleAgent/AI/NPC/StaffAIController.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ACameraHub::ACameraHub(){
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	//Create the root
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	this->SetRootComponent(Root);
	OperatorPosition = CreateDefaultSubobject<USceneComponent>(TEXT("OperatorPosition"));
	OperatorPosition->SetupAttachment(Root);
	OperatorPosition->SetRelativeLocation(FVector(0, 200, 0), false);
	//Create the box collision and attach it to the root
	BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
	BoxCollision->SetupAttachment(Root);
	//Resize the collision box
	BoxCollision->SetRelativeLocation(FVector(0,150,100));
	BoxCollision->SetRelativeScale3D(FVector(5, 8, 3.5));
	//Enable the collision on the object
	BoxCollision->SetGenerateOverlapEvents(true);
	SetActorEnableCollision(true);
	BoxCollision->OnComponentBeginOverlap.AddDynamic(this, &ACameraHub::OnBoxBeginOverlap);
	BoxCollision->OnComponentEndOverlap.AddDynamic(this, &ACameraHub::OnComponentOverlapEnd);

	//Set up table and stand static mesh and attach to the root component
	TableMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TableAndStand"));
	TableMesh->SetupAttachment(Root);

	UStaticMesh* Mesh;
	Mesh = LoadObject<UStaticMesh>(NULL, TEXT("StaticMesh'/Game/Art/Meshes/Security/CameraHub/SM_CameraHub.SM_CameraHub'"));
	TableMesh->SetStaticMesh(Mesh);
	
	//Set up all the screen static meshes and attach to the table component
	//Screen 001
	ScreenMeshes.Add(CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Screen1")));
	ScreenMeshes[0]->SetupAttachment(TableMesh);
	Mesh = LoadObject<UStaticMesh>(NULL, TEXT("StaticMesh'/Game/Art/Meshes/Security/CameraHub/SM_CameraHub_001.SM_CameraHub_001'"));
	ScreenMeshes[0]->SetStaticMesh(Mesh);
	//Screen 002
	ScreenMeshes.Add(CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Screen2")));
	ScreenMeshes[1]->SetupAttachment(TableMesh);
	Mesh = LoadObject<UStaticMesh>(NULL, TEXT("StaticMesh'/Game/Art/Meshes/Security/CameraHub/SM_CameraHub_002.SM_CameraHub_002'"));
	ScreenMeshes[1]->SetStaticMesh(Mesh);
	//Screen 003
	ScreenMeshes.Add(CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Screen3")));
	ScreenMeshes[2]->SetupAttachment(TableMesh);
	Mesh = LoadObject<UStaticMesh>(NULL, TEXT("StaticMesh'/Game/Art/Meshes/Security/CameraHub/SM_CameraHub_003.SM_CameraHub_003'"));
	ScreenMeshes[2]->SetStaticMesh(Mesh);
	//Screen 004
	ScreenMeshes.Add(CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Screen4")));
	ScreenMeshes[3]->SetupAttachment(TableMesh);
	Mesh = LoadObject<UStaticMesh>(NULL, TEXT("StaticMesh'/Game/Art/Meshes/Security/CameraHub/SM_CameraHub_004.SM_CameraHub_004'"));
	ScreenMeshes[3]->SetStaticMesh(Mesh);
	//Screen 005
	ScreenMeshes.Add(CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Screen5")));
	ScreenMeshes[4]->SetupAttachment(TableMesh);
	Mesh = LoadObject<UStaticMesh>(NULL, TEXT("StaticMesh'/Game/Art/Meshes/Security/CameraHub/SM_CameraHub_005.SM_CameraHub_005'"));
	ScreenMeshes[4]->SetStaticMesh(Mesh);
	//Screen 006
	ScreenMeshes.Add(CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Screen6")));
	ScreenMeshes[5]->SetupAttachment(TableMesh);
	Mesh = LoadObject<UStaticMesh>(NULL, TEXT("StaticMesh'/Game/Art/Meshes/Security/CameraHub/SM_CameraHub_006.SM_CameraHub_006'"));
	ScreenMeshes[5]->SetStaticMesh(Mesh);
	//Increase scale so that the table is a more appropriate size
	//Rotate the CameraHub mesh so that it'll match with the collision box
	TableMesh->SetRelativeScale3D(FVector(1.3f,1.3f,1.3f));
	TableMesh->SetRelativeRotation(FRotator(0.0f, 90.0f,0.0f));

	//Load texture targets
	UTextureRenderTarget2D* TextureRenderTarget2D = LoadObject<UTextureRenderTarget2D>(NULL, TEXT("TextureRenderTarget2D'/Game/SecuritySystem/Cam_Input/RT_CameraFeed.RT_CameraFeed'"));
	TextureTargets.Add(TextureRenderTarget2D);
	TextureRenderTarget2D = LoadObject<UTextureRenderTarget2D>(NULL, TEXT("TextureRenderTarget2D'/Game/SecuritySystem/Cam_Input/RT_CameraFeed2.RT_CameraFeed2'"));
	TextureTargets.Add(TextureRenderTarget2D);
	TextureRenderTarget2D = LoadObject<UTextureRenderTarget2D>(NULL, TEXT("TextureRenderTarget2D'/Game/SecuritySystem/Cam_Input/RT_CameraFeed3.RT_CameraFeed3'"));
	TextureTargets.Add(TextureRenderTarget2D);
	TextureRenderTarget2D = LoadObject<UTextureRenderTarget2D>(NULL, TEXT("TextureRenderTarget2D'/Game/SecuritySystem/Cam_Input/RT_CameraFeed4.RT_CameraFeed4'"));
	TextureTargets.Add(TextureRenderTarget2D);
	TextureRenderTarget2D = LoadObject<UTextureRenderTarget2D>(NULL, TEXT("TextureRenderTarget2D'/Game/SecuritySystem/Cam_Input/RT_CameraFeed5.RT_CameraFeed5'"));
	TextureTargets.Add(TextureRenderTarget2D);
	TextureRenderTarget2D = LoadObject<UTextureRenderTarget2D>(NULL, TEXT("TextureRenderTarget2D'/Game/SecuritySystem/Cam_Input/RT_CameraFeed6.RT_CameraFeed6'"));
	TextureTargets.Add(TextureRenderTarget2D);

	PerceptionStimuliSource = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(TEXT("Perception stimuliSource"));
	
	bPowerOn = true;
}

// Called when the game starts or when spawned
void ACameraHub::BeginPlay(){
	Super::BeginPlay();
	//Setup collision functions
	TArray<AActor*> tempCameras;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACamera::StaticClass(), tempCameras); // Get all camera actors
	for(auto c : tempCameras){
		Cameras.Add(dynamic_cast<ACamera*>(c)); //Cast all camera actors into camera pointers and store them in an array
	}
	for(int i = 0; i < Cameras.Num(); i++){
		CameraAutoDefault.Add(Cameras[i]->bAutoRotate);
		Cameras[i]->CaptureComponent->TextureTarget = TextureTargets[i]; //Setting each camera to send their texture target to the associated target texture
		Cameras[i]->CaptureComponent->CaptureSceneDeferred();
		Cameras[i]->CameraHub = this;
	}

	PerceptionStimuliSource->RegisterForSense(UAISense_Sight::StaticClass());
}

bool ACameraHub::CanBeSeenFrom(const FVector& ObserverLocation, FVector& OutSeenLocation, int32& NumberOfLoSChecksPerformed, float& OutSightStrength, const AActor* IgnoreActor) const
{
	//Setup
	FHitResult HitResult;
	NumberOfLoSChecksPerformed++;
	
	//If door is visible
	if (!GetWorld()->LineTraceSingleByChannel(HitResult, ObserverLocation, ScreenMeshes[2]->GetComponentLocation(), ECollisionChannel(ECC_Visibility), FCollisionQueryParams(FName(TEXT("CenterLOS")), true, IgnoreActor)) || HitResult.Actor->IsOwnedBy(this))
	{
		OutSeenLocation = ScreenMeshes[2]->GetComponentLocation();
		OutSightStrength = 1;
		return true;
	}

	//Return false if nothing was hit
	OutSightStrength = 0;
	return false;
}

void ACameraHub::OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                   UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult){
	if(OtherActor->IsA(APlayer_Character::StaticClass())){
		if(bPowerOn){
			EnableCapture();
		}
	}
}

void ACameraHub::OnComponentOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex){
	if(OtherActor->IsA(APlayer_Character::StaticClass())){
		DisableCapture();
	} else{
		//If camera operator just left
		if (OperatorNPC != nullptr)
		{
			OperatorNPC->GetBlackboardComponent()->SetValueAsBool("CamerasActive", false);
			OperatorNPC->GetBlackboardComponent()->SetValueAsBool("UsingCameras", false);
			OperatorNPC = nullptr;
		}
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

void ACameraHub::EnableDisplay()
{
	//If display is already on
	if (bDisplayOn)
		return;
	
	bDisplayOn = true;
	
	UMaterial* Material = LoadObject<UMaterial>(NULL, TEXT("Material'/Game/SecuritySystem/Cam_Output/M_ViewScreen.M_ViewScreen'"));
	ScreenMeshes[0]->SetMaterial(0, Material);
	Material = LoadObject<UMaterial>(NULL, TEXT("Material'/Game/SecuritySystem/Cam_Output/M_ViewScreen2.M_ViewScreen2'"));
	ScreenMeshes[1]->SetMaterial(0, Material);
	Material = LoadObject<UMaterial>(NULL, TEXT("Material'/Game/SecuritySystem/Cam_Output/M_ViewScreen3.M_ViewScreen3'"));
	ScreenMeshes[2]->SetMaterial(0, Material);
	Material = LoadObject<UMaterial>(NULL, TEXT("Material'/Game/SecuritySystem/Cam_Output/M_ViewScreen4.M_ViewScreen4'"));
	ScreenMeshes[3]->SetMaterial(0, Material);
	Material = LoadObject<UMaterial>(NULL, TEXT("Material'/Game/SecuritySystem/Cam_Output/M_ViewScreen5.M_ViewScreen5'"));
	ScreenMeshes[4]->SetMaterial(0, Material);
	Material = LoadObject<UMaterial>(NULL, TEXT("Material'/Game/SecuritySystem/Cam_Output/M_ViewScreen6.M_ViewScreen6'"));
	ScreenMeshes[5]->SetMaterial(0, Material);
}

void ACameraHub::DisableDisplay()
{
	//If display is already off
	if (!bDisplayOn)
		return;
	
	bDisplayOn = false;
	
	UMaterial* Material = LoadObject<UMaterial>(NULL, TEXT("Material'/Engine/EngineDebugMaterials/BlackUnlitMaterial.BlackUnlitMaterial'"));
	for (int i = 0; i < ScreenMeshes.Num(); i++)
	{
		ScreenMeshes[i]->SetMaterial(0, Material);
	}
}

void ACameraHub::AutoRotateDisableReaction()
{
	if (OperatorNPC != nullptr)
	{
		for (int i = 0; i < Cameras.Num(); i++)
		{
			//If cameras was previously rotating
			if (CameraAutoDefault[i] != Cameras[i]->bAutoRotate)
			{
				Cameras[i]->bAutoRotate = true;
				Cameras[i]->TargetYaw = Cameras[i]->RightYawLimit;
			}
		}
	}
}
