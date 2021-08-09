// Fill out your copyright notice in the Description page of Project Settings.

#include "SHealthComponent.h"
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
USHealthComponent::USHealthComponent()
{
	DefaultHealth = 100;

	SetIsReplicated(true);
}

// Called when the game starts
void USHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	// Only hook if we are Server //컴포넌트에는 Role이 없으며 자체 액터에만 있다.
	// 따라서 컴포넌트에서 if (Role = ROLE_Authority)을 사용하면 에러가 뜬다.
	if (GetOwnerRole() == ROLE_Authority) //체력 계산을 서버가 하게한다. //Client가 체력 계산을 하는 것은 보안적으로 위험이 높다
	{
		AActor* MyOwner = GetOwner();
		if (MyOwner)
		{
			MyOwner->OnTakeAnyDamage.AddDynamic(this, &USHealthComponent::HandleTakeAnyDamage);
			//OnTakeAnyDamage 이벤트에 바인딩한다.
		}

		Health = DefaultHealth;
	}
}

void USHealthComponent::OnRep_Health(float OldHealth)
{
	float Damage = Health - OldHealth;

	OnHealthChanged.Broadcast(this, Health, Damage,nullptr, nullptr, nullptr);
}

void USHealthComponent::HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	if (Damage <= 0.0f)
	{
		return;
	}

	//Update health Clampd
	Health = FMath::Clamp(Health - Damage, 0.0f, DefaultHealth);

	//로깅용
	UE_LOG(LogTemp, Log, TEXT("Health Changed: %s"), *FString::SanitizeFloat(Health));

	OnHealthChanged.Broadcast(this, Health, Damage, DamageType, InstigatedBy, DamageCauser);
}

void USHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const //무엇을 복제하고 어떤 복제방법을 선택할지 정하는 함수?
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(USHealthComponent, Health); //랩 수명에 대한 매크로 // 복제할 위치에 대한 가장 간단한 기본 사양]
	// 해당 매크로는 서버와 연결된 모든 관련 클라이언트에 대한 리플리케이터를 만한다.
}
