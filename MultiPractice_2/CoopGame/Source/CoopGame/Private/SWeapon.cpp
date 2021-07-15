// Fill out your copyright notice in the Description page of Project Settings.

#include "SWeapon.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Components//SkeletalMeshComponent.h"
#include "Particles/ParticleSystemComponent.h"


// Sets default values
ASWeapon::ASWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComp"));
	RootComponent = MeshComp;

	MuzzleSocketName = "MuzzleSocket";
	TracerTargetName = "Target";
}

// Called when the game starts or when spawned
void ASWeapon::BeginPlay()
{
	Super::BeginPlay();
	
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

		DrawDebugLine(GetWorld(), EyeLocation, TraceEnd, FColor::White, false, 1.0f, 0, 1.0f);

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
				TracerComp->SetVectorParameter(TracerTargetName, TracerEndPoint);
				//Target �Ķ���Ϳ� TracerEndPoint( ��ƼŬ �ý����� ������ )�� �Ѱ��ش�.
			}
		}
	}
}

// Called every frame
void ASWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

