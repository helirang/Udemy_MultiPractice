// Fill out your copyright notice in the Description page of Project Settings.

#include "SGameMode.h"
#include "TimerManager.h"

void ASGameMode::StartWave()
{
	WaveCount++;

	NrOfBotsToSpawn = 2 * WaveCount;

	// 타이머 핸들이 있으면 추적 할 수 있다. 추적할 수 있으면 취소 등의 행위가 가능하다.
	GetWorldTimerManager().SetTimer(TimerHandle_BotSpawner, &ASGameMode::SpawnBotTimerElapsed, 1.0f, true, 0.0f);
	//기록될 TimerHandel, 작동시킬 함수, 지연시간(1초마다 함수 실행), 반복여부, 시작 딜레이 ( 처음 시작할 때의 딜레이 )
}

void ASGameMode::StartPlay()
{
	Super::StartPlay();

	PrepareForNextWave();
}

void ASGameMode::SpawnBotTimerElapsed()
{
	SpawnNewBot();

	NrOfBotsToSpawn--;

	if (NrOfBotsToSpawn <= 0)
	{
		EndWave();
	}
}

