// Fill out your copyright notice in the Description page of Project Settings.


#include "DialogueSubsystem/DialogueChoiceSlot.h"
#include "Components/Button.h"
#include "Components/RichTextBlock.h"

void UDialogueChoiceSlot::NativeConstruct()
{
	Super::NativeConstruct();
}

void UDialogueChoiceSlot::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	Button_Main->OnClicked.AddDynamic(this, &UDialogueChoiceSlot::BroadcastResult);
}

void UDialogueChoiceSlot::SetHeaderText(FString NewHeaderText)
{
	Header->SetText(FText::FromString(NewHeaderText));
}

void UDialogueChoiceSlot::BroadcastResult()
{
	OnDialogueChoiceSignal.Broadcast(Header->GetText().ToString());
}
