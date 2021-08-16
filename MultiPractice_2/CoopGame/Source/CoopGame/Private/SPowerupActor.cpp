// Fill out your copyright notice in the Description page of Project Settings.

#include "SPowerupActor.h"
#include "Net/UnrealNetwork.h"


// Sets default values
ASPowerupActor::ASPowerupActor()
{
	PowerupInterval = 0.0f;
	TotalNrOfTicks = 0;

	bIsPowerupActive = false;

	SetReplicates(true);
}

void ASPowerupActor::OnTickPowerup()
{
	TicksProcessed++;

	OnPowerupTicked();

	if (TicksProcessed >= TotalNrOfTicks)
	{
		OnExPIRED();

		// Delete timer
		GetWorldTimerManager().ClearTimer(TimerHandle_PowerupTick);
	}

}

void ASPowerupActor::OnRep_PowerupActive()
{
	OnPowerupStateChanged(bIsPowerupActive);
}

void ASPowerupActor::ActivatePowerup()
{
	OnActivated();
	//활성화 호출

	bIsPowerupActive = true;
	OnRep_PowerupActive();

	if (PowerupInterval > 0.0f)
	{
		GetWorldTimerManager().SetTimer(TimerHandle_PowerupTick, this, &ASPowerupActor::OnTickPowerup, PowerupInterval, true /*,0.0f*/);
		// 0.0f 제거, 하나의 full 틱만 있으면 됨,즉시 발생 but 첫번째 인터페이스가 통과되야 발생 시작
	}
	{
		OnTickPowerup();
	}
}

void ASPowerupActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const //무엇을 복제하고 어떤 복제방법을 선택할지 정하는 함수?
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASPowerupActor, bIsPowerupActive)
}
