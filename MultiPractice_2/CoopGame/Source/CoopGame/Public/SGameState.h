// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "SGameState.generated.h"

UENUM(BlueprintType) //HUD에 사용할 때, BP를 사용하면 편하다?
enum class EWaveState : uint8
{
	WatingToStart,

	WaveInProgress,

	// No longer spawning new bots, waiting for players to kill remaining
	WatingToComplete,

	WaveComplete,

	GameOver,
};

/**
 * 
 */
UCLASS()
class COOPGAME_API ASGameState : public AGameStateBase
{
	GENERATED_BODY()

protected:
	UFUNCTION()
	void OnRep_WaveState(EWaveState OldState);
	
	// OldState 이전 상태를 알아야 예시) 기존 장착 장비 해제 및 새로운 장비 장착을 할 수 있다.
	// 대충 이전 상태 해제용
	UFUNCTION(BlueprintImplementableEvent, Category = "GameState")
	void WaveStateChange(EWaveState NewState, EWaveState OldState);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_WaveState, Category = "GameState")
	EWaveState WaveState;
	
public:
	
	void SetWaveState(EWaveState NewState);
};
