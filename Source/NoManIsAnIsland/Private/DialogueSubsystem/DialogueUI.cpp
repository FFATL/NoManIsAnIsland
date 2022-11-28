// Fill out your copyright notice in the Description page of Project Settings.


#include "DialogueSubsystem/DialogueUI.h"
#include "Kismet/GameplayStatics.h"
#include "Components/TextBlock.h"
#include "Components/RichTextBlock.h"
#include "Components/Button.h"
#include "Components/Border.h"
#include "Components/Image.h"
#include "Components/CheckBox.h"
#include "Components/ScrollBox.h"

void UDialogueUI::NativeConstruct()
{
	Super::NativeConstruct();
	
	ChoiceSlot_SoftClassPtr = TSoftClassPtr<UDialogueChoiceSlot>(FSoftClassPath("/Game/Dialogue/BP_DialogueChoiceSlot.BP_DialogueChoiceSlot_C"));
	ChoiceSlotClass = ChoiceSlot_SoftClassPtr.Get();
}

void UDialogueUI::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	Button_Forward->OnClicked.AddDynamic(this, &UDialogueUI::PrintLine);
	TextBlock->SetAutoWrapText(true);
}

void UDialogueUI::SetSession(FString ConfigKey)
{
	UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(this);
	UDialogueSubsystem* DialogueSubsystem = GameInstance->GetSubsystem<UDialogueSubsystem>();
	CurrentSession = DialogueSubsystem->GetSessions(ConfigKey);
	if (CurrentSession) {
		int32 index = 0;
		for (auto& item : CurrentSession->SessionConfig.DisplayNameDataMap) {
			if (item.Value.Texture != nullptr && !item.Value.bSkipSpriteProcessing) {
				if (index == 0) {
					Sprite_Left->SetBrushFromTexture(item.Value.Texture);
					if (item.Value.bSpriteInstantShown) {
						Sprite_Left->SetVisibility(ESlateVisibility::Visible);
					}
					ImageMap.Emplace(item.Key, Sprite_Left);
					index++;
				}
				else if (index == 1) {
					Sprite_Right->SetBrushFromTexture(item.Value.Texture);
					if (item.Value.bSpriteInstantShown) {
						Sprite_Right->SetVisibility(ESlateVisibility::Visible);
					}
					ImageMap.Emplace(item.Key, Sprite_Right);
					index++;
				}
				else if (index == 2) {
					Sprite_Center->SetBrushFromTexture(item.Value.Texture);
					if (item.Value.bSpriteInstantShown) {
						Sprite_Center->SetVisibility(ESlateVisibility::Visible);
					}
					ImageMap.Emplace(item.Key, Sprite_Center);
					index++;
				}
				else if (index == 3) {
					Sprite_4->SetBrushFromTexture(item.Value.Texture);
					if (item.Value.bSpriteInstantShown) {
						Sprite_4->SetVisibility(ESlateVisibility::Visible);
					}
					ImageMap.Emplace(item.Key, Sprite_4);
					break;
				}
			}
		}

		PrintLine();
	}
}

void UDialogueUI::PrintLine()
{
	if (CurrentSession) {
		OnNextLine.Broadcast();
		OnNextLine.RemoveAll(this);
		
		FDialogueLine& CurrentLine = CurrentSession->GetLine();
		if (CurrentLine.IsEndLine()) {
			if (bNeedNextSceneHint) {
				if (bReachTheEnd) {
					SelfDestroy();
				}
				else {
					FName NewHint = FName("Next Scene");
					TB_ForwardHint->SetText(FText::FromName(NewHint));
					Border_NameBlock->SetVisibility(ESlateVisibility::Collapsed);
					Border_TextBlock->SetVisibility(ESlateVisibility::Collapsed);
					PreDialogueSessionEnd.Broadcast();
					bReachTheEnd = true;
				}
			}
			else {
				SelfDestroy();
			}
		}
		else {
			PrintLineCore(CurrentLine);
		}
	}
}

void UDialogueUI::SelfDestroy()
{
	OnDialogueSessionEnd.Broadcast();
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::White, TEXT("OnDialogueSessionEnd.Broadcast()"));
	if (CurrentSession->SessionConfig.bResetProgressAtEnd) {
		CurrentSession->ResetProgress();
	}
	RemoveFromViewport();
	MarkAsGarbage();
}

void UDialogueUI::BP_SetInputChoice(FString ChosenHeader)
{
	SetInputChoice(ChosenHeader);
}

void UDialogueUI::PrintLineCore(FDialogueLine& LineStruct)
{
	FName& DisplayName = LineStruct.Core.DisplayName;
	FName& Line = LineStruct.Core.Line;
	FLinearColor NameColor;

	if (LineStruct.Core.bShowDisplayName) {
		NameBlock->SetText(FText::FromName(DisplayName));
	}
	else {
		NameBlock->SetText(FText());
	}
	TextBlock->SetText(FText::FromName(Line));

	TMap<FName, FDialogueDisplayData>& DataMap = CurrentSession->SessionConfig.DisplayNameDataMap;
	if (DataMap.Contains(DisplayName)) {
		NameColor = DataMap[DisplayName].Color;
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, NameColor.QuantizeRound(), FString::Printf(TEXT("%s: %s"), *DisplayName.ToString(), *Line.ToString()));
	}
	else {
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::White, FString::Printf(TEXT("%s: %s"), *DisplayName.ToString(), *Line.ToString()));
	}
	NameBlock->SetColorAndOpacity(NameColor);
	
	if (LineStruct.bNeedOptions) {
		GetWorld()->GetTimerManager().ClearTimer(AutoTimer);
		Border_ChoiceSection->SetVisibility(ESlateVisibility::Visible);
		SB_Choice->ClearChildren();
		for (auto& item : LineStruct.OptionMap) {
			NewSlot = CreateWidget<UDialogueChoiceSlot>(this, ChoiceSlotClass);
			NewSlot->SetHeaderText(item.Key);
			NewSlot->OnDialogueChoiceSignal.AddDynamic(this, &UDialogueUI::BP_SetInputChoice);
			SB_Choice->AddChild(NewSlot);
		}
		Button_Forward->SetVisibility(ESlateVisibility::Collapsed);
	}

	for (EDialogueLineEffect& LineEffect : LineStruct.Effect) {
		switch (LineEffect)
		{
		case EDialogueLineEffect::BroadcastOne:
			OnDialogueSignalOne.Broadcast();
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::White, TEXT("OnDialogueSignalOne.Broadcast()"));
			break;

		case EDialogueLineEffect::BroadcastTwo:
			OnDialogueSignalTwo.Broadcast();
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::White, TEXT("OnDialogueSignalTwo.Broadcast()"));
			break;

		case EDialogueLineEffect::ShowSprite:
			if (ImageMap.Contains(DisplayName)) {
				ImageMap[DisplayName]->SetVisibility(ESlateVisibility::Visible);
			}
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::White, TEXT("EDialogueLineEffect::ShowSprite"));
			break;

		case EDialogueLineEffect::HideSprite:
			if (ImageMap.Contains(DisplayName)) {
				OnNextLine.AddLambda([=]() {ImageMap[DisplayName]->SetVisibility(ESlateVisibility::Hidden);; });
			}
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::White, TEXT("EDialogueLineEffect::HideSprite"));
			break;

		case EDialogueLineEffect::Forward:
			if (!LineStruct.bNeedOptions) {
				GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::White, TEXT("EDialogueLineEffect::Forward"));
				PrintLine();
			}
			
			return;

		default:
			break;
		}
	}

	if (CheckBox_Auto->GetCheckedState() == ECheckBoxState::Checked && !LineStruct.bNeedOptions) {
		GetWorld()->GetTimerManager().SetTimer(AutoTimer, this, &UDialogueUI::PrintLine, 0.5f, false);
	}

}

void UDialogueUI::SetInputChoice(FString ChosenHeader)
{
	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::White, FString::Printf(TEXT("Receive [%s]"), *ChosenHeader));
	FDialogueLine& CurrentLine = CurrentSession->GetLine();
	if (!CurrentLine.bChoosingDone) {
		CurrentLine.bChoosingDone = true;
		if (CurrentLine.OptionMap.Contains(ChosenHeader)) {
			CurrentLine.OptionChoice = ChosenHeader;
			SB_Choice->ClearChildren();
			Border_ChoiceSection->SetVisibility(ESlateVisibility::Collapsed);
			Button_Forward->SetVisibility(ESlateVisibility::Visible);
			PrintLine();
		}
		else {
			GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, FString::Printf(TEXT("[%s] not in CurrentLine.OptionMap"), *ChosenHeader));
			UE_LOG(LogTemp, Warning, TEXT("Error: [%s]"), *ChosenHeader);
			for (auto& item : CurrentLine.OptionMap) {
				UE_LOG(LogTemp, Warning, TEXT("Source: <%s>"), *ChosenHeader);
			}
			CurrentLine.bChoosingDone = false;
		}
	}
}
