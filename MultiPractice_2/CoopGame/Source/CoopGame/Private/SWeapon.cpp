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
#include "Net/UnrealNetwork.h"

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

	SetReplicates(true);

	NetUpdateFrequency = 66.0f;
	MinNetUpdateFrequency = 33.0f;
	// MinNetUpdateFrequency �⺻���� 2.0f��. 
	// ���� ������ �ʴ� 60���������� ����� ������ ����. 
	// NetUpdateFrequency�� �⺻�� 100.0f�� �����ؼ� 66.0f�� �����.
}

void ASWeapon::BeginPlay()
{
	Super::BeginPlay();

	TimeBetweenShots = 60 / RateOfFire;
}

void ASWeapon::StartFire()
{
	float FirstDelay = FMath::Max(LastFireTime + TimeBetweenShots - GetWorld()->TimeSeconds, 0.0f);
	//FMath::Max == (1,2) 1 �Ǵ� 2�߿��� ū ���� ����. �� 1�� �����̸� 2�� 0.0f�� ��ȯ�ȴ�.
	GetWorldTimerManager().SetTimer(TimerHandle_TimeBetweenShots, this, &ASWeapon::Fire, TimeBetweenShots, true, FirstDelay);
	//���� 1.0f, �߻��� �����ϸ� 1�ʸ��� ȣ��
}

void ASWeapon::StopFire()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_TimeBetweenShots);
}

void ASWeapon::Fire()
{
	/*We're going to check if the role is not role authorities, that it is basically something like a client.
		And if that is the case, we just call server fire and then we return.*/
	if (Role < ROLE_Authority) //������ �ƴϰ� Ŭ���̾�Ʈ�� ���� ���̸�
	{
		ServerFire();
	}

	//���� WEAPON�� ����������, ���� ��� �ִ� ����� �������� �˾ƾߵȴ�
	AActor* MyOwner = GetOwner();
	if (MyOwner)
	{
		FVector EyeLocation;
		FRotator EyeRotation;
		MyOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation); //Owner�� ActorEyes������ ���
		//��ġ���� ȸ������ ������(�Ķ���ͷ�?), ������ ���޵ȴ�.

		FVector ShotDirection = EyeRotation.Vector();
		FVector TraceEnd = EyeLocation + (ShotDirection * 10000);

		FCollisionQueryParams QueryParams; // �浹����, ����ü
		QueryParams.AddIgnoredActor(MyOwner);//()�� ���Ե� ���� ����
		QueryParams.AddIgnoredActor(this);
		QueryParams.bTraceComplex = true; //�츮�� ġ�� �ִ� �޽��� �� ���� �ﰢ���� ���� �����Ѵ�.
		QueryParams.bReturnPhysicalMaterial = true;
		//And now what this does, it will trace against each individual triangle off the mesh that we're hitting.
		//TraceComplex�� ����� ������� �츮�� ������ �ƴ����� ���� ��Ȯ�� ����� �����Ѵ�.
		//Trace�� false�̸� ��� ���ڿ� ���� �ܼ� ������ ���� �մϴ�.
		//If this is a false, it will only do a simple collection that it could be a simple like bounding box

		//Particle "Target" parameter
		FVector TracerEndPoint = TraceEnd;

		EPhysicalSurface SurfaceType = SurfaceType_Default;

		FHitResult Hit; // ������ �ƴ���, �󸶳� �ָ� ������ �ִ��� ���� ���� �����Ͱ� ����ִ� ����ü
		if (GetWorld()->LineTraceSingleByChannel(Hit, EyeLocation, TraceEnd, ECC_GameTraceChannel1, QueryParams))
		{
			//Hit ����ü, EyeLocation(��� ��ġ), TraceEnd(���� ��ġ), ä��( ��ħ, ���� ��� üũ�ϴ� ä��), �浹����
			//Blocking hit! Process damage 

			AActor* HitActor = Hit.GetActor();

			SurfaceType = UPhysicalMaterial::DetermineSurfaceType(Hit.PhysMaterial.Get());

			float ActualDamage = BaseDamage;
			if (SurfaceType == SURFACE_FLESHVULNERABLE)
			{
				ActualDamage *= 4.0f;
			}

			UGameplayStatics::ApplyPointDamage(HitActor, ActualDamage, ShotDirection, Hit, MyOwner->GetInstigatorController(), this, DamageType);
			//PointDamage�� ���������� �ۿ��ϴ� ���, ��ü�� �浹(Impulse)�� �����Ѵ�. ��) ������ ������Ʈ�� �i����, ������ �浹�� ����ȴ�.
			//���������� �ùķ��̼� �ϰų� ragdoll
			//HitActor, 20������, �߻����, Hit����ü, �߻��� ����, ��������(?),
			//TSubclassOf<UDamageType>�� ȭ�� �������� �� ������ ���� ����

			//Hit.PhysMaterial�� ���� ��ü �����ͷ�, �޸𸮿� �����Ǵ°��� ������� �ʴ´�.
			//���� ���, �� ����ü�� �� ������ ��� �����͸� ����ϴ� ������ ������ ����ü�̸� ������ �޸𸮿��� �̸� ������ �� �ִ�.
			// �������� ��ƴ�. ���� ��ü ������ TWeakObjectPointer( TWeakObjectPtr )
			// ��) ���� ���������� ����ϴ� ��� ��찡 ������ �� ������, ������ �����ϰ� ���� �ʽ��ϴ�.

			PlayImpactEffect(SurfaceType, Hit.ImpactPoint);
			TracerEndPoint = Hit.ImpactPoint;
		}

		if (DebugWeaponDrawing > 0)
		{
			DrawDebugLine(GetWorld(), EyeLocation, TraceEnd, FColor::White, false, 1.0f, 0, 1.0f);
		}

		PlayFireEffects(TracerEndPoint);

		if (Role == ROLE_Authority)
		{
			HitScanTrace.TraceTo = TracerEndPoint;
			//HitScanTrace.TraceFrom == Muzzle ��ġ? ������ġ?
			HitScanTrace.Surfacetype = SurfaceType;
		}

		LastFireTime = GetWorld()->TimeSeconds;
	}
}

