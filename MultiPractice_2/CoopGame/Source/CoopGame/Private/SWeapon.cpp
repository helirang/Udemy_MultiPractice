// Fill out your copyright notice in the Description page of Project Settings.

#include "SWeapon.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Components//SkeletalMeshComponent.h"
#include "Particles/ParticleSystemComponent.h"

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
}

void ASWeapon::Fire()
{
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
		//And now what this does, it will trace against each individual triangle off the mesh that we're hitting.
		//TraceComplex�� ����� ������� �츮�� ������ �ƴ����� ���� ��Ȯ�� ����� �����Ѵ�.
		//Trace�� false�̸� ��� ���ڿ� ���� �ܼ� ������ ���� �մϴ�.
		//If this is a false, it will only do a simple collection that it could be a simple like bounding box

		//Particle "Target" parameter
		FVector TracerEndPoint = TraceEnd;


		FHitResult Hit; // ������ �ƴ���, �󸶳� �ָ� ������ �ִ��� ���� ���� �����Ͱ� ����ִ� ����ü
		if (GetWorld()->LineTraceSingleByChannel(Hit, EyeLocation, TraceEnd, ECC_Visibility, QueryParams))
		{
			//Hit ����ü, EyeLocation(��� ��ġ), TraceEnd(���� ��ġ), ä��( ��ħ, ���� ��� üũ�ϴ� ä��), �浹����
			//Blocking hit! Process damage

			AActor* HitActor = Hit.GetActor();

			UGameplayStatics::ApplyPointDamage(HitActor, 20.0f, ShotDirection, Hit, MyOwner->GetInstigatorController(), this, DamageType);
			//HitActor, 20������, �߻����, Hit����ü, �߻��� ����, ��������(?),
			//TSubclassOf<UDamageType>�� ȭ�� �������� �� ������ ���� ����

			if (ImpactEffect)
			{
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactEffect, Hit.ImpactPoint, Hit.ImpactNormal.Rotation());
				//ImpactPoint ������, ImpactNormal( ������ �˱� ���� ��� )
			}

			TracerEndPoint = Hit.ImpactPoint;
		}

		if (DebugWeaponDrawing > 0)
		{
			DrawDebugLine(GetWorld(), EyeLocation, TraceEnd, FColor::White, false, 1.0f, 0, 1.0f);
		}

		PlayFireEffects(TracerEndPoint);

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

		UParticleSystemComponent* TracerComp = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), TracerEffect, MuzzleLocation);//��ƼŬ ����, ������ġ MuzzleLocation
		if (TracerComp)
		{
			TracerComp->SetVectorParameter(TracerTargetName, TracerEnd);
			//Target �Ķ���Ϳ� TracerEndPoint( ��ƼŬ �ý����� ������ )�� �Ѱ��ش�.
		}
	}
}
