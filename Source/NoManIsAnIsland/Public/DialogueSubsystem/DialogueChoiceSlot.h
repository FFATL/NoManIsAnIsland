// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DialogueChoiceSlot.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDialogueChoiceSignal, FString, ChosenHeader);

/**
 * 
 */
UCLASS()
class NOMANISANISLAND_API UDialogueChoiceSlot : public UUserWidget
{
	GENERATED_BODY()

protected:

	virtual void NativeConstruct() override;

	virtual void NativeOnInitialized() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UButton* Button_Main;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class URichTextBlock* Header;

public:

	UPROPERTY(BlueprintAssignable, Category = "Event: Player Input")
	FDialogueChoiceSignal OnDialogueChoiceSignal;

	UFUNCTION(BlueprintCallable, Exec)
	void SetHeaderText(FString NewHeaderText);
	UFUNCTION(BlueprintCallable, Exec)
	void BroadcastResult();
	
};
