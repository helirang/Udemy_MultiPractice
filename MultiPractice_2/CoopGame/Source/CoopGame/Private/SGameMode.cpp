// Fill out your copyright notice in the Description page of Project Settings.

#include "SGameMode.h"
#include "TimerManager.h"

void ASGameMode::StartWave()
{
	WaveCount++;

	NrOfBotsToSpawn = 2 * WaveCount;

	// Ÿ�̸� �ڵ��� ������ ���� �� �� �ִ�. ������ �� ������ ��� ���� ������ �����ϴ�.
	GetWorldTimerManager().SetTimer(TimerHandle_BotSpawner, &ASGameMode::SpawnBotTimerElapsed, 1.0f, true, 0.0f);
	//��ϵ� TimerHandel, �۵���ų �Լ�, �����ð�(1�ʸ��� �Լ� ����), �ݺ�����, ���� ������ ( ó�� ������ ���� ������ )
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

