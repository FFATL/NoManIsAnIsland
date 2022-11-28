// Fill out your copyright notice in the Description page of Project Settings.


#include "DialogueSubsystem/DialogueSession.h"
#include "Kismet/GameplayStatics.h"
#include "DialogueSubsystem/DialogueSubsystem.h"

UDialogueSession::UDialogueSession()
{
}

void UDialogueSession::InitializeLocalMeta(FString& MetaPath, int32 SetVersion)
{
	FString JsonPath = FPaths::ProjectDir() + FString::Printf(TEXT("Content/Dialogue/Data/SessionMeta/%s.json"), *MetaPath);
	JsonPath = FPaths::ConvertRelativePathToFull(JsonPath);

	if (FPlatformFileManager::Get().GetPlatformFile().FileExists(*JsonPath)) {
		FString JsonData;
		FFileHelper::LoadFileToString(JsonData, *JsonPath);

		TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(JsonData);
		TSharedPtr<FJsonObject> RootJsonObj = MakeShareable(new FJsonObject);

		if (FJsonSerializer::Deserialize(JsonReader, RootJsonObj)) {
			FJsonObjectConverter::JsonObjectStringToUStruct<FDialogueMeta>(JsonData, &SessionConfig, 0, 0);
			CurrentVersion = SetVersion;
			PreSetVersion = SetVersion;
			for (FString& path : SessionConfig.LineArray) {
				InitializeJsonArray(path);
			}
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

void UDialogueSession::InitializeJsonArray(FString& DialoguePath, bool bIsPrimary, UDialogueSession* ParentSessionPtr)
{
	FString JsonPath;
	if (bIsPrimary) {
		JsonPath = FPaths::ProjectDir() + FString::Printf(TEXT("Content/Dialogue/Data/PrimarySession/%s.json"), *DialoguePath);
	}
	else {
		JsonPath = FPaths::ProjectDir() + FString::Printf(TEXT("Content/Dialogue/Data/SecondarySession/%s.json"), *DialoguePath);
		ParentSessionWeakPtr = TWeakObjectPtr<UDialogueSession>(ParentSessionPtr);
	}
	
	JsonPath = FPaths::ConvertRelativePathToFull(JsonPath);

	if (FPlatformFileManager::Get().GetPlatformFile().FileExists(*JsonPath)) {
		FString JsonData;
		FFileHelper::LoadFileToString(JsonData, *JsonPath);

		TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(JsonData);
		TSharedPtr<FJsonObject> RootJsonObj = MakeShareable(new FJsonObject);

		if (FJsonSerializer::Deserialize(JsonReader, RootJsonObj)) {
			FDialogueJson& NewSlot = LineArrayJsonArray.AddDefaulted_GetRef();
			FJsonObjectConverter::JsonObjectStringToUStruct<FDialogueJson>(JsonData, &NewSlot, 0, 0);
			VersionProgress.Emplace(LineArrayJsonArray.Num() - 1, 0);
			CheckOptions(NewSlot);
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

void UDialogueSession::CheckOptions(FDialogueJson& Slot)
{
	for (FDialogueLine& line : Slot.Dialogue) {
		if (line.bNeedOptions) {
			for (auto& item : line.OptionMap) {
				UDialogueSession* NewSession = NewObject<UDialogueSession>(this);
				NewSession->InitializeJsonArray(item.Value, false, this);
				OptionMap.Emplace(item.Key, NewSession);
			}
		}
	}
}

bool UDialogueSession::GetLocalCheckMapValue(FName CheckKey)
{
	if (ParentSessionWeakPtr.IsValid()) {
		return ParentSessionWeakPtr.Get()->GetLocalCheckMapValue(CheckKey);
	}
	else {
		return SessionConfig.CheckMap[CheckKey];
	}
}

FDialogueLine& UDialogueSession::GetLine()
{
	int32& VersionProgressValue = VersionProgress[CurrentVersion];
	if (VersionProgressValue >= LineArrayJsonArray[CurrentVersion].Dialogue.Num()) {
		// Get next version because current version ran out of lines
		int32 NextVersion = CurrentVersion + 1;
		if (VersionProgress.Contains(NextVersion)) {
			CurrentVersion = NextVersion;
			int32& NextVersionProgressValue = VersionProgress[CurrentVersion];
			return GetLineCore(NextVersionProgressValue);
		}
		else {
			// No more versions so end this session
			UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(this);
			UDialogueSubsystem* DialogueSubsystem = GameInstance->GetSubsystem<UDialogueSubsystem>();
			return DialogueSubsystem->GetEndLine();
		}
	}
	else {
		// Get line
		return GetLineCore(VersionProgressValue);
	}
}

FDialogueLine& UDialogueSession::GetLineCore(int32& ProgressIndex)
{
	FDialogueLine& CurrentLine = LineArrayJsonArray[CurrentVersion].Dialogue[ProgressIndex];
	bool CheckArrayPass = true;
	for (FDialogueCheck& check : CurrentLine.CheckArray) {
		bool KeyValue;
		if (check.bGlobalKey) {
			UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(this);
			UDialogueSubsystem* DialogueSubsystem = GameInstance->GetSubsystem<UDialogueSubsystem>();
			KeyValue = DialogueSubsystem->GetCheckMapValue(check.Key);
		}
		else {
			KeyValue = GetLocalCheckMapValue(check.Key);
		}
		if (KeyValue != check.RequireValue) {
			CheckArrayPass = false;
			break;
		}
	}

	if (CheckArrayPass) {
		if (CurrentLine.bNeedOptions) {
			if (CurrentLine.bChoosingDone) {
				FDialogueLine& GetOptionLine = OptionMap[CurrentLine.OptionChoice]->GetLine();
				if (GetOptionLine.IsEndLine()) {
					ProgressIndex += 1;
					return GetLine();
				}
				else {
					return GetOptionLine;
				}
			}
			else {
				return CurrentLine;
			}
		}
		else {
			ProgressIndex += 1;
			return CurrentLine;
		}
	}
	else {
		ProgressIndex += 1;
		return GetLine();
	}
}

void UDialogueSession::ResetProgress()
{
	CurrentVersion = PreSetVersion;
	for (auto& item : VersionProgress) {
		VersionProgress[item.Key] = 0;
	}

	for (FDialogueJson& slot : LineArrayJsonArray) {
		for (FDialogueLine& line : slot.Dialogue) {
			if (line.bNeedOptions) {
				line.bChoosingDone = false;
			}
		}
	}

	for (auto& item : OptionMap) {
		item.Value->ResetProgress();
	}
}
