// Please don't steal

#include "BTTask_Speak.h"
#include "Perception/AISense_Hearing.h"
#include "BehaviorTree/BlackboardComponent.h"

EBTNodeResult::Type UBTTask_Speak::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	//Save NPC and blackboard
	APawn* NPC = Cast<AAIController>(OwnerComp.GetOwner())->GetPawn();
	UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
	
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

	//If overwriting
	if (bOverwriteVocalStatus)
	{
		Cast<AAIControllerBase>(OwnerComp.GetOwner())->GetBlackboardComponent()->SetValueAsEnum("VocalStatus", static_cast<uint8>(NewVocalStatus));
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
			return ESpeechEvent::SpeechEvent_Action_Cleaning;
		case (EActionStatus::Action_Washroom):
			return ESpeechEvent::SpeechEvent_Action_BathroomBreak;
		case (EActionStatus::Action_IdleTransition):
			return ESpeechEvent::SpeechEvent_Action_IdleTransition;
		case (EActionStatus::Action_BodySearch):
			return ESpeechEvent::SpeechEvent_Action_BodySearch;
		case (EActionStatus::Action_NoiseInvestigation):
			return ESpeechEvent::SpeechEvent_Action_NoiseInvestigation;
		case (EActionStatus::Action_PowerBox):
			return ESpeechEvent::SpeechEvent_Action_PowerBox;
		case (EActionStatus::Action_CameraHub):
			return ESpeechEvent::SpeechEvent_Action_CameraHub;
		case (EActionStatus::Action_Restock):
			return ESpeechEvent::SpeechEvent_Action_Restocking;
		case (EActionStatus::Action_Backup):
			return ESpeechEvent::SpeechEvent_Action_Backup;
		case (EActionStatus::Action_Reloading):
			return ESpeechEvent::SpeechEvent_Action_Reloading;
		case (EActionStatus::Action_Revive):
			return ESpeechEvent::SpeechEvent_Action_Revive;
		default:
			break;
		}
	}
		switch (NewVocalStatus)
		{
		case (EVocalStatus::Vocal_Idle):
			return ESpeechEvent::SpeechEvent_Idle;
		case (EVocalStatus::Vocal_Patrol):
			return ESpeechEvent::SpeechEvent_Patrol;
		case (EVocalStatus::Vocal_Cautious):
			return ESpeechEvent::SpeechEvent_Cautious;
		case (EVocalStatus::Vocal_Alert):
			return ESpeechEvent::SpeechEvent_Alert;
		case (EVocalStatus::Vocal_Subduing):
			return ESpeechEvent::SpeechEvent_Subduing;
		case (EVocalStatus::Vocal_Searching):
			return ESpeechEvent::SpeechEvent_Searching;
		case (EVocalStatus::Vocal_Engaging):
			return ESpeechEvent::SpeechEvent_Engaging;
		default:
			break;
		}
	return ESpeechEvent::SpeechEvent_Idle;
}
