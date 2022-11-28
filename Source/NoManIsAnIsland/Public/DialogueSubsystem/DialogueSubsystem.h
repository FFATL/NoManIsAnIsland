// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "DialogueSubsystem/DialogueSession.h"
#include "DialogueSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class NOMANISANISLAND_API UDialogueSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

    UPROPERTY()
    FDialogueConfig LoadConfig;

    UPROPERTY()
    TMap<FString, UDialogueSession*> SessionArchive;

    UPROPERTY()
    FDialogueLine EndLine;

public:

    // Begin USubsystem
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;
    // End USubsystem

    UFUNCTION(BlueprintCallable, Exec)
    void InitializeDialogueConfig();

    UFUNCTION(BlueprintCallable, Exec)
    void SpawnSessions();

    UFUNCTION(BlueprintCallable, Exec)
    UDialogueSession* GetSessions(FString ConfigKey);

    UFUNCTION(BlueprintCallable, Exec)
    FDialogueLine& GetEndLine() { return EndLine; };

    UFUNCTION(BlueprintCallable, Exec)
    bool GetCheckMapValue(FName ConfigKey) { return LoadConfig.CheckMap[ConfigKey]; };
    UFUNCTION(BlueprintCallable, Exec)
    bool SetCheckMapValue(FName ConfigKey, bool NewValue);

};
