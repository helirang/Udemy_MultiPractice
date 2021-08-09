// Fill out your copyright notice in the Description page of Project Settings.

#include "STrackerBot.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "AI/Navigation//NavigationSystem.h"
#include "AI/Navigation/NavigationPath.h"
#include "GameFramework/Character.h"
#include "DrawDebugHelpers.h"
#include "SHealthComponent.h"
#include "SCharacter.h"
#include "Components/SphereComponent.h"  

// Sets default values
ASTrackerBot::ASTrackerBot()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	//������ �ִϸ��̼��� ������ �׸��� ���� ������, StaticMesh�� ����Ѵ�.
	MeshComp->SetCanEverAffectNavigation(false);
	MeshComp->SetSimulatePhysics(true);

	RootComponent = MeshComp;

	HealthComp = CreateDefaultSubobject<USHealthComponent>(TEXT("HelathComp"));
	HealthComp->OnHealthChanged.AddDynamic(this, &ASTrackerBot::HandleTakeDamage);
	// OnHealthChanged�̺�Ʈ�� �߰�. ����δ� ����(���꽺ũ������)��� �Ѵ�.
	//HandleTakeDamage �Լ��� OnHealthChanged�� �μ��� ���ƾ� �Ѵ�(?)

	SphereCom = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCom"));
	SphereCom->SetSphereRadius(200);
	SphereCom->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SphereCom->SetCollisionResponseToAllChannels(ECR_Ignore);
	SphereCom->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap); // ECR_Overlap ���������� ó���ϴ� ����� �ٿ��ش�. �˾ƺ���
	SphereCom->SetupAttachment(RootComponent);

	bUseVelocityChange = false;
	MovementForce = 1000;
	RequiredDistanceToTarget = 100;

	ExplosionDamage = 40;
	ExplosionRadius = 200;
}

// Called when the game starts or when spawned
void ASTrackerBot::BeginPlay()
{
	Super::BeginPlay();
	
	// Find initial move-to
	// �ʱ� �̵� ã��
	NextPathPoint = GetNextPathPoint();

}

// Called every frame
void ASTrackerBot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	float DistanceToTarget = (GetActorLocation() - NextPathPoint).Size();

	//if(!GetActorLocation().Equals(NextPathPoint))�� �����̴� ��ü�� ��Ȯ�� ���� ���� ��ġ�ϱ� ����Ƿ� �Ʒ��� ���� �����Ѵ�.
	if (DistanceToTarget <= RequiredDistanceToTarget)
	{
		NextPathPoint = GetNextPathPoint();

		DrawDebugString(GetWorld(), GetActorLocation(), "Target Reached!");
	}
	else
	{
		// Keep moving towards next target
		FVector ForceDirection = NextPathPoint - GetActorLocation();
		ForceDirection.Normalize();

		ForceDirection *= MovementForce;

		MeshComp->AddForce(ForceDirection, NAME_None, bUseVelocityChange);

		DrawDebugDirectionalArrow(GetWorld(), GetActorLocation(), GetActorLocation() + ForceDirection, 32, FColor::Yellow, false, 0.0f,0,1.0f);
	}

	DrawDebugSphere(GetWorld(), NextPathPoint, 20, 12, FColor::Yellow, false, 0.0f, 1.0f);
}

FVector ASTrackerBot::GetNextPathPoint()
{
	// Hack, to get player location // Hack���� ���̺��� �����Ͽ� �÷��̾ ���� ���̴�. (���߿�)
	ACharacter* PlayerPawn = UGameplayStatics::GetPlayerCharacter(this, 0); //��Ƽ�÷��̾�� �������� ����.

	UNavigationPath* NavPath = UNavigationSystem::FindPathToActorSynchronously(this, GetActorLocation(), PlayerPawn);
	//AI�� ��ġ�� ����[����]�� ������� �˷�����Ѵ�.

	if( NavPath->PathPoints.Num() > 1) // �迭�� ũ�Ⱑ 1���� �۴ٸ� ������ ��ġ���� ���� ���̴�.
	{
		//Return next point in the path
		return NavPath->PathPoints[1];
	}

	//Failed to find path
	return GetActorLocation();
}

void ASTrackerBot::HandleTakeDamage(USHealthComponent* OwningHealthComp, float Health, float HealthDelta, const class UDamageType* DamageType,
	class AController* InstigatedBy, AActor* DamageCauser)
{
	if (MatInst == nullptr)
	{
		MatInst = MeshComp->CreateAndSetMaterialInstanceDynamicFromMaterial(0, MeshComp->GetMaterial(0));
		// ��� ���ο� ���� ������ �Ǽ�, ���̳��� ���׸����� �����.
		// ���� �ν��Ͻ��� �ƴ� �ν��ͽ��� ������ �����ϸ�, �̹� ������ ��ġ�Ǿ� �ִ� �ν��Ͻ��鵵 ���� ����ȴ�.
		// ��Ÿ�ӿ� �Ű������� �����Ϸ��� �׻� ������ ���� �ν��ͽ��� �����ؾ��Ѵ�.
		// �� �ڵ带 ���� ��� ���� �ν��Ͻ��� ���� TrackBot�� �Ű������� ������ �� �ִ� ������ �ִ�.
	}

	if (MatInst)
	{
		MatInst->SetScalarParameterValue("LastTimeDamageTaken", GetWorld()->TimeSeconds);
	}

	UE_LOG(LogTemp, Log, TEXT("Health %s of %s"), *FString::SanitizeFloat(Health), *GetName());

	// Explode on hitpoints == 0 , ü�� 0�̸� ����
	if (Health <= 0.0f)
	{
		SelfDistruct();
	}
}

void ASTrackerBot::SelfDistruct()
{
	if (bExploded)
	{
		return;
	}

	bExploded = true;

	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionEffect, GetActorLocation());

	TArray<AActor*> IgnoredActors;
	IgnoredActors.Add(this);

	//Apply Damage
	UGameplayStatics::ApplyRadialDamage(this, ExplosionDamage, GetActorLocation(), ExplosionRadius, nullptr, IgnoredActors, this, GetInstigatorController(), true);
	// ������ 100, ���� GetActorLocation, ���عݰ� 200, nullpts, ������ ���� �迭 ���, ���� ���� this, Get, �⺻������ �ΰ� �������� �������� ���� bool

	DrawDebugSphere(GetWorld(), GetActorLocation(), ExplosionRadius, 12, FColor::Red, false, 2.0f, 0, 1.0f);

	// Delete Actor immediately
	Destroy();
}

void ASTrackerBot::NotifyActorBeginOverlap(AActor* OtherActor)
{
	if (!bStartedSelfDestruction)
	{
		ASCharacter* PlayerPawn = Cast<ASCharacter>(OtherActor);
		if (PlayerPawn)
		{
			// We overlapped with a player
			// Start self destruction sequence
			// GetWorldTimerManager�� include ���� �����Ѵ�.
			GetWorldTimerManager().SetTimer(TimerHandle_SelfDamage, this, &ASTrackerBot::DamageSelf, 0.5f, true, 0.0f);

			bool bStartedSelfDestruction = true;
		}

	}
}

void ASTrackerBot::DamageSelf()
{
	UGameplayStatics::ApplyDamage(this, 20, GetInstigatorController(), this, nullptr);
	// �� Ÿ�̸� �̺�Ʈ �ν�Ƽ�������� ��� ƽ�� �ν�Ƽ������ ��Ʈ�ѷ��� ������ ������ �Ѵ�. == 3��° �Ű����� = GetInstigatorController
}

