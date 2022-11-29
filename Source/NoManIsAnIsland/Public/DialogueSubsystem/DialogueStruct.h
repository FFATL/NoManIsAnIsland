// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "DialogueStruct.generated.h"

UENUM(BlueprintType)
enum class EDialogueLineEffect : uint8
{
	None,
	BroadcastOne,
	BroadcastTwo,
	ShowSprite,
	HideSprite,
	Forward

};

USTRUCT(BlueprintType)
struct FDialogueCheck
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName Key;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool RequireValue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bGlobalKey;

};

USTRUCT(BlueprintType)
struct FDialogueDisplayData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FLinearColor Color;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* Texture = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bSpriteInstantShown;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bSkipSpriteProcessing;

};

/**
 * Set Font config in RichText
 */
USTRUCT(BlueprintType)
struct FDialogueLineCore
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bShowDisplayName = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName Line;

};

USTRUCT(BlueprintType)
struct FDialogueLine
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FDialogueLineCore Core;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<EDialogueLineEffect> Effect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FDialogueCheck> CheckArray;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bNeedOptions;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FString, FString> OptionMap;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString OptionChoice;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bChoosingDone;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bHideOptions;

	bool IsEndLine() {
		if (Effect.Contains(EDialogueLineEffect::Forward) || bNeedOptions) {
			return false;
		}
		else {
			return Core.Line == FName();
		}
	};

};

USTRUCT(BlueprintType)
struct FDialogueJson : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FDialogueLine> Dialogue;

};

/**
 * Local Config : Used by UDialogueSession
 */
USTRUCT(BlueprintType)
struct FDialogueMeta : public FTableRowBase
{
	GENERATED_BODY()

	/** Local check map, independent of global config*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FName, bool> CheckMap;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FName, FDialogueDisplayData> DisplayNameDataMap;

	/** Parallel sessions in one session*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FString> LineArray;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 InitialVersion;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bResetProgressAtEnd;

};

/**
 * Global Config : Used by UDialogueSubsystem
 */
USTRUCT(BlueprintType)
struct FDialogueConfig : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FName, bool> CheckMap;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FString, int32> MetaVersionMap;

};

