// Fill out your copyright notice in the Description page of Project Settings.


#include "DialogueComponent.h"

// Sets default values for this component's properties
UDialogueComponent::UDialogueComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	// ...

	// Getting datatable for stocking dialogue variables
	static ConstructorHelpers::FObjectFinder<UDataTable> DialogueVariablesObject(TEXT("DataTable'/Game/Blueprints/Dialogues/DT_Dialogue_Variables.DT_Dialogue_Variables'"));
	if (DialogueVariablesObject.Succeeded())
	{
		DialogueVariablesData = DialogueVariablesObject.Object;
	}

	// Getting datatable for stocking dialogue texts
	static ConstructorHelpers::FObjectFinder<UDataTable> DialogueDataObject(TEXT("DataTable'/Game/Blueprints/Dialogues/DT_Dialogue.DT_Dialogue'"));
	if (DialogueDataObject.Succeeded())
	{
		DialogueData = DialogueDataObject.Object;
	}

	// Initializing
	CharacterName = "";
	CurrentNodeName = "START";
}

// ...
void UDialogueComponent::LoadDialogue()
{
	bHasDialogue = false;
	// Getting dialogue file
	const FString JsonFilePath = FPaths::ProjectContentDir() + "Blueprints/Dialogues/Json/" + DialogueFileName + ".json";

	if (!FPaths::FileExists(JsonFilePath)) {
		GLog->Log("LogCustom: dialogue file not found: " + JsonFilePath);
	}
	else
	{
		// Loading file content
		FString JsonString;
		FFileHelper::LoadFileToString(JsonString, *JsonFilePath);
		TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create("{\"data\": " + JsonString + "}");

		// Logging
		//GLog->Log("LogCustom: Json Path:");
		//GLog->Log("LogCustom: " + JsonFilePath);
		//GLog->Log("LogCustom: Json String:");
		//GLog->Log("LogCustom: " + JsonString);

		// Converting content to JSON object
		if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
		{
			// Getting JSON objects
			TArray<TSharedPtr<FJsonValue>> RawDataArray = JsonObject->GetArrayField("data");
			TSharedPtr<FJsonObject> RawDataObject = RawDataArray[0]->AsObject();

			// Getting nodes part
			LoadDialogueNodes(RawDataObject->GetArrayField("nodes"));

			// Getting variables part
			LoadDialogueVariables(RawDataObject->GetObjectField("variables"));
		}
		else
		{
			GLog->Log("LogCustom: couldn't deserialize");
		}
	}
}

//
void UDialogueComponent::LoadDialogueNodes(TArray<TSharedPtr<FJsonValue>> NodesArr)
{
	RawData = NodesArr;
	//GLog->Log("LogCustom: getting dialogue nodes...");
	for (int32 index = 0; index < NodesArr.Num(); index++)
	{
		// Adding to datatable
		DialogueNode* NodeElement = new DialogueNode(NodesArr[index]->AsObject());
		Datatable.Add(NodeElement->Name, *NodeElement);
	}
}

//
void UDialogueComponent::LoadDialogueVariables(TSharedPtr<FJsonObject> VariablesRawData)
{

	//GLog->Log("LogCustom: populating variables...");
	for (auto& Elem : VariablesRawData->Values)
	{
		TSharedPtr<FJsonObject> SingleVariable = VariablesRawData->GetObjectField(Elem.Key);
		//GLog->Log("LogCustom: " + Elem.Key);
		// Getting datatable element
		FDialogueVariablesStruct Variable;
		Variable.node_type = SingleVariable->GetIntegerField("type");
		Variable.node_value = SingleVariable->GetStringField("value");
		// Adding row
		DialogueVariablesData->AddRow(FName(Elem.Key), Variable);
	}
}

