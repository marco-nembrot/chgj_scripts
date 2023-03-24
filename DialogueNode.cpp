// Fill out your copyright notice in the Description page of Project Settings.


#include "DialogueNode.h"

DialogueNode::DialogueNode(TSharedPtr<FJsonObject> JsonData) : DialogueNode()
{
	Name = JsonData->GetStringField("node_name");
	Type = JsonData->GetStringField("node_type");
	RawData = JsonData;
}

DialogueNode::DialogueNode()
{
	// TODO: find a way to get this value elsewhere (GameState / whatever)
	Language = "ENG";
}

DialogueNode::~DialogueNode()
{
}

//
void DialogueNode::Status()
{
	GLog->Log("LogCustom: Status of " + Name);
}

//
FString DialogueNode::Next()
{
	return Next(RawData);
}
//
FString DialogueNode::Next(TSharedPtr<FJsonObject> NodeData)
{
	FString NextValue = "";
	NodeData->TryGetStringField("next", NextValue);

	return NextValue;
}

//
FString DialogueNode::GetCharacterName()
{
	FString NameCharacter = "";

	TArray<TSharedPtr<FJsonValue>> Choices = RawData->GetArrayField("character");
	if (Choices.Num() > 0)
	{
		NameCharacter = Choices[0]->AsString();
	}

	return NameCharacter;
}

//
FString DialogueNode::GetTextContent()
{
	return GetTextContent(RawData);
}
//
FString DialogueNode::GetTextContent(TSharedPtr<FJsonObject> NodeData)
{
	//
	FString TextValue = "";
	NodeData->TryGetStringField("text", TextValue);
	if (TextValue == "")
	{
		TSharedPtr<FJsonObject> TextObject = NodeData->GetObjectField("text");
		TextObject->TryGetStringField(Language, TextValue);
	}
	//GLog->Log("LogCustom: GetTextContent text=" + TextValue);

	return TextValue;
}

//
FString DialogueNode::GetConditionBranch(FString ConditionValue)
{
	//
	TSharedPtr<FJsonObject> ConditionData = RawData->GetObjectField("branches");
	FString NextValue = "";
	ConditionData->TryGetStringField(ConditionValue, NextValue);
	GLog->Log("LogCustom: GetConditionBranch " + NextValue);

	return NextValue;
}

//
void DialogueNode::BuildChoices()
{

	if (RawData->HasField("choices"))
	{
		//GLog->Log("LogCustom: getting dialogue nodes...");
		//
		DialoguesChoices.Reset();
		//
		TArray<TSharedPtr<FJsonValue>> Choices = RawData->GetArrayField("choices");
		for (int32 index = 0; index < Choices.Num(); index++)
		{
			TSharedPtr<FJsonObject> ChoiceObject = Choices[index]->AsObject();
			// Adding to datatable
			DialoguesChoices.Add(GetTextContent(ChoiceObject), Next(ChoiceObject));
			//GLog->Log("LogCustom: " + GetTextContent(ChoiceObject));
		}
	}
	else
	{
		GLog->Log("LogCustom: no BuildChoices");
	}
}

//
FString DialogueNode::MakeTheChoice(FString DialogueText)
{
	GLog->Log("LogCustom: DialogueNode MakeTheChoice");
	BuildChoices();

	return DialoguesChoices.FindRef(DialogueText);
}

//
FString DialogueNode::GetVariableName()
{
	//
	FString VariableName = "";
	RawData->TryGetStringField("var_name", VariableName);

	return VariableName;
}
//
FString DialogueNode::GetVariableValue()
{
	return RawData->GetStringField("value");
}