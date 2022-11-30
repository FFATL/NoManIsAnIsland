// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "DarkestGameObject.generated.h"


UENUM(BlueprintType)
enum class EDarkestDirection : uint8
{
	None,
	Right,
	Up,
	Left,
	Down
};

UENUM(BlueprintType)
enum class EExploreCondition : uint8
{
	Safe,
	Trap,
	Fail,
	Win
};

USTRUCT(BlueprintType)
struct FDarkestGameGrid
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<int32> Horizontal_Coord;

};

/**
 * 
 */
UCLASS(BlueprintType)
class UDarkestGameObject : public UObject
{
	GENERATED_BODY()

	UPROPERTY()
	bool bGameOngoing;

	UPROPERTY()
	int32 RequiredTurn;

	UPROPERTY()
	int32 HintGiverNumber;

	UPROPERTY()
	int32 TruthGiverIndex;

	/** <y, {x}>*/
	UPROPERTY()
	TMap<int32, FDarkestGameGrid> SafeLocation;
	/** <y, {x}>*/
	UPROPERTY()
	TMap<int32, FDarkestGameGrid> TrapLocation;

	/** Weight = .6 : 60% chance the true direction will become a trap*/
	UPROPERTY()
	float TrueDirectionFailWeight;
	/** Weight = .6 : 60% chance the hint will become a trap*/
	UPROPERTY()
	float TrapWeight;

	UPROPERTY()
	int32 CompletedTurn;

	UPROPERTY()
	int32 CurrentX;
	UPROPERTY()
	int32 CurrentY;
	UPROPERTY()
	int32 TargetX;
	UPROPERTY()
	int32 TargetY;

	UPROPERTY()
	EDarkestDirection PreviousDirectionOpposite;

public:

	UDarkestGameObject();

	UFUNCTION(BlueprintCallable, Exec)
	void SetTruthGiverIndex(int32 NewIndex) { TruthGiverIndex = NewIndex; };

	UFUNCTION(BlueprintCallable, Exec)
	int32 GetCompletedTurn() { return CompletedTurn; };

	UFUNCTION(BlueprintCallable, Exec)
	void InitializeGameSession(int32 NewHintGiverNumber, int32 NewRequiredTurn, float NewTrueDirectionFailWeight, float NewTrapWeight);

	UFUNCTION(BlueprintCallable, Exec)
	TMap<int32, EDarkestDirection> GetDirectionHint();

	UFUNCTION(BlueprintCallable, Exec)
	EExploreCondition GetCheckResult(EDarkestDirection Choice);

	UFUNCTION(BlueprintCallable, Exec)
	void RegisterLocationMap(int32 LocX, int32 LocY, TMap<int32, FDarkestGameGrid>& MapRef);

	UFUNCTION(BlueprintCallable, Exec)
	bool IsInLocationMap(int32 LocX, int32 LocY, TMap<int32, FDarkestGameGrid>& MapRef);

	/** [x,y]*/
	UFUNCTION(BlueprintCallable, Exec)
	TArray<int32> GetLocationFromDirection(int32 LocX, int32 LocY, EDarkestDirection Direction);

	UFUNCTION(BlueprintCallable, Exec)
	bool IsGetResultInLocationMap(int32 LocX, int32 LocY, EDarkestDirection Direction, TMap<int32, FDarkestGameGrid>& MapRef);

	UFUNCTION(BlueprintCallable, Exec)
	void RegisterGetResultInLocationMap(int32 LocX, int32 LocY, EDarkestDirection Direction, TMap<int32, FDarkestGameGrid>& MapRef);

};
