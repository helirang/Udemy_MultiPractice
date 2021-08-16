// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SHealthComponent.generated.h"

//OnHealthChanged event
DECLARE_DYNAMIC_MULTICAST_DELEGATE_SixParams(FOnHealthChangedSignature,USHealthComponent*, HealthComp, float, Health, float, HealthDelta, const class UDamageType*, DamageType, class AController*, InstigatedBy, AActor*, DamageCauser);
// (1.이벤트 유형의 이름 / 2부터 매개변수 / 현재 프로젝트에서 사용한 매개 변수 2.트리거한 상태 구성 요소, 3. 체력 변수, 4.체력 변경용 변수, 
// 5부터는 HandleTakeAnyDamage과 연관되기 때문에 해당 함수 관련 매개변수를 가져옴)

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
	//손상을 입으면 OnTakeDamage 이벤트와 일치해야되기 때문에  OnTakeDamage가 요구하는 매개 변수를 동일하게 사용한다.
	//UFUNCTION은 이벤트 또는 대리자와 함께 사용하는 경우 매우매우 중요하다.

	UFUNCTION()
	void OnRep_Health(float OldHealth);

	// Called when the game starts
	virtual void BeginPlay() override;

public:
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnHealthChangedSignature OnHealthChanged;
	//BlueprintAssignable == 대리자를 청사진에 사용 할 수 있다.

protected:
	UPROPERTY(ReplicatedUsing=OnRep_Health,BlueprintReadOnly, Category = "HealthComponent")
	float Health;

	UPROPERTY(EditAnywhere ,BlueprintReadWrite, Category = "HealthComponent")
	float DefaultHealth;
};
