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

	//ƽ Ȱ��ȭ �� ���� ����
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

	// Ÿ�̸� �ڵ��� ������ ���� �� �� �ִ�. ������ �� ������ ��� ���� ������ �����ϴ�.
	GetWorldTimerManager().SetTimer(TimerHandle_BotSpawner, this, &ASGameMode::SpawnBotTimerElapsed, 1.0f, true, 0.0f);
	//��ϵ� TimerHandel, �����ų GameMode�� obj?,�۵���ų �Լ�, �����ð�(1�ʸ��� �Լ� ����), �ݺ�����, ���� ������ ( ó�� ������ ���� ������ )

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
	//TimerHandle_NextWaveStart�� Ȱ��ȭ �Ǿ� �ִ��� �ľ�
	bool bIsPrepareingForWave = GetWorldTimerManager().IsTimerActive(TimerHandle_NextWaveStart);

	if (NrOfBotsToSpawn > 0 || bIsPrepareingForWave)
	{
		return;
	}

	bool bIsAnyBotAlive = false;

	// ��� ������ �� �Ǵ� ������ �ִ� ���� �� ����� ����
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