// 
void UDialogueComponent::GetDialogue()
{
	GLog->Log("LogCustom: GetDialogue");
	//
	if (CurrentNodeName == "null")
	{
		bHasDialogue = false;
	}
	else
	{
		if (CurrentNodeName.Len() > 0) {
			NodeContent = Datatable.FindRef(CurrentNodeName);
			NodeContent.Status();
			//
			if (CharacterName == "")
			{
				CharacterName = NodeContent.GetCharacterName();
			}


			GLog->Log("LogCustom: " + NodeContent.Type);
			bool bIsRecursive = false;
			if (NodeContent.Type == "start")
			{
				CurrentNodeName = NodeContent.Next();
				bIsRecursive = true;
			}
			else if (NodeContent.Type == "condition_branch")
			{
				bIsRecursive = true;
				FString ConditionValue = "False";
				FString TextValue = NodeContent.GetTextContent();
				FString* Verb = new FString();
				FString* Variable = new FString();
				if (TextValue.Split(" ", Verb, Variable))
				{
					// TODO: process regarding to Verb value and get variable on character
					if (*Verb == "have")
					{
						bObjectInInventory = true;
						InventoryObject = *Variable;
						GLog->Log("LogCustom: InventoryObject=" + InventoryObject);
						CheckInventory();
						GLog->Log("LogCustom: InventoryObject=" + InventoryObject);
					}
				}
				else
				{
					// Getting variable from dialogue datatable
					static const FString ContextString(TEXT("Dialogue"));
					FDialogueVariablesStruct* VariableContent = DialogueVariablesData->FindRow<FDialogueVariablesStruct>(FName(TextValue), ContextString, false);
					if (VariableContent)
					{
						ConditionValue = VariableContent->node_value;
					}
				}
				//
				GLog->Log("LogCustom: ConditionValue is " + ConditionValue);
				CurrentNodeName = NodeContent.GetConditionBranch(ConditionValue);
			}
			else if (NodeContent.Type == "show_message")
			{
				GLog->Log(NodeContent.GetTextContent());
				bHasDialogue = true;
				SetDialogue();
			}
			else if (NodeContent.Type == "set_local_variable")
			{
				FString VariableName = NodeContent.GetVariableName();
				if (VariableName != "")
				{
					static const FString ContextString(TEXT("Dialogue"));
					FString VariableValue = NodeContent.GetVariableValue();
					FDialogueVariablesStruct* VariableContent = DialogueVariablesData->FindRow<FDialogueVariablesStruct>(FName(VariableName), ContextString, false);
					if (VariableContent)
					{
						VariableContent->node_value = VariableValue;
					}
					//
					CurrentNodeName = NodeContent.Next();
					bIsRecursive = true;
				}
				else
				{
					CurrentNodeName = NodeContent.Next();
				}
			}
			else
			{
				CurrentNodeName = NodeContent.Next();
			}

			if (bIsRecursive)
			{
				GetDialogue();
			}
		}
		else
		{
			bHasDialogue = false;
			GLog->Log("LogCustom: unable to get dialogue.");
		}
	}
}

//
void UDialogueComponent::SetDialogue()
{

	//
	NodeContent.BuildChoices();
	//
	DialogueData->EmptyTable();
	// Getting datatable
	FDialogueStruct Dialogue;
	TMap<FString, FString> DialogueChoice;
	Dialogue.NPC_Name = CharacterName;
	Dialogue.NPC_Text = NodeContent.GetTextContent();
	//
	if (NodeContent.DialoguesChoices.Num() > 0)
	{
		for (auto& Elem : NodeContent.DialoguesChoices)
		{
			DialogueChoice.Add(Elem.Value, Elem.Key);
		}
		Dialogue.Answers_Text = DialogueChoice;
	}
	else
	{
		DialogueChoice.Add(NodeContent.Next(), "END");
	}
	// Adding row
	DialogueData->AddRow(FName(CurrentNodeName), Dialogue);
}

//
void UDialogueComponent::MakeTheChoice(FString DialogueText)
{
	DialogueNode NodeElement = Datatable.FindRef(CurrentNodeName);

	GLog->Log("LogCustom: NodeContent " + NodeElement.Name);
	GLog->Log("LogCustom: making choice from " + CurrentNodeName + " to " + DialogueText);
	CurrentNodeName = NodeElement.MakeTheChoice(DialogueText);
	GLog->Log("LogCustom: CurrentNodeName " + CurrentNodeName);
	//
	GetDialogue();
}


// Called when the game starts
void UDialogueComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UDialogueComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}