#include "Gimmick/CHA/WavePath.h"

#include "Components/SceneComponent.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "UObject/ConstructorHelpers.h"

AWavePath::AWavePath()
{
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

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
	UpdateBalls(DeltaTime);
}

float AWavePath::GetNow() const
{
	return GetWorld() ? GetWorld()->GetTimeSeconds() : 0.f;
}

FVector AWavePath::GetMoveDirWorld() const
{
	FVector Dir = bUseRightVector ? GetActorRightVector() : GetActorForwardVector();
	Dir = Dir.GetSafeNormal();
	if (bInvertDirection) Dir *= -1.f;
	return Dir;
}

float AWavePath::CalcCollisionRadius() const
{
	const float BaseRadius = 50.0f;
	return BaseRadius * FMath::Max(0.01f, BallScale) * FMath::Clamp(CollisionScale, 0.1f, 1.0f);
}

void AWavePath::RefreshIfNeeded()
{
	const bool bLayoutChanged =
		(CachedBallCount != BallCount) ||
		(!FMath::IsNearlyEqual(CachedBallSpacing, BallSpacing)) ||
		(!FMath::IsNearlyEqual(CachedBallScale, BallScale)) ||
		(!FMath::IsNearlyEqual(CachedBallZOffset, BallZOffset)) ||
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
		CachedBallZOffset = BallZOffset;
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
		if (Visual) Visual->DestroyComponent();
	}
	for (USphereComponent* Body : BallBodies)
	{
		if (!Body) continue;
		Body->OnComponentHit.Clear();
		Body->DestroyComponent();
	}

	BallBodies.Empty();
	VisualMeshes.Empty();
	BaseLocalLocations.Empty();
	BallDirSign.Empty();
	BallAmpScale.Empty();
}

void AWavePath::BuildBalls()
{
	if (!BallMesh) return;

	BallBodies.SetNum(BallCount);
	VisualMeshes.SetNum(BallCount);
	BaseLocalLocations.SetNum(BallCount);

	const float Radius = CalcCollisionRadius();
	float CurrentY = 0.f;

	for (int32 i = 0; i < BallCount; ++i)
	{
		if (i > 0)
		{
			CurrentY += BallSpacing;

			if (GapEveryN > 0 && (i % GapEveryN) == 0)
			{
				CurrentY += ExtraGapSize;
			}
		}

		const FVector BaseLocal(0.f, CurrentY, BallZOffset);
		BaseLocalLocations[i] = BaseLocal;

		if (bSkipEveryNBall && SkipEveryN > 1 && ((i + 1) % SkipEveryN == 0))
		{
			BallBodies[i] = nullptr;
			VisualMeshes[i] = nullptr;
			continue;
		}

		// ===== 물리 본체 =====
		const FName BodyName = *FString::Printf(TEXT("BallBody_%02d"), i);
		USphereComponent* Body = NewObject<USphereComponent>(this, BodyName);
		if (!Body)
		{
			BallBodies[i] = nullptr;
			VisualMeshes[i] = nullptr;
			continue;
		}

		AddInstanceComponent(Body);
		Body->SetupAttachment(Root);
		Body->SetRelativeLocation(BaseLocal);

		Body->InitSphereRadius(Radius);

		// ✅ 박치기(Block Hit)
		Body->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		Body->SetCollisionObjectType(ECC_WorldDynamic);
		Body->SetCollisionResponseToAllChannels(ECR_Block);
		Body->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);

		Body->SetNotifyRigidBodyCollision(true);
		Body->OnComponentHit.AddDynamic(this, &AWavePath::OnBallHit);

		Body->SetSimulatePhysics(true);
		Body->SetEnableGravity(false);

		Body->SetLinearDamping(LinearDamping);
		Body->SetAngularDamping(AngularDamping);

		// 회전 잠금(원하면 풀어도 됨)
		Body->BodyInstance.bLockXRotation = true;
		Body->BodyInstance.bLockYRotation = true;
		Body->BodyInstance.bLockZRotation = true;

		Body->SetMassOverrideInKg(NAME_None, MassKg, true);

		// ===== 비주얼 =====
		const FName VisualName = *FString::Printf(TEXT("Visual_%02d"), i);
		UStaticMeshComponent* Visual = NewObject<UStaticMeshComponent>(this, VisualName);
		if (!Visual)
		{
			Body->OnComponentHit.Clear();
			Body->DestroyComponent();
			BallBodies[i] = nullptr;
			VisualMeshes[i] = nullptr;
			continue;
		}

		AddInstanceComponent(Visual);
		Visual->SetupAttachment(Body);
		Visual->SetStaticMesh(BallMesh);
		Visual->SetRelativeLocation(FVector::ZeroVector);
		Visual->SetRelativeScale3D(FVector(BallScale));
		Visual->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		if (UWorld* W = GetWorld())
		{
			Body->RegisterComponentWithWorld(W);
			Visual->RegisterComponentWithWorld(W);
		}
		else
		{
			Body->RegisterComponent();
			Visual->RegisterComponent();
		}

		BallBodies[i] = Body;
		VisualMeshes[i] = Visual;
	}
}

void AWavePath::InitPerBallMotionParams()
{
	BallDirSign.SetNum(BallCount);
	BallAmpScale.SetNum(BallCount);

	FRandomStream Rng(RandomSeed);
	const float J = FMath::Clamp(AmplitudeJitter, 0.f, 1.f);

	for (int32 i = 0; i < BallCount; ++i)
	{
		BallDirSign[i] = bAlternateDirectionPerBall ? ((i % 2 == 0) ? 1.f : -1.f) : 1.f;
		BallAmpScale[i] = 1.f + Rng.FRandRange(-J, J);
	}
}

void AWavePath::UpdateBalls(float DeltaTime)
{
	if (BallBodies.Num() == 0) return;
	if (Period <= 0.f) return;
	if (DeltaTime <= KINDA_SMALL_NUMBER) return;

	const float Now = GetNow();
	const float Omega = 2.f * PI / Period;
	const FVector DirW = GetMoveDirWorld();
	const float Easy = FMath::Clamp(GlobalEasyScale, 0.f, 1.f);

	for (int32 i = 0; i < BallBodies.Num(); ++i)
	{
		USphereComponent* Body = BallBodies[i];
		if (!Body) continue;

		const float Phase = Omega * (Now - i * PhaseDelay);
		const float Swing = FMath::Sin(Phase);

		const float Sign = BallDirSign.IsValidIndex(i) ? BallDirSign[i] : 1.f;
		const float AmpScale = BallAmpScale.IsValidIndex(i) ? BallAmpScale[i] : 1.f;

		const float Amp = PushAmplitude * Easy * AmpScale;
		const float Offset = Sign * Amp * Swing;

		const FVector BaseWorld = GetActorTransform().TransformPosition(BaseLocalLocations[i]);
		const FVector TargetWorld = BaseWorld + (DirW * Offset);

		const FVector CurrentWorld = Body->GetComponentLocation();
		FVector DesiredVel = (TargetWorld - CurrentWorld) / DeltaTime;

		const float Speed = DesiredVel.Size();
		if (Speed > MaxDriveSpeed)
		{
			DesiredVel = DesiredVel.GetSafeNormal() * MaxDriveSpeed;
		}

		// ✅ 물리 속도로 구슬을 끌어감(박치기 가능)
		Body->SetPhysicsLinearVelocity(DesiredVel, false);
	}
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

void AWavePath::TemporarilyIgnorePawn(USphereComponent* BallBody) const
{
	if (!BallBody) return;
	if (!GetWorld()) return;
	if (IgnorePawnAfterHitTime <= 0.f) return;

	// 히트 직후 잠깐 Pawn만 Ignore로 바꿔서 “붙어서 버벅” 제거
	BallBody->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);

	TWeakObjectPtr<USphereComponent> WeakBall = BallBody;

	FTimerHandle Tmp;
	GetWorld()->GetTimerManager().SetTimer(
		Tmp,
		FTimerDelegate::CreateLambda([WeakBall]()
			{
				if (!WeakBall.IsValid()) return;
				WeakBall->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
			}),
		IgnorePawnAfterHitTime,
		false
	);
}

void AWavePath::OnBallHit(
	UPrimitiveComponent* HitComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	FVector NormalImpulse,
	const FHitResult& Hit
)
{
	if (!OtherActor || OtherActor == this) return;

	ACharacter* Ch = Cast<ACharacter>(OtherActor);
	if (!Ch) return;

	if (!CanHit(OtherActor)) return;

	// FVector로 확정(타입 꼬임 방지)
	FVector FromBallWorld = HitComp ? HitComp->GetComponentLocation() : FVector(Hit.ImpactPoint);

	ApplyKnockbackToCharacter(OtherActor, FromBallWorld);

	// ✅ 버벅임 제거
	TemporarilyIgnorePawn(Cast<USphereComponent>(HitComp));

	MarkHit(OtherActor);
}

void AWavePath::ApplyKnockbackToCharacter(AActor* OtherActor, const FVector& FromBallWorld) const
{
	ACharacter* Ch = Cast<ACharacter>(OtherActor);
	if (!Ch) return;

	UCharacterMovementComponent* Move = Ch->GetCharacterMovement();
	if (!Move) return;

	const FVector CharLoc = Ch->GetActorLocation();

	// 공 -> 캐릭터 수평 방향
	FVector Away = (CharLoc - FromBallWorld);
	Away.Z = 0.f;
	Away = Away.GetSafeNormal();
	if (Away.IsNearlyZero())
	{
		Away = GetMoveDirWorld();
	}

	// ✅ “박치기하면 훅 날아감”
	FVector LaunchVel = (Away * KnockbackStrength);
	LaunchVel += (GetMoveDirWorld() * ForwardBoost);
	LaunchVel += FVector(0, 0, -DownwardForce);

	Move->SetMovementMode(MOVE_Falling);

	// 덮어쓰기 true => 즉시 튕김
	Ch->LaunchCharacter(LaunchVel, true, true);
}
