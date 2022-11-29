// Fill out your copyright notice in the Description page of Project Settings.


#include "DialogueSubsystem/DialogueSubsystem.h"

void UDialogueSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	EndLine = FDialogueLine();
	InitializeDialogueConfig();

}

void UDialogueSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

void UDialogueSubsystem::InitializeDialogueConfig()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::White, TEXT("UDialogueSubsystem::InitializeDialogueConfig()"));
	FString JsonPath = FPaths::ProjectDir() + FString(TEXT("Content/Dialogue/Data/DialogueConfig.json"));
	JsonPath = FPaths::ConvertRelativePathToFull(JsonPath);

	if (FPlatformFileManager::Get().GetPlatformFile().FileExists(*JsonPath)) {
		FString JsonData;
		FFileHelper::LoadFileToString(JsonData, *JsonPath);

		TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(JsonData);
		TSharedPtr<FJsonObject> RootJsonObj = MakeShareable(new FJsonObject);

		if (FJsonSerializer::Deserialize(JsonReader, RootJsonObj)) {
			FJsonObjectConverter::JsonObjectStringToUStruct<FDialogueConfig>(JsonData, &LoadConfig, 0, 0);
			SpawnSessions();
		}
		else {
			GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, TEXT("FJsonSerializer::Deserialize failed"));
		}
	}
	else {
		GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, FString::Printf(TEXT("Cannot access %s. Check the path."), *JsonPath));
		UE_LOG(LogTemp, Warning, TEXT("Non Exist Path: %s"), *JsonPath);
	}
}

void UDialogueSubsystem::SpawnSessions()
{
	for (auto& item : LoadConfig.MetaVersionMap) {
		UDialogueSession* NewSession = NewObject<UDialogueSession>(this);
		NewSession->InitializeLocalMeta(item.Key, item.Value);
		SessionArchive.Emplace(item.Key, NewSession);
	}
}

UDialogueSession* UDialogueSubsystem::GetSessions(FString ConfigKey)
{
	return SessionArchive.Contains(ConfigKey) ? SessionArchive[ConfigKey] : nullptr;
}

bool UDialogueSubsystem::SetCheckMapValue(FName ConfigKey, bool NewValue)
{
	if (LoadConfig.CheckMap.Contains(ConfigKey)) {
		LoadConfig.CheckMap[ConfigKey] = NewValue;
		return true;
	}
	else {
		return false;
	}
}
