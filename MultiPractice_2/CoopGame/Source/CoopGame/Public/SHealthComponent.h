// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SHealthComponent.generated.h"

//OnHealthChanged event
DECLARE_DYNAMIC_MULTICAST_DELEGATE_SixParams(FOnHealthChangedSignature,USHealthComponent*, HealthComp, float, Health, float, HealthDelta, const class UDamageType*, DamageType, class AController*, InstigatedBy, AActor*, DamageCauser);
// (1.�̺�Ʈ ������ �̸� / 2���� �Ű����� / ���� ������Ʈ���� ����� �Ű� ���� 2.Ʈ������ ���� ���� ���, 3. ü�� ����, 4.ü�� ����� ����, 
// 5���ʹ� HandleTakeAnyDamage�� �����Ǳ� ������ �ش� �Լ� ���� �Ű������� ������)

UCLASS(ClassGroup=(COOP), meta=(BlueprintSpawnableComponent) )
class COOPGAME_API USHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USHealthComponent();

public:
	UFUNCTION(BlueprintCallable, Category = "HealthComponent")
	void Heal(float HealAmount);

protected:
	UFUNCTION() 
	void HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);
	//�ջ��� ������ OnTakeDamage �̺�Ʈ�� ��ġ�ؾߵǱ� ������  OnTakeDamage�� �䱸�ϴ� �Ű� ������ �����ϰ� ����Ѵ�.
	//UFUNCTION�� �̺�Ʈ �Ǵ� �븮�ڿ� �Բ� ����ϴ� ��� �ſ�ſ� �߿��ϴ�.

	UFUNCTION()
	void OnRep_Health(float OldHealth);

	// Called when the game starts
	virtual void BeginPlay() override;

public:
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnHealthChangedSignature OnHealthChanged;
	//BlueprintAssignable == �븮�ڸ� û������ ��� �� �� �ִ�.

protected:
	UPROPERTY(ReplicatedUsing=OnRep_Health,BlueprintReadOnly, Category = "HealthComponent")
	float Health;

	UPROPERTY(EditAnywhere ,BlueprintReadWrite, Category = "HealthComponent")
	float DefaultHealth;
};
