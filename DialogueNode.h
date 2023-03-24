// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"

/**
 * 
 */
class CULTURALHERITAGEGJ2_API DialogueNode
{
	FString Language;
	TSharedPtr<FJsonObject> RawData;

public:
	//
	FString Name;
	FString Type;
	TMap<FString, FString> DialoguesChoices;

protected:
	//
	FString Next(TSharedPtr<FJsonObject> NodeData);
	//
	FString GetTextContent(TSharedPtr<FJsonObject> NodeData);

public:
	DialogueNode();
	DialogueNode(TSharedPtr<FJsonObject> JsonData);
	~DialogueNode();
	//
	void Status();
	//
	FString Next();
	//
	FString GetCharacterName();
	//
	FString GetTextContent();
	//
	FString GetConditionBranch(FString ConditionValue);
	//
	void BuildChoices();
	//
	FString MakeTheChoice(FString DialogueText);
	//
	FString GetVariableName();
	//
	FString GetVariableValue();
};
