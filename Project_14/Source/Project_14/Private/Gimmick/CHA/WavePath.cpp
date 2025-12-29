#include "Gimmick/CHA/WavePath.h"

#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Character.h"
#include "Engine/World.h"
#include "UObject/ConstructorHelpers.h"

AWavePath::AWavePath()
{
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	// ✅ ConstructorHelpers는 생성자에서만
	if (!BallMesh)
	{
		static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMesh(TEXT("/Engine/BasicShapes/Sphere.Sphere"));
		if (SphereMesh.Succeeded())
		{
			BallMesh = SphereMesh.Object;
		}
	}
}

void AWavePath::BeginPlay()
{
	Super::BeginPlay();
	RefreshIfNeeded();
}

void AWavePath::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	RefreshIfNeeded();
	UpdateBalls();
}

float AWavePath::GetNow() const
{
	return GetWorld() ? GetWorld()->GetTimeSeconds() : 0.0f;
}

FVector AWavePath::GetMoveDir() const
{
	FVector Dir = bUseRightVector ? GetActorRightVector() : GetActorForwardVector();
	Dir = Dir.GetSafeNormal();
	if (bInvertDirection)
	{
		Dir *= -1.0f;
	}
	return Dir;
}

float AWavePath::CalcCollisionRadius() const
{
	// UE 기본 Sphere는 반지름 50(Scale 1.0)로 가정
	const float BaseRadius = 50.0f;
	return BaseRadius * FMath::Max(0.01f, BallScale) * FMath::Clamp(CollisionScale, 0.1f, 1.0f);
}

void AWavePath::RefreshIfNeeded()
{
	const bool bLayoutChanged =
		(CachedBallCount != BallCount) ||
		(!FMath::IsNearlyEqual(CachedBallSpacing, BallSpacing)) ||
		(!FMath::IsNearlyEqual(CachedBallScale, BallScale)) ||
		(!FMath::IsNearlyEqual(CachedCollisionScale, CollisionScale)) ||
		(CachedBallMesh != BallMesh) ||
		(CachedGapEveryN != GapEveryN) ||
		(!FMath::IsNearlyEqual(CachedExtraGapSize, ExtraGapSize)) ||
		(CachedSkipEnabled != bSkipEveryNBall) ||
		(CachedSkipEveryN != SkipEveryN);

	const bool bMotionParamsChanged =
		(CachedAltDir != bAlternateDirectionPerBall) ||
		(!FMath::IsNearlyEqual(CachedJitter, AmplitudeJitter)) ||
		(CachedSeed != RandomSeed);

	if (bLayoutChanged)
	{
		DestroyBalls();
		BuildBalls();
		InitPerBallMotionParams();

		CachedBallCount = BallCount;
		CachedBallSpacing = BallSpacing;
		CachedBallScale = BallScale;
		CachedCollisionScale = CollisionScale;
		CachedBallMesh = BallMesh;

		CachedGapEveryN = GapEveryN;
		CachedExtraGapSize = ExtraGapSize;
		CachedSkipEnabled = bSkipEveryNBall;
		CachedSkipEveryN = SkipEveryN;

		CachedAltDir = bAlternateDirectionPerBall;
		CachedJitter = AmplitudeJitter;
		CachedSeed = RandomSeed;

		return;
	}

	if (bMotionParamsChanged)
	{
		InitPerBallMotionParams();
		CachedAltDir = bAlternateDirectionPerBall;
		CachedJitter = AmplitudeJitter;
		CachedSeed = RandomSeed;
	}
}

void AWavePath::DestroyBalls()
{
	for (UStaticMeshComponent* Visual : VisualMeshes)
	{
		if (!Visual) continue;
		Visual->DestroyComponent();
	}
	for (USphereComponent* Sphere : HitSpheres)
	{
		if (!Sphere) continue;
		Sphere->OnComponentBeginOverlap.Clear();
		Sphere->DestroyComponent();
	}

	HitSpheres.Empty();
	VisualMeshes.Empty();
	BaseLocalLocations.Empty();
	BallDirSign.Empty();
	BallAmpScale.Empty();
}

