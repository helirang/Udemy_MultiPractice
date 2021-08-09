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
	//실제로 애니메이션을 적용할 항목이 없기 때문에, StaticMesh를 사용한다.
	MeshComp->SetCanEverAffectNavigation(false);
	MeshComp->SetSimulatePhysics(true);

	RootComponent = MeshComp;

	HealthComp = CreateDefaultSubobject<USHealthComponent>(TEXT("HelathComp"));
	HealthComp->OnHealthChanged.AddDynamic(this, &ASTrackerBot::HandleTakeDamage);
	// OnHealthChanged이벤트에 추가. 영어로는 구독(서브스크라이퍼)라고 한다.
	//HandleTakeDamage 함수는 OnHealthChanged와 인수가 같아야 한다(?)

	SphereCom = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCom"));
	SphereCom->SetSphereRadius(200);
	SphereCom->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SphereCom->SetCollisionResponseToAllChannels(ECR_Ignore);
	SphereCom->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap); // ECR_Overlap 물리엔진이 처리하는 비용을 줄여준다. 알아보기
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
	// 초기 이동 찾기
	NextPathPoint = GetNextPathPoint();

}

// Called every frame
void ASTrackerBot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	float DistanceToTarget = (GetActorLocation() - NextPathPoint).Size();

	//if(!GetActorLocation().Equals(NextPathPoint))는 움직이는 물체가 정확한 벡터 값이 일치하기 힘드므로 아래와 같이 변경한다.
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
	// Hack, to get player location // Hack으로 레이블을 지정하여 플레이어를 얻을 것이다. (나중에)
	ACharacter* PlayerPawn = UGameplayStatics::GetPlayerCharacter(this, 0); //멀티플레이어에서 유지되지 않음.

	UNavigationPath* NavPath = UNavigationSystem::FindPathToActorSynchronously(this, GetActorLocation(), PlayerPawn);
	//AI가 위치한 세상[레벨]이 어디인지 알려줘야한다.

	if( NavPath->PathPoints.Num() > 1) // 배열의 크기가 1보다 작다면 현재의 위치만을 가진 것이다.
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
		// 계속 새로운 것을 만들어야 되서, 다이나믹 메테리얼을 사용함.
		// 동적 인스턴스가 아닌 인스터스의 변수를 변경하면, 이미 레벨에 배치되어 있는 인스턴스들도 전부 변경된다.
		// 런타임에 매개변수를 변경하려면 항상 일종의 동적 인스터스를 생성해야한다.
		// 위 코드를 예로 들면 동적 인스턴스라 단일 TrackBot의 매개변수만 변경할 수 있는 권한이 있다.
	}

	if (MatInst)
	{
		MatInst->SetScalarParameterValue("LastTimeDamageTaken", GetWorld()->TimeSeconds);
	}

	UE_LOG(LogTemp, Log, TEXT("Health %s of %s"), *FString::SanitizeFloat(Health), *GetName());

	// Explode on hitpoints == 0 , 체력 0이면 폭발
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
	// 데미지 100, 원점 GetActorLocation, 피해반경 200, nullpts, 무시할 액터 배열 목록, 피해 원인 this, Get, 기본값으로 두고 데미지를 적용할지 여부 bool

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
			// GetWorldTimerManager는 include 없이 동작한다.
			GetWorldTimerManager().SetTimer(TimerHandle_SelfDamage, this, &ASTrackerBot::DamageSelf, 0.5f, true, 0.0f);

			bool bStartedSelfDestruction = true;
		}

	}
}

void ASTrackerBot::DamageSelf()
{
	UGameplayStatics::ApplyDamage(this, 20, GetInstigatorController(), this, nullptr);
	// 이 타이머 이벤트 인스티게이터의 모든 틱은 인스티게이터 컨트롤러를 스스로 끄도록 한다. == 3번째 매개변수 = GetInstigatorController
}

