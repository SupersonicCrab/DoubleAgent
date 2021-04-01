// Please don't steal

#include "BTTask_Speak.h"
#include "Perception/AISense_Hearing.h"
#include "BehaviorTree/BlackboardComponent.h"

EBTNodeResult::Type UBTTask_Speak::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	//Save NPC and blackboard
	APawn* NPC = Cast<AAIController>(OwnerComp.GetOwner())->GetPawn();
	UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();

	//If overwriting
	if (bOverwriteVocalStatus)
	{
		Blackboard->SetValueAsEnum("VocalStatus", static_cast<uint8>(NewVocalStatus));
		Cast<AAICharacterBase_CHARACTER>(NPC)->NetRequestSpeak(GetSpeechEvent());
		return EBTNodeResult::Succeeded;
	}
	
	//If new action status is not idle
	if (NewActionStatus != EActionStatus::Action_Idle)
	{
		//Raise vocal status
		Cast<AStaffAIController>(OwnerComp.GetOwner())->RaiseVocalStatus(NewVocalStatus);
		
		//Set action status
		Blackboard->SetValueAsEnum("ActionStatus", static_cast<uint8>(NewActionStatus));

		//Speak
		Cast<AAICharacterBase_CHARACTER>(NPC)->NetRequestSpeak(GetSpeechEvent());

		return EBTNodeResult::Succeeded;
	}
	
	//If vocal status was raised or restating
	if (Cast<AAIControllerBase>(OwnerComp.GetOwner())->RaiseVocalStatus(NewVocalStatus) || bRestateVocalStatus)
		Cast<AAICharacterBase_CHARACTER>(NPC)->NetRequestSpeak(GetSpeechEvent());
	return EBTNodeResult::Succeeded;
}

ESpeechEvent UBTTask_Speak::GetSpeechEvent()
{
	if (NewActionStatus > EActionStatus::Action_Idle)
	{
		switch (NewActionStatus)
		{
		case (EActionStatus::Action_Cleaning):
			return ESpeechEvent::Cleaning;
		case (EActionStatus::Action_Washroom):
			return ESpeechEvent::BathroomBreak;
		case (EActionStatus::Action_IdleTransition):
			return ESpeechEvent::IdleTransition;
		case (EActionStatus::Action_BodySearch):
			return ESpeechEvent::BodySearch;
		case (EActionStatus::Action_NoiseInvestigation):
			return ESpeechEvent::NoiseInvestigation;
		case (EActionStatus::Action_PowerBox):
			return ESpeechEvent::Powerbox;
		case (EActionStatus::Action_CameraHub):
			return ESpeechEvent::CameraHub;
		case (EActionStatus::Action_Restock):
			return ESpeechEvent::Restocking;
		case (EActionStatus::Action_Backup):
			return ESpeechEvent::Backup;
		case (EActionStatus::Action_Reloading):
			return ESpeechEvent::Reload;
		case (EActionStatus::Action_Revive):
			return ESpeechEvent::Revive;
		default:
			break;
		}
	}
		switch (NewVocalStatus)
		{
		case (EVocalStatus::Vocal_Idle):
			return ESpeechEvent::Idle;
		case (EVocalStatus::Vocal_Patrol):
			return ESpeechEvent::Patrol;
		case (EVocalStatus::Vocal_Cautious):
			return ESpeechEvent::Cautious;
		case (EVocalStatus::Vocal_Alert):
			return ESpeechEvent::Alert;
		case (EVocalStatus::Vocal_Subduing):
			return ESpeechEvent::Subduing;
		case (EVocalStatus::Vocal_Searching):
			return ESpeechEvent::Searching;
		case (EVocalStatus::Vocal_Engaging):
			return ESpeechEvent::Engaging;
		default:
			break;
		}
	return ESpeechEvent::Idle;
}
