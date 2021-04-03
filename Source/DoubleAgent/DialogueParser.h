// Please don't steal

#pragma once

#include "CoreMinimal.h"

struct DialogueInfo{
	//This holds the display name of the person who is saying the dialogue line
FString Speaker;
	//This holds the content of the dialogue line that is being spoken
FString Message;
	//This holds the length of the dialogue line in seconds (Determined by the audio lines)
float MessageLength;
	//This determines whether or not the line can be said when censored lines are turned on
bool CensoredLine;
};
 
class DOUBLEAGENT_API DialogueParser{
public:
	DialogueParser();
	~DialogueParser();

	static DialogueInfo GetDialogueInfo(FString DialogueKey);
};
