// Please don't steal

#include "RadioHub.h"
#include "DoubleAgent/AI/NPC/StaffAIController.h"
#include "Kismet/GameplayStatics.h"

ARadioHub::ARadioHub()
{
    //Setup mesh
    UStaticMesh* Mesh = LoadObject<UStaticMesh>(NULL, TEXT("StaticMesh'/Game/Art/Meshes/Security/RadioHub/SM_RadioHub.SM_RadioHub'"));
    StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
    StaticMesh->SetStaticMesh(Mesh);
}

bool ARadioHub::StartRadioEvent(FRadioEvent RadioEvent_)
{
    //If power is off
    if (!bPowerOn)
        return false;
    
    //Save radio event
    RadioEvent = RadioEvent_;

    //Set broadcast after voice line
    FTimerDelegate TimerDelegate;
    GetWorld()->GetTimerManager().SetTimer(SpeechTimer, this, &ARadioHub::BroadcastRadioEvent, RadioEvent_.Delay, false);

    return true;
}

void ARadioHub::BroadcastRadioEvent()
{
    //Get all staff with a radio
    TArray<AActor*> Staff;
    UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("Radio"), Staff);
    
    //Remove staff that called the event
    Staff.RemoveAt(Staff.Find(RadioEvent.NPC));

    for (int i = 0; i < Staff.Num(); i++)
    {
        AStaffAIController* StaffNPC = Cast<AStaffAIController>(Cast<AAICharacterBase_CHARACTER>(Staff[i])->GetController());
        //If staff is not unconscious
        if (StaffNPC->BrainComponent->IsRunning())
            StaffNPC->HandleRadioEvent(RadioEvent);
    }
}

void ARadioHub::PowerOn(){
    bPowerOn = true;
}

void ARadioHub::PowerOff(){
    bPowerOn = false;
}