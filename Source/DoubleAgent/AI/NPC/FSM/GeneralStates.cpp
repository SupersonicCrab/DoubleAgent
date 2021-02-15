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
    //If no animation is playing and detection is above 90
    return (!Cast<ACharacter>(Controller->GetPawn())->GetMesh()->GetAnimInstance()->IsAnyMontagePlaying() && Blackboard->GetValueAsFloat("Detection") > 90) &&
        //If LoudNoiseLocation is not set or UnconsciousNPC is set
        (!Blackboard->IsVectorValueSet("LoudNoiseLocation") || Blackboard->GetValueAsObject("UnconsciousNPC") != nullptr);
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
    //If a loud noise has been heard or at engaging or lightswitch is set or UnconsciousNPC is set
    return (Controller->GetBlackboardComponent()->GetValueAsFloat("Detection") != 100 &&
        (Controller->GetBlackboardComponent()->IsVectorValueSet("LoudNoiseLocation") ||
            Controller->GetBlackboardComponent()->GetValueAsEnum("VocalStatus") == static_cast<uint8>(EVocalStatus::Vocal_Engaging) ||
            Controller->GetBlackboardComponent()->GetValueAsObject("LightSwitch") != nullptr) ||
            Controller->GetBlackboardComponent()->GetValueAsObject("UnconsciousNPC") != nullptr);
}

FSMState* CowerTransition::GetNewState()
{
    return new Cower();
}

Talk::Talk()
{
    EntryActions.Add(new TurnAtCrowd());
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

    for (int i = NPCs.Num()-1; i > 0; i--)
    {
        if (NPCs[i]->GetVelocity().Size() > 1)
            NPCs.RemoveAt(i);
    }
    
    return (NPCs.Num() >= 1);
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
    FDateTime Temp;
    Temp.Now();
    
    return (ArrivalTime - Temp > WaitTime*60);
}

FSMState* WanderTransition::GetNewState()
{
    return new Wander();
}
