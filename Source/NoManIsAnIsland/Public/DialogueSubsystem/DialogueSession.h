// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "DialogueSubsystem/DialogueStruct.h"
#include "JsonObjectConverter.h"
#include "DialogueSession.generated.h"

/**
 * 
 */
UCLASS()
class NOMANISANISLAND_API UDialogueSession : public UObject
{
	GENERATED_BODY()

	UPROPERTY()
	int32 PreSetVersion;


	
public:

	UDialogueSession();

	UFUNCTION(BlueprintCallable, Exec)
	void InitializeLocalMeta(FString& MetaPath, int32 SetVersion);

	UFUNCTION(BlueprintCallable, Exec)
	void InitializeJsonArray(FString& DialoguePath, bool bIsPrimary = true, UDialogueSession* ParentSessionPtr = nullptr);

	UFUNCTION(BlueprintCallable, Exec)
	void CheckOptions(FDialogueJson& Slot);

	UFUNCTION(BlueprintCallable, Exec)
	bool GetLocalCheckMapValue(FName CheckKey);

	UFUNCTION(BlueprintCallable, Exec)
	FDialogueLine& GetLine();

	UFUNCTION(BlueprintCallable, Exec)
	FDialogueLine& GetLineCore(int32& ProgressIndex);

	UFUNCTION(BlueprintCallable, Exec)
	void ResetProgress();
	
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TWeakObjectPtr<UDialogueSession> ParentSessionWeakPtr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FDialogueMeta SessionConfig;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 CurrentVersion;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FDialogueJson> LineArrayJsonArray;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<int32, int32> VersionProgress;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FString, UDialogueSession*> OptionMap;

};
