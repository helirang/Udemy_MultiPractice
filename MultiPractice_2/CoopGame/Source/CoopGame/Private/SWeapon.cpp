// Fill out your copyright notice in the Description page of Project Settings.

#include "SWeapon.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Components//SkeletalMeshComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "CoopGame.h"
#include "PhysicalMaterials//PhysicalMaterial.h"
#include "TimerManager.h"

static int32 DebugWeaponDrawing = 0;
FAutoConsoleVariableRef CVARDebugWeaponDrawing(
	TEXT("COOP.DebugWeapons"),
	DebugWeaponDrawing,
	TEXT("Draw Debug Lines for Weapons"),
	ECVF_Cheat);

// Sets default values
ASWeapon::ASWeapon()
{
	MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComp"));
	RootComponent = MeshComp;

	MuzzleSocketName = "MuzzleSocket";
	TracerTargetName = "Target";
	BaseDamage = 20.0f;

	RateOfFire = 600;
}

void ASWeapon::BeginPlay()
{
	Super::BeginPlay();

	TimeBetweenShots = 60 / RateOfFire;
}

void ASWeapon::StartFire()
{
	float FirstDelay = FMath::Max(LastFireTime + TimeBetweenShots - GetWorld()->TimeSeconds, 0.0f);
	//FMath::Max == (1,2) 1 또는 2중에서 큰 놈을 고른다. 즉 1이 음수이면 2인 0.0f가 반환된다.
	GetWorldTimerManager().SetTimer(TimerHandle_TimeBetweenShots, this, &ASWeapon::Fire, TimeBetweenShots, true, FirstDelay);
	//지연 1.0f, 발사할 시작하면 1초마다 호출
}

void ASWeapon::StopFire()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_TimeBetweenShots);
}

void ASWeapon::Fire()
{
	//누가 WEAPON의 소유주인지, 또한 들고 있는 사람이 누구인지 알아야된다
	AActor* MyOwner = GetOwner();
	if (MyOwner)
	{
		FVector EyeLocation;
		FRotator EyeRotation;
		MyOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation); //Owner의 ActorEyes관점을 사용
		//위치값과 회전값이 있으며(파라미터로?), 참조로 전달된다.

		FVector ShotDirection = EyeRotation.Vector();
		FVector TraceEnd = EyeLocation + (ShotDirection * 10000);

		FCollisionQueryParams QueryParams; // 충돌쿼리, 구조체
		QueryParams.AddIgnoredActor(MyOwner);//()에 포함된 액터 무시
		QueryParams.AddIgnoredActor(this);
		QueryParams.bTraceComplex = true; //우리가 치고 있는 메시의 각 개별 삼각형에 대해 추적한다.
		QueryParams.bReturnPhysicalMaterial = true;
		//And now what this does, it will trace against each individual triangle off the mesh that we're hitting.
		//TraceComplex는 비용이 비싸지만 우리가 무엇을 쳤는지에 대한 정확한 결과를 제공한다.
		//Trace가 false이면 경계 상자와 같이 단순 수집만 수행 합니다.
		//If this is a false, it will only do a simple collection that it could be a simple like bounding box

		//Particle "Target" parameter
		FVector TracerEndPoint = TraceEnd;


		FHitResult Hit; // 무엇을 쳤는지, 얼마나 멀리 떨어져 있는지 등의 여러 데이터가 담겨있는 구조체
		if (GetWorld()->LineTraceSingleByChannel(Hit, EyeLocation, TraceEnd, ECC_GameTraceChannel1, QueryParams))
		{
			//Hit 구조체, EyeLocation(출발 위치), TraceEnd(도착 위치), 채널( 겹침, 차단 등등 체크하는 채널), 충돌쿼리
			//Blocking hit! Process damage 

			AActor* HitActor = Hit.GetActor();

			EPhysicalSurface SurfaceType = UPhysicalMaterial::DetermineSurfaceType(Hit.PhysMaterial.Get());

			float ActualDamage = BaseDamage;
			if (SurfaceType == SURFACE_FLESHVULNERABLE)
			{
				ActualDamage *= 4.0f;
			}

			UGameplayStatics::ApplyPointDamage(HitActor, ActualDamage, ShotDirection, Hit, MyOwner->GetInstigatorController(), this, DamageType);
			//HitActor, 20데미지, 발사방향, Hit구조체, 발사자 정보, 무기정보(?),
			//TSubclassOf<UDamageType>로 화염 데미지나 독 데미지 구현 가능

			//Hit.PhysMaterial은 약한 객체 포인터로, 메모리에 보관되는것이 보장되지 않는다.
			//예를 들어, 이 구조체가 이 물리적 재료 포인터를 사용하는 유일한 유일한 구조체이면 엔진은 메모리에서 이를 제거할 수 있다.
			// 번역투라 어렵다. 약한 객체 포인터 TWeakObjectPointer( TWeakObjectPtr )
			// 예) 내가 마지막으로 사용하는 경우 배우가 삭제될 수 있으며, 강제로 삭제하고 싶지 않습니다.

			UParticleSystem* SelectedEffect = nullptr;

			switch (SurfaceType)
			{
			case SURFACE_FLESHDEFAULT:
			case SURFACE_FLESHVULNERABLE:
				SelectedEffect = FleshImpactEffect;
				break;
			default:
				SelectedEffect = DefaultImpactEffect;
				break;
			}

			if (SelectedEffect)
			{
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SelectedEffect, Hit.ImpactPoint, Hit.ImpactNormal.Rotation());
				//ImpactPoint 도착지, ImpactNormal( 방향을 알기 위해 사용 )
			}

			TracerEndPoint = Hit.ImpactPoint;
		}

		if (DebugWeaponDrawing > 0)
		{
			DrawDebugLine(GetWorld(), EyeLocation, TraceEnd, FColor::White, false, 1.0f, 0, 1.0f);
		}

		PlayFireEffects(TracerEndPoint);

		LastFireTime = GetWorld()->TimeSeconds;
	}
}

void ASWeapon::PlayFireEffects(FVector TracerEnd)
{
	if (MuzzleEffect)
	{
		UGameplayStatics::SpawnEmitterAttached(MuzzleEffect, MeshComp, MuzzleSocketName);
	}

	if (TracerEffect)
	{
		FVector MuzzleLocation = MeshComp->GetSocketLocation(MuzzleSocketName);

		UParticleSystemComponent* TracerComp = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), TracerEffect, MuzzleLocation);//파티클 생성, 생성위치 MuzzleLocation
		if (TracerComp)
		{
			TracerComp->SetVectorParameter(TracerTargetName, TracerEnd);
			//Target 파라미터에 TracerEndPoint( 파티클 시스템의 도착지 )를 넘겨준다.
		}
	}

	APawn* MyOwner = Cast<APawn>(GetOwner());
	if (MyOwner)
	{
		APlayerController* PC = Cast<APlayerController>(MyOwner->GetController());
		if (PC)
		{
			PC->ClientPlayCameraShake(FireCamShake);
		}
	}
}