void AWavePath::BuildBalls()
{
	// 메쉬 없으면 생성 안 함(크래시 방지)
	if (!BallMesh) return;

	HitSpheres.SetNum(BallCount);
	VisualMeshes.SetNum(BallCount);
	BaseLocalLocations.SetNum(BallCount);

	const float Radius = CalcCollisionRadius();

	float CurrentY = 0.0f;

	for (int32 i = 0; i < BallCount; ++i)
	{
		// 간격 누적
		if (i > 0)
		{
			CurrentY += BallSpacing;

			// N개마다 큰 간격
			if (GapEveryN > 0 && (i % GapEveryN) == 0)
			{
				CurrentY += ExtraGapSize;
			}
		}

		const FVector BaseLocal(0.0f, CurrentY, 0.0f);
		BaseLocalLocations[i] = BaseLocal;

		// 빈 칸
		if (bSkipEveryNBall && SkipEveryN > 1 && ((i + 1) % SkipEveryN == 0))
		{
			HitSpheres[i] = nullptr;
			VisualMeshes[i] = nullptr;
			continue;
		}

		// ====== 판정용 SphereComponent ======
		const FName SphereName = *FString::Printf(TEXT("HitSphere_%02d"), i);
		USphereComponent* HitSphere = NewObject<USphereComponent>(this, SphereName);
		if (!HitSphere)
		{
			HitSpheres[i] = nullptr;
			VisualMeshes[i] = nullptr;
			continue;
		}

		AddInstanceComponent(HitSphere);
		HitSphere->SetupAttachment(Root);
		HitSphere->SetRelativeLocation(BaseLocal);

		// ✅ 충돌 반경(판정만 축소)
		HitSphere->InitSphereRadius(Radius);
		HitSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		HitSphere->SetCollisionObjectType(ECC_WorldDynamic);
		HitSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
		HitSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
		HitSphere->SetGenerateOverlapEvents(true);

		HitSphere->OnComponentBeginOverlap.AddDynamic(this, &AWavePath::OnBallBeginOverlap);

		// ====== 비주얼용 StaticMeshComponent (충돌 OFF) ======
		const FName VisualName = *FString::Printf(TEXT("Visual_%02d"), i);
		UStaticMeshComponent* Visual = NewObject<UStaticMeshComponent>(this, VisualName);
		if (!Visual)
		{
			HitSphere->OnComponentBeginOverlap.Clear();
			HitSphere->DestroyComponent();

			HitSpheres[i] = nullptr;
			VisualMeshes[i] = nullptr;
			continue;
		}

		AddInstanceComponent(Visual);
		Visual->SetupAttachment(HitSphere); // 판정 구에 붙여서 같이 움직이게
		Visual->SetStaticMesh(BallMesh);
		Visual->SetRelativeLocation(FVector::ZeroVector);
		Visual->SetRelativeScale3D(FVector(BallScale));

		// ✅ 비주얼은 충돌 끔(판정은 HitSphere만 담당)
		Visual->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		// 등록
		if (UWorld* W = GetWorld())
		{
			HitSphere->RegisterComponentWithWorld(W);
			Visual->RegisterComponentWithWorld(W);
		}
		else
		{
			HitSphere->RegisterComponent();
			Visual->RegisterComponent();
		}

		HitSpheres[i] = HitSphere;
		VisualMeshes[i] = Visual;
	}
}

void AWavePath::InitPerBallMotionParams()
{
	BallDirSign.SetNum(BallCount);
	BallAmpScale.SetNum(BallCount);

	FRandomStream Rng(RandomSeed);
	const float J = FMath::Clamp(AmplitudeJitter, 0.0f, 1.0f);

	for (int32 i = 0; i < BallCount; ++i)
	{
		BallDirSign[i] = bAlternateDirectionPerBall ? ((i % 2 == 0) ? 1.0f : -1.0f) : 1.0f;
		const float Rand01 = Rng.FRandRange(-J, J);
		BallAmpScale[i] = 1.0f + Rand01;
	}
}

void AWavePath::UpdateBalls()
{
	if (HitSpheres.Num() == 0) return;
	if (Period <= 0.0f) return;

	const float Now = GetNow();
	const float Omega = 2.0f * PI / Period;
	const FVector Dir = GetMoveDir();
	const float Easy = FMath::Clamp(GlobalEasyScale, 0.0f, 1.0f);

	for (int32 i = 0; i < HitSpheres.Num(); ++i)
	{
		USphereComponent* HitSphere = HitSpheres[i];
		if (!HitSphere) continue;

		const float Phase = Omega * (Now - i * PhaseDelay);
		const float LocalSwing = FMath::Sin(Phase); // -1..+1

		const float Sign = BallDirSign.IsValidIndex(i) ? BallDirSign[i] : 1.0f;
		const float AmpScale = BallAmpScale.IsValidIndex(i) ? BallAmpScale[i] : 1.0f;

		const float Amp = PushAmplitude * Easy * AmpScale;
		const float Offset = Sign * Amp * LocalSwing;

		const FVector NewLocal = BaseLocalLocations[i] + (Dir * Offset);
		HitSphere->SetRelativeLocation(NewLocal, false, nullptr, ETeleportType::TeleportPhysics);
	}
}

void AWavePath::OnBallBeginOverlap(
	UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult
)
{
	if (!OtherActor || OtherActor == this) return;

	ACharacter* Ch = Cast<ACharacter>(OtherActor);
	if (!Ch) return;

	if (!CanHit(OtherActor)) return;

	ApplyKnockbackToCharacter(OtherActor);
	MarkHit(OtherActor);
}

bool AWavePath::CanHit(AActor* OtherActor) const
{
	if (!OtherActor) return false;

	const float Now = GetNow();
	if (const float* Last = LastHitTime.Find(OtherActor))
	{
		return (Now - *Last) >= HitCooldown;
	}
	return true;
}

void AWavePath::MarkHit(AActor* OtherActor)
{
	if (!OtherActor) return;
	LastHitTime.Add(OtherActor, GetNow());
}

void AWavePath::ApplyKnockbackToCharacter(AActor* OtherActor) const
{
	ACharacter* Ch = Cast<ACharacter>(OtherActor);
	if (!Ch) return;

	const FVector Dir = GetMoveDir();
	const FVector LaunchVel = (Dir * KnockbackStrength) + FVector(0, 0, UpwardBoost);

	Ch->LaunchCharacter(LaunchVel, true, true);
}
