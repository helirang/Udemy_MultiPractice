// Fill out your copyright notice in the Description page of Project Settings.

#include "SGameMode.h"
#include "SHealthComponent.h"
#include "SGameState.h"
#include "SPlayerState.h"
#include "TimerManager.h"	

ASGameMode::ASGameMode()
{
	TimeBetweenWaves = 2.0f;

	GameStateClass = ASGameMode::StaticClass();
	PlayerStateClass = ASPlayerState::StaticClass();

	//틱 활성화 및 간격 설정
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 1.0f;
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

void ASGameMode::StartWave()
{
	WaveCount++;

	NrOfBotsToSpawn = 2 * WaveCount;

	// 타이머 핸들이 있으면 추적 할 수 있다. 추적할 수 있으면 취소 등의 행위가 가능하다.
	GetWorldTimerManager().SetTimer(TimerHandle_BotSpawner, this, &ASGameMode::SpawnBotTimerElapsed, 1.0f, true, 0.0f);
	//기록될 TimerHandel, 실행시킬 GameMode의 obj?,작동시킬 함수, 지연시간(1초마다 함수 실행), 반복여부, 시작 딜레이 ( 처음 시작할 때의 딜레이 )

	SetWaveState(EWaveState::WaveInProgress);
}

void ASGameMode::StartPlay()
{
	Super::StartPlay();

	PrepareForNextWave();
}

void ASGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	CheckWaveState();
	CheckAnyPlayerAlive();
}

void ASGameMode::EndWave()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_BotSpawner);

	SetWaveState(EWaveState::WatingToComplete);
}

void ASGameMode::PrepareForNextWave()
{
	
	GetWorldTimerManager().SetTimer(TimerHandle_NextWaveStart, this, &ASGameMode::StartWave, TimeBetweenWaves, false);

	SetWaveState(EWaveState::WatingToStart);
}

void ASGameMode::CheckWaveState()
{
	//TimerHandle_NextWaveStart이 활성화 되어 있는지 파악
	bool bIsPrepareingForWave = GetWorldTimerManager().IsTimerActive(TimerHandle_NextWaveStart);

	if (NrOfBotsToSpawn > 0 || bIsPrepareingForWave)
	{
		return;
	}

	bool bIsAnyBotAlive = false;

	// 사용 가능한 폰 또는 레벨에 있는 유사 폰 목록이 유지
	for(FConstPawnIterator It = GetWorld()->GetPawnIterator(); It; ++It)
	{
		APawn* TestPawn = It->Get();
		if (TestPawn == nullptr || TestPawn->IsPlayerControlled())
		{
			continue;
		}

		USHealthComponent* HealthComp = Cast<USHealthComponent>(TestPawn->GetComponentByClass(USHealthComponent::StaticClass()));
		if (HealthComp && HealthComp->GetHealth() > 0.0f)
		{
			bIsAnyBotAlive = true;
			break;
		}
	}

	if (!bIsAnyBotAlive)
	{
		SetWaveState(EWaveState::WaveComplete);
		PrepareForNextWave();
	}
}

void ASGameMode::CheckAnyPlayerAlive()
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		APlayerController* PC = It->Get();
		if (PC && PC->GetPawn())
		{
			 APawn* MyPanw = PC->GetPawn();
			 USHealthComponent* HealthComp = Cast<USHealthComponent>(MyPanw->GetComponentByClass(USHealthComponent::StaticClass()));
			 if (ensure(HealthComp) && HealthComp->GetHealth() > 0.0f)
			 {
				// A player is still alive.
				 return;
			 }
		}
	}
	
	// No player alive
	GameOver();
}

void ASGameMode::GameOver()
{
	EndWave();

	// @TODO: Finish up the match, present 'game over' to players.

	SetWaveState(EWaveState::GameOver);

	UE_LOG(LogTemp, Log, TEXT("GAME OVER! Players Died"));
}

void ASGameMode::SetWaveState(EWaveState NewState)
{
	ASGameState* GS = GetGameState<ASGameState>();
	if (ensureAlways(GS))
	{
		GS->SetWaveState(NewState);
	}
}
