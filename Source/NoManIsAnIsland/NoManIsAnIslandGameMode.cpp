// Copyright Epic Games, Inc. All Rights Reserved.

#include "NoManIsAnIslandGameMode.h"
#include "NoManIsAnIslandCharacter.h"
#include "UObject/ConstructorHelpers.h"

ANoManIsAnIslandGameMode::ANoManIsAnIslandGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

}
