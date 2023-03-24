// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/DataTable.h"
#include "Misc/Paths.h"
#include "DialogueNode.h"
#include "DialogueComponent.generated.h"

USTRUCT(BlueprintType)
struct FDialogueVariablesStruct : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY()
		int32 node_type;

	UPROPERTY()
		FString node_value;
};

USTRUCT()
struct FDialogueStruct : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY()
		FString NPC_Name;

	UPROPERTY()
		FString NPC_Text;

	UPROPERTY()
		TMap<FString, FString> Answers_Text;
};

UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CULTURALHERITAGEGJ2_API UDialogueComponent : public UActorComponent
{
	GENERATED_BODY()
	//
	FString CharacterName;
	// 
	FString CurrentNodeName;
	//
	DialogueNode NodeContent;
	//
	TArray<TSharedPtr<FJsonValue>> RawData;
	//
	TMap<FString, DialogueNode> Datatable;
	//
	//UPROPERTY(EditDefaultsOnly)
	class UDataTable* DialogueData;
	//
	class UDataTable* DialogueVariablesData;

public:	
	// Sets default values for this component's properties
	UDialogueComponent();
	//
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Dialogue)
	FString DialogueFileName;
	//
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Dialogue)
	bool bHasDialogue;
	//
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Dialogue)
	FString InventoryObject;
	//
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Dialogue)
	bool bObjectInInventory;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	//
	void LoadDialogueNodes(TArray<TSharedPtr<FJsonValue>> NodesArr);
	//
	void LoadDialogueVariables(TSharedPtr<FJsonObject> VariablesRawData);
	//
	void SetDialogue();

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	//
	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	void LoadDialogue();
	//
	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	void GetDialogue();
	//
	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	void MakeTheChoice(FString DialogueText);
	// 
	UFUNCTION(BlueprintImplementableEvent, Category = "Dialogue")
	void CheckInventory();
	// 
	UFUNCTION(BlueprintImplementableEvent, Category = "Dialogue")
	bool RemoveFromInventory();
};
