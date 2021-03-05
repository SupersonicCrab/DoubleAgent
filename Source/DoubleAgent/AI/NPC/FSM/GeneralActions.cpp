// Please don't steal

#include "GeneralActions.h"
#include "Animation/AnimInstance.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "DoubleAgent/BaseCharacter_CHARACTER.h"
#include "DoubleAgent/AI/AICharacterBase_CHARACTER.h"
#include "EnvironmentQuery/EnvQuery.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Misc/OutputDeviceNull.h"


bool DeleteSelf::PerformAction(AFSMController* Controller)
{
    if (Controller->LastMoveResult.IsFailure())
        return false;
    
    //If NPC is not moving or animating
    if (Controller->GetMoveStatus() == EPathFollowingStatus::Idle && !Cast<ACharacter>(Controller->GetPawn())->GetMesh()->GetAnimInstance()->IsAnyMontagePlaying())
    {
        Controller->GetPawn()->Destroy();
        Controller->Destroy();
        return true;
    }
       
    return false;
}

bool CowerAnimation::PerformAction(AFSMController* Controller)
{
    //Set detection to 100
    Controller->GetBlackboardComponent()->SetValueAsFloat("Detection", 100);
    
    //Stop all movement if any
    Controller->StopMovement();
    
    //Update blackboard
    UBlackboardComponent* Blackboard = Controller->GetBlackboardComponent();
    Controller->RaiseDetection(100.0f);
    Blackboard->ClearValue("LoudNoiseLocation");

    FOutputDeviceNull OutputDeviceNull;
    const TCHAR* CmdAndParams = TEXT("NetCower True");
    Controller->GetPawn()->CallFunctionByNameWithArguments(CmdAndParams, OutputDeviceNull, nullptr, true);
    
    return true;
}

bool SpeakToCrowd::PerformAction(AFSMController* Controller)
{
    ABaseCharacter_CHARACTER* Speaker = Cast<ABaseCharacter_CHARACTER>(Controller->GetBlackboardComponent()->GetValueAsObject("Speaker"));
    
    //If someone has already spoken nearby recently
    if (IsValid(Speaker))
    {
        //If speaker is within speaking distance and is not self
        if (Controller->GetPawn()->GetDistanceTo(Speaker) <= SpeakingDistance && Speaker != Controller->GetPawn())
        {
            //Look at speaker
            Controller->SetControlRotation(UKismetMathLibrary::FindLookAtRotation(Controller->GetPawn()->GetActorLocation(), Speaker->GetActorLocation()));
        }
        return true;
    }
    
    Cast<ABaseCharacter_CHARACTER>(Controller->GetPawn())->NetRequestSpeak(ESpeechEvent::SpeechEvent_Idle);
    return true;
}

bool GotoCrowd::PerformAction(AFSMController* Controller)
{
    //Skip if already moving
    if (Controller->GetMoveStatus() != EPathFollowingStatus::Idle)
        return true;
    
    //Change characters speed to walking
    Cast<ACharacter>(Controller->GetPawn())->GetCharacterMovement()->MaxWalkSpeed = 130;

    //Load eqs query to be run
    UEnvQuery* Query = LoadObject<UEnvQuery>(NULL, TEXT("EnvQuery'/Game/AI/EQS/Queries/FindCrowd_EQSQUERY.FindCrowd_EQSQUERY'"));

    //Run eqs behaviour
    Controller->FindEQS(Query);
    return true;
}

bool GoToDespawn::PerformAction(AFSMController* Controller)
{
    //Change characters speed to running
    Cast<ACharacter>(Controller->GetPawn())->GetCharacterMovement()->MaxWalkSpeed = 630;

    //Load eqs query to be run
    UEnvQuery* Query = LoadObject<UEnvQuery>(NULL, TEXT("EnvQuery'/Game/AI/EQS/Queries/FindCivlianDespawn_EQSQUERY.FindCivlianDespawn_EQSQUERY'"));

    //Run eqs behaviour
    Controller->FindEQS(Query);
    return true;
}

bool StopMovement::PerformAction(AFSMController* Controller)
{
    //If already stopped
    if (Controller->GetMoveStatus() == EPathFollowingStatus::Idle)
        return true;

    Controller->StopMovement();
    return true;
}

bool TurnAtCrowd::PerformAction(AFSMController* Controller)
{
    //Get all nearby characters
    TArray<AActor*> NPCs;
    TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
    ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));
    TArray<AActor*> Ignore;
    Ignore.Add(Controller->GetPawn());
    UKismetSystemLibrary::SphereOverlapActors(Controller->GetWorld(), Controller->GetPawn()->GetActorLocation(), 500, ObjectTypes, AAICharacterBase_CHARACTER::StaticClass(), Ignore, NPCs);
    
    //Remove moving characters
    for (int i = NPCs.Num()-1; i > 0; i--)
    {
        if (NPCs[i]->GetVelocity().Size() > 1)
            NPCs.RemoveAt(i);
    }

    //Stop if no nearby NPCs was found
    if (NPCs.Num() == 0)
        return true;

    //Average locations of nearby NPCs
    FVector Temp;
    for (int i = 0; i < NPCs.Num(); i++)
        Temp += NPCs[i]->GetActorLocation();
    Temp = Temp/NPCs.Num();

    //Look at averaged location
    Controller->SetControlRotation(UKismetMathLibrary::FindLookAtRotation(Controller->GetPawn()->GetActorLocation(), Temp));
    return true;
}
