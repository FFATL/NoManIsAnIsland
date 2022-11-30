// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleObject/DarkestGameObject.h"
#include "Kismet/KismetMathLibrary.h"

UDarkestGameObject::UDarkestGameObject() 
{
}

void UDarkestGameObject::InitializeGameSession(int32 NewHintGiverNumber, int32 NewRequiredTurn, float NewTrueDirectionFailWeight, float NewTrapWeight)
{
	bGameOngoing = true;
	RequiredTurn = NewRequiredTurn;
	CompletedTurn = 0;
	int32 UpperLimit = RequiredTurn / 2;
	int32 LowerLimit = -RequiredTurn / 2;
	TargetX = FMath::RandRange(LowerLimit, UpperLimit);
	TargetY = FMath::RandRange(LowerLimit, UpperLimit);
	GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::White, FString::Printf(TEXT("Destination : (%d, %d)"), TargetX, TargetY));
	HintGiverNumber = NewHintGiverNumber;
	
	SafeLocation = TMap<int32, FDarkestGameGrid>();
	CurrentX = 0;
	CurrentY = 0;
	SafeLocation.Emplace(0, FDarkestGameGrid());
	SafeLocation[0].Horizontal_Coord.Add(0);

	TrapLocation = TMap<int32, FDarkestGameGrid>();
	TrueDirectionFailWeight = NewTrueDirectionFailWeight;
	TrapWeight = NewTrapWeight;
}

TMap<int32, EDarkestDirection> UDarkestGameObject::GetDirectionHint() 
{
	// Get true direction
	EDarkestDirection ApproachDirection = EDarkestDirection::None;
	EDarkestDirection SecondaryDirection = EDarkestDirection::None;
	if (CurrentX != TargetX || CurrentY != TargetY) {
		float Degree = UKismetMathLibrary::DegAtan2(TargetY - CurrentY, TargetX - CurrentX);
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::White, FString::Printf(TEXT("Degree: %.2f [y = %d / x = %d]"), Degree, TargetY - CurrentY, TargetX - CurrentX));
		if (Degree < 0) {
			Degree = 360 + Degree;
		}
		if (Degree >= 45 && Degree < 135) {
			ApproachDirection = EDarkestDirection::Up;
			if (Degree >= 90) {
				SecondaryDirection = EDarkestDirection::Left;
			}
			else {
				SecondaryDirection = EDarkestDirection::Right;
			}
		}
		else if (Degree >= 135 && Degree < 225) {
			ApproachDirection = EDarkestDirection::Left;
			if (Degree >= 180) {
				SecondaryDirection = EDarkestDirection::Down;
			}
			else {
				SecondaryDirection = EDarkestDirection::Up;
			}
		}
		else if (Degree >= 225 && Degree < 315) {
			ApproachDirection = EDarkestDirection::Down;
			if (Degree >= 270) {
				SecondaryDirection = EDarkestDirection::Right;
			}
			else {
				SecondaryDirection = EDarkestDirection::Left;
			}
		}
		else {
			ApproachDirection = EDarkestDirection::Right;
			if (Degree >= 0) {
				SecondaryDirection = EDarkestDirection::Up;
			}
			else {
				SecondaryDirection = EDarkestDirection::Down;
			}
		}
	}
	GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::White, FString::Printf(TEXT("Primary: %s; Srcondary: %s"), *UEnum::GetValueAsString(ApproachDirection), *UEnum::GetValueAsString(SecondaryDirection)));

	// Check all possible directions
	TArray<EDarkestDirection> DirectionPriorityArray = TArray<EDarkestDirection>();
	if (!IsGetResultInLocationMap(CurrentX, CurrentY, ApproachDirection, TrapLocation)) {
		DirectionPriorityArray.AddUnique(ApproachDirection);
	}
	if (!IsGetResultInLocationMap(CurrentX, CurrentY, SecondaryDirection, TrapLocation)) {
		DirectionPriorityArray.AddUnique(SecondaryDirection);
	}
	for (int32 i = 1; i < 5; i++) {
		EDarkestDirection CurrentDirection = static_cast<EDarkestDirection>(i);
		if (CurrentDirection != ApproachDirection && CurrentDirection != SecondaryDirection && CurrentDirection != PreviousDirectionOpposite) {
			if (!IsGetResultInLocationMap(CurrentX, CurrentY, CurrentDirection, TrapLocation)) {
				DirectionPriorityArray.AddUnique(CurrentDirection);
			}
		}
	}

	// Process hints
	EDarkestDirection TruthDirection = EDarkestDirection::None;
	EDarkestDirection GuessDirection = EDarkestDirection::None;
	GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::White, FString::Printf(TEXT("Number of possible directions: %d"), DirectionPriorityArray.Num()));
	if (DirectionPriorityArray.Num() == 0) {
		TruthDirection = PreviousDirectionOpposite;
		GuessDirection = PreviousDirectionOpposite;
	}
	else if (DirectionPriorityArray.Num() == 1) {
		TruthDirection = DirectionPriorityArray[0];
		GuessDirection = DirectionPriorityArray[0];
	}
	else if (DirectionPriorityArray.Num() >= 2) {
		if (UKismetMathLibrary::RandomBoolWithWeight(TrueDirectionFailWeight)) {
			RegisterGetResultInLocationMap(CurrentX, CurrentY, DirectionPriorityArray[0], TrapLocation);
			TruthDirection = DirectionPriorityArray[1];
		}
		else {
			TruthDirection = DirectionPriorityArray[0];
		}

		GuessDirection = DirectionPriorityArray[FMath::RandRange(0, DirectionPriorityArray.Num() - 1)];
		while (GuessDirection == TruthDirection) {
			GuessDirection = DirectionPriorityArray[FMath::RandRange(0, DirectionPriorityArray.Num() - 1)];
		}
		if (UKismetMathLibrary::RandomBoolWithWeight(TrapWeight)) {
			RegisterGetResultInLocationMap(CurrentX, CurrentY, GuessDirection, TrapLocation);
		}
	}
	RegisterGetResultInLocationMap(CurrentX, CurrentY, TruthDirection, SafeLocation);
	for (EDarkestDirection tag : DirectionPriorityArray) {
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::White, FString::Printf(TEXT("%s"), *UEnum::GetValueAsString(tag)));
	}
	GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, FString::Printf(TEXT("Guess: %s"), *UEnum::GetValueAsString(GuessDirection)));
	
	// Spwan map to return
	TMap<int32, EDarkestDirection> NewHintMap = TMap<int32, EDarkestDirection>();
	for (int32 i = 0; i < HintGiverNumber; i++) {
		if (i == TruthGiverIndex) {
			NewHintMap.Emplace(i, TruthDirection);
		}
		else {
			NewHintMap.Emplace(i, GuessDirection);
		}
	}
	return NewHintMap;
}

EExploreCondition UDarkestGameObject::GetCheckResult(EDarkestDirection Choice) 
{
	TArray<int32> CoordSet = GetLocationFromDirection(CurrentX, CurrentY, Choice);
	if (CoordSet[0] == TargetX && CoordSet[1] == TargetY) {
		bGameOngoing = false;
		return EExploreCondition::Win;
	}
	else if (IsInLocationMap(CoordSet[0], CoordSet[1], TrapLocation)) {
		bGameOngoing = false;
		return EExploreCondition::Trap;
	}
	else {
		CompletedTurn++;
		if (CompletedTurn > RequiredTurn) {
			bGameOngoing = false;
			return EExploreCondition::Fail;
		}
		else {
			CurrentX = CoordSet[0];
			CurrentY = CoordSet[1];
			switch (Choice)
			{
			case EDarkestDirection::Right:
				PreviousDirectionOpposite = EDarkestDirection::Left;
				break;
			case EDarkestDirection::Up:
				PreviousDirectionOpposite = EDarkestDirection::Down;
				break;
			case EDarkestDirection::Left:
				PreviousDirectionOpposite = EDarkestDirection::Right;
				break;
			case EDarkestDirection::Down:
				PreviousDirectionOpposite = EDarkestDirection::Up;
				break;
			default:
				PreviousDirectionOpposite = EDarkestDirection::None;
				break;
			}

			RegisterLocationMap(CurrentX, CurrentY, SafeLocation);
			GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::White, FString::Printf(TEXT("Current Location: (%d ,%d); Destination : (%d, %d)"), CurrentX, CurrentY, TargetX, TargetY));
			return EExploreCondition::Safe;
		}
	}
}

void UDarkestGameObject::RegisterLocationMap(int32 LocX, int32 LocY, TMap<int32, FDarkestGameGrid>& MapRef)
{
	if (MapRef.Contains(LocY)) {
		MapRef[LocY].Horizontal_Coord.AddUnique(LocX);
	}
	else {
		MapRef.Emplace(LocY, FDarkestGameGrid());
		MapRef[LocY].Horizontal_Coord.Add(LocX);
	}
}

bool UDarkestGameObject::IsInLocationMap(int32 LocX, int32 LocY, TMap<int32, FDarkestGameGrid>& MapRef)
{
	if (MapRef.Contains(LocY)) {
		return MapRef[LocY].Horizontal_Coord.Contains(LocX);
	}
	else {
		return false;
	}
}

TArray<int32> UDarkestGameObject::GetLocationFromDirection(int32 LocX, int32 LocY, EDarkestDirection Direction)
{
	TArray<int32> CoordSet;
	switch (Direction)
	{
	case EDarkestDirection::None:
		CoordSet.Add(LocX);
		CoordSet.Add(LocY);
		break;
	case EDarkestDirection::Right:
		CoordSet.Add(LocX + 1);
		CoordSet.Add(LocY);
		break;
	case EDarkestDirection::Up:
		CoordSet.Add(LocX);
		CoordSet.Add(LocY + 1);
		break;
	case EDarkestDirection::Left:
		CoordSet.Add(LocX - 1);
		CoordSet.Add(LocY);
		break;
	case EDarkestDirection::Down:
		CoordSet.Add(LocX);
		CoordSet.Add(LocY - 1);
		break;
	default:
		break;
	}
	return CoordSet;
}

bool UDarkestGameObject::IsGetResultInLocationMap(int32 LocX, int32 LocY, EDarkestDirection Direction, TMap<int32, FDarkestGameGrid>& MapRef)
{
	TArray<int32> CoordSet = GetLocationFromDirection(LocX, LocY, Direction);
	return IsInLocationMap(CoordSet[0], CoordSet[1], MapRef);
}

void UDarkestGameObject::RegisterGetResultInLocationMap(int32 LocX, int32 LocY, EDarkestDirection Direction, TMap<int32, FDarkestGameGrid>& MapRef) 
{
	TArray<int32> CoordSet = GetLocationFromDirection(LocX, LocY, Direction);
	RegisterLocationMap(CoordSet[0], CoordSet[1], MapRef);
}