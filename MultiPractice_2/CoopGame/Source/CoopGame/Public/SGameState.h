// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "SGameState.generated.h"

UENUM(BlueprintType) //HUD�� ����� ��, BP�� ����ϸ� ���ϴ�?
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
	
	// OldState ���� ���¸� �˾ƾ� ����) ���� ���� ��� ���� �� ���ο� ��� ������ �� �� �ִ�.
	// ���� ���� ���� ������
	UFUNCTION(BlueprintImplementableEvent, Category = "GameState")
	void WaveStateChange(EWaveState NewState, EWaveState OldState);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_WaveState, Category = "GameState")
	EWaveState WaveState;
	
public:
	
	void SetWaveState(EWaveState NewState);
};
