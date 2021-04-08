// Please don't steal


#include "DialogueParser.h"
#include <string>
#include "Misc/FileHelper.h"

FDialogueInfo::FDialogueInfo()
{
}

FDialogueInfo DialogueParser::GetDialogueInfo(FString DialogueKey){
    //Declare a temp variable to store return values
    FDialogueInfo temp;
    //Declare an empty string to hold the incoming data
    FString DialogueScript;
    //Load the text from the designated path to the designated string
    FFileHelper::LoadFileToString(DialogueScript, *(FPaths::ProjectDir() + "Content/Dialogue/Dialogue.txt"));
    //Find the line for the dialogue information by using the DialogueKey
    int KeyStart = DialogueScript.Find(DialogueKey);
    int Start = KeyStart + (DialogueKey.Len() + 1);
    int End = 0;
    int EndCount = 0;
    //Iterate through each character until the desired character has been found
    for(int32 i = Start; i < DialogueScript.Len(); i++){
        FString CurrentChar = DialogueScript.Mid(i, 1);
        if(CurrentChar.Equals(TEXT("|"))){
            //Save the location of the desired character
            End = i;
            break;
        }
    }
    EndCount = End-Start;
    //Gather the dialogue message from the start and end point
    temp.Message = DialogueScript.Mid(Start, EndCount);

    //The next substring is going to start 2 digits after the previous one ended
    Start = End + 5;

    //Iterate through each character until the desired character has been found
    for(int32 i = Start; i < DialogueScript.Len(); i++){
        FString CurrentChar = DialogueScript.Mid(i, 1);
        if(CurrentChar.Equals(TEXT("}"))){
            //Save the location of the desired character
            End = i;
            break;
        }
    }
    EndCount = End-Start;
    //Gather the dialogue speaker from the start and end point
    temp.Speaker = DialogueScript.Mid(Start, EndCount);

    //Move forward one space from the end to check for the censorship symbol; if it's there, this line can be censored
    End = End + 1;
    temp.CensoredLine = (DialogueScript.Mid(End, 1).Equals(TEXT("#")));

    return temp;
}
