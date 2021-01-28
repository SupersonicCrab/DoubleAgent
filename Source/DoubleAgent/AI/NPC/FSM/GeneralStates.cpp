// Please don't steal

#include "GeneralStates.h"
#include "GeneralActions.h"
#include "Animation/AnimInstance.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "DoubleAgent/AI/AICharacterBase_CHARACTER.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"

Despawn::Despawn()
{
    Actions.Add(new DeleteSelf());
    Transitions.Add(new CowerTransition());
}

bool DespawnCondition::TestCondition(AFSMController* Controller)
{
    UBlackboardComponent* Blackboard = Controller->GetBlackboardComponent();
    return Blackboard->GetValueAsFloat("Detection") > 90 && !Controller->GetBlackboardComponent()->IsVectorValueSet("LoudNoiseLocation") && !Cast<ACharacter>(Controller->GetPawn())->GetMesh()->GetAnimInstance()->IsAnyMontagePlaying();
}

FSMState* DespawnTransition::GetNewState()
{
    return new Despawn();
}

Cower::Cower()
{
    Actions.Add(new CowerAnimation());
    Transitions.Add(new DespawnTransition());
}

bool CowerCondition::TestCondition(AFSMController* Controller)
{
    //If a loud noise has been heard or at engaging or lightswitch is set
    return (Controller->GetBlackboardComponent()->GetValueAsFloat("Detection") != 100 &&
        (Controller->GetBlackboardComponent()->IsVectorValueSet("LoudNoiseLocation") ||
            Controller->GetBlackboardComponent()->GetValueAsEnum("VocalStatus") == static_cast<uint8>(EVocalStatus::Vocal_Engaging) ||
            Controller->GetBlackboardComponent()->GetValueAsObject("LightSwitch") != nullptr));
}

FSMState* CowerTransition::GetNewState()
{
    return new Cower();
}

Talk::Talk()
{
    Actions.Add(new SpeakToCrowd());
    Transitions.Add(new CowerTransition());
    Transitions.Add(new DespawnTransition());
    Transitions.Add(new WanderTransition());
}

bool TalkCondition::TestCondition(AFSMController* Controller)
{
    TArray<AActor*> NPCs;
    TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
    ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));
    TArray<AActor*> Ignore;
    Ignore.Add(Controller->GetPawn());
    UKismetSystemLibrary::SphereOverlapActors(Controller->GetWorld(), Controller->GetPawn()->GetActorLocation(), 200, ObjectTypes, AAICharacterBase_CHARACTER::StaticClass(), Ignore, NPCs);

    return (NPCs.Num() >= 2);
}

FSMState* TalkTransition::GetNewState()
{
    return new Talk();
}

Wander::Wander()
{
    Actions.Add(new GotoCrowd());
    Transitions.Add(new CowerTransition());
    Transitions.Add(new DespawnTransition());
    Transitions.Add(new TalkTransition());
}

bool WanderCondition::TestCondition(AFSMController* Controller)
{
    return false;
}

FSMState* WanderTransition::GetNewState()
{
    return new Wander();
}
