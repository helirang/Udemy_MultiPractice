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

	// Only hook if we are Server //������Ʈ���� Role�� ������ ��ü ���Ϳ��� �ִ�.
	// ���� ������Ʈ���� if (Role = ROLE_Authority)�� ����ϸ� ������ ���.
	if (GetOwnerRole() == ROLE_Authority) //ü�� ����� ������ �ϰ��Ѵ�. //Client�� ü�� ����� �ϴ� ���� ���������� ������ ����
	{
		AActor* MyOwner = GetOwner();
		if (MyOwner)
		{
			MyOwner->OnTakeAnyDamage.AddDynamic(this, &USHealthComponent::HandleTakeAnyDamage);
			//OnTakeAnyDamage �̺�Ʈ�� ���ε��Ѵ�.
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

	//�α��
	UE_LOG(LogTemp, Log, TEXT("Health Changed: %s"), *FString::SanitizeFloat(Health));

	OnHealthChanged.Broadcast(this, Health, Damage, DamageType, InstigatedBy, DamageCauser);
}

void USHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const //������ �����ϰ� � ��������� �������� ���ϴ� �Լ�?
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(USHealthComponent, Health); //�� ���� ���� ��ũ�� // ������ ��ġ�� ���� ���� ������ �⺻ ���]
	// �ش� ��ũ�δ� ������ ����� ��� ���� Ŭ���̾�Ʈ�� ���� ���ø������͸� ���Ѵ�.
}