void ASWeapon::OnRep_HitScanTrace()
{
	// Play cosmetic FX
	PlayFireEffects(HitScanTrace.TraceTo);

	PlayImpactEffect(HitScanTrace.Surfacetype, HitScanTrace.TraceTo);
}


void ASWeapon::PlayImpactEffect(EPhysicalSurface SurfaceType, FVector ImpactPoint)
{

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
		FVector MuzzleLocation = MeshComp->GetSocketLocation(MuzzleSocketName);

		FVector ShotDirection = ImpactPoint - MuzzleLocation;
		ShotDirection.Normalize();

		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SelectedEffect, ImpactPoint,ShotDirection.Rotation());
		//ImpactPoint ������, ImpactNormal( ������ �˱� ���� ��� )
	}
}

void ASWeapon::ServerFire_Implementation() //���� ����� ����� ����, Server�� ���λ�� ����ϴ°��� �߿� ��Ģ�̴�.
{
	Fire();
}

bool ASWeapon::ServerFire_Validate() //�ڵ��� ��ȿ���� �˻��ϴ� ���, �ش� �˻翡�� false�� ��ȯ�Ǹ� �ش� Ŭ���̾�Ʈ�� �������� ������ ��������.
{
	return true;
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

		UParticleSystemComponent* TracerComp = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), TracerEffect, MuzzleLocation);//��ƼŬ ����, ������ġ MuzzleLocation
		if (TracerComp)
		{
			TracerComp->SetVectorParameter(TracerTargetName, TracerEnd);
			//Target �Ķ���Ϳ� TracerEndPoint( ��ƼŬ �ý����� ������ )�� �Ѱ��ش�.
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


/*
And the other file, it's already done for us by the annual header tool by by EPIK that takes care of
this through the day care of certain certain functions for us, which includes this one.
This just gets auto generated for us.
������Ͽ� ��Ʈ��ũ�� ���������� �ʾƵ� �ȴ�. ������ �ʿ䰡 ���� Ư���� ��� �� �ϳ��̴�.
*/
void ASWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const //������ �����ϰ� � ��������� �������� ���ϴ� �Լ�?
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ASWeapon, HitScanTrace, COND_SkipOwner); //�� ���� ���� ��ũ�� // ������ ��ġ�� ���� ���� ������ �⺻ ���]
	// DOREPLIFETIME ��ũ�δ� ������ ����� ��� ���� Ŭ���̾�Ʈ�� ���� ���ø������͸��Ѵ�.
	// DOREPLIFETIME_CONDITION(,,COND_SkipOwner)�� ������ Ŭ���̾�Ʈ�� �̰��� �����ϰ� ���� ���� �� ����Ѵ�. ��) �������״� ���ø�����Ʈ ����
	// ��) ���⸦ �߻��� ����� ������ ��� ������� ���ĵȴ�.
}