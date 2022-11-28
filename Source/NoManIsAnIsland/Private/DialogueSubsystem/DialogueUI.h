// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DialogueSubsystem/DialogueSubsystem.h"
#include "DialogueSubsystem/DialogueChoiceSlot.h"
#include "DialogueUI.generated.h"

DECLARE_MULTICAST_DELEGATE(FDialogueNextLine);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDialogueSignal);

/**
 * 
 */
UCLASS()
class UDialogueUI : public UUserWidget
{
	GENERATED_BODY()

	UPROPERTY()
	TSoftClassPtr<UDialogueChoiceSlot> ChoiceSlot_SoftClassPtr;

	UPROPERTY()
	UClass* ChoiceSlotClass;

	UPROPERTY()
	UDialogueSession* CurrentSession;

	UPROPERTY()
	bool bReachTheEnd;

	UPROPERTY()
	bool bNeedNextSceneHint;

	UPROPERTY()
	TMap<FName, class UImage*> ImageMap;

protected:

	virtual void NativeConstruct() override;

	virtual void NativeOnInitialized() override;

	UPROPERTY()
	class UDialogueChoiceSlot* NewSlot;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "DialogueSystem")
	class UBorder* Border_NameBlock;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "DialogueSystem")
	class UTextBlock* NameBlock;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "DialogueSystem")
	class UBorder* Border_TextBlock;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "DialogueSystem")
	class URichTextBlock* TextBlock;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "DialogueSystem")
	class UButton* Button_Forward;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "DialogueSystem")
	class UTextBlock* TB_ForwardHint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "DialogueSystem")
	class UCheckBox* CheckBox_Auto;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "DialogueSystem")
	class UBorder* Border_ChoiceSection;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "DialogueSystem")
	class UScrollBox* SB_Choice;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UImage* Sprite_Left;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UImage* Sprite_Right;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UImage* Sprite_Center;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UImage* Sprite_4;

public:

	FTimerHandle AutoTimer;

	UPROPERTY(BlueprintAssignable, Category = "Event: Progress")
	FDialogueSignal OnDialogueSignalOne;
	UPROPERTY(BlueprintAssignable, Category = "Event: Progress")
	FDialogueSignal OnDialogueSignalTwo;

	UPROPERTY(BlueprintAssignable, Category = "Event: End")
	FDialogueSignal PreDialogueSessionEnd;
	UPROPERTY(BlueprintAssignable, Category = "Event: End")
	FDialogueSignal OnDialogueSessionEnd;

	FDialogueNextLine OnNextLine;

	/** Bind all delegates before calling this function*/
	UFUNCTION(BlueprintCallable, Exec)
	void SetSession(FString ConfigKey);
	UFUNCTION(BlueprintCallable, Exec)
	void SetNeedNextSceneHint(bool NeedNextSceneHint) { bNeedNextSceneHint = NeedNextSceneHint; };
	UFUNCTION(BlueprintCallable, Exec)
	void PrintLine();
	UFUNCTION(BlueprintCallable, Exec)
	void SelfDestroy();
	UFUNCTION(BlueprintCallable, Exec)
	void BP_SetInputChoice(FString ChosenHeader);

	virtual void PrintLineCore(FDialogueLine& LineStruct);
	virtual void SetInputChoice(FString ChosenHeader);
};
