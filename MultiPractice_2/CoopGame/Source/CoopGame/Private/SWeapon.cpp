// Fill out your copyright notice in the Description page of Project Settings.

#include "SWeapon.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ASWeapon::ASWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComp"));
	RootComponent = MeshComp;
}

// Called when the game starts or when spawned
void ASWeapon::BeginPlay()
{
	Super::BeginPlay();
	
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
		//And now what this does, it will trace against each individual triangle off the mesh that we're hitting.
		//TraceComplex는 비용이 비싸지만 우리가 무엇을 쳤는지에 대한 정확한 결과를 제공한다.
		//Trace가 false이면 경계 상자와 같이 단순 수집만 수행 합니다.
		//If this is a false, it will only do a simple collection that it could be a simple like bounding box

		FHitResult Hit; // 무엇을 쳤는지, 얼마나 멀리 떨어져 있는지 등의 여러 데이터가 담겨있는 구조체
		if (GetWorld()->LineTraceSingleByChannel(Hit, EyeLocation, TraceEnd, ECC_Visibility, QueryParams))
		{
			//Hit 구조체, EyeLocation(출발 위치), TraceEnd(도착 위치), 채널( 겹침, 차단 등등 체크하는 채널), 충돌쿼리
			//Blocking hit! Process damage

			AActor* HitActor = Hit.GetActor();

			UGameplayStatics::ApplyPointDamage(HitActor, 20.0f, ShotDirection, Hit, MyOwner->GetInstigatorController(), this, DamageType);
			//HitActor, 20데미지, 발사방향, Hit구조체, 발사자 정보, 무기정보(?),
			//TSubclassOf<UDamageType>로 화염 데미지나 독 데미지 구현 가능
		}

		DrawDebugLine(GetWorld(), EyeLocation, TraceEnd, FColor::White, false, 1.0f, 0, 1.0f);
	}
}

// Called every frame
void ASWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

