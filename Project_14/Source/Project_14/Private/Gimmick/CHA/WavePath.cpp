#include "Gimmick/CHA/WavePath.h"

#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Character.h"
#include "Engine/World.h"
#include "UObject/ConstructorHelpers.h"

AWavePath::AWavePath()
{
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	// ✅ ConstructorHelpers는 생성자에서만!
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

	RebuildBallsIfNeeded();
}

void AWavePath::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 에디터에서 값 바꾸거나 BallCount 변경했을 때 자동 재구성
	RebuildBallsIfNeeded();
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

void AWavePath::RebuildBallsIfNeeded()
{
	const bool bNeedRebuild =
		(Balls.Num() != BallCount) ||
		(BaseLocalLocations.Num() != BallCount) ||
		(BallDirSign.Num() != BallCount) ||
		(BallAmpScale.Num() != BallCount);

	if (!bNeedRebuild)
	{
		// 메쉬가 바뀐 경우도 반영(옵션)
		for (UStaticMeshComponent* Ball : Balls)
		{
			if (Ball && BallMesh && Ball->GetStaticMesh() != BallMesh)
			{
				Ball->SetStaticMesh(BallMesh);
			}
		}
		return;
	}

	DestroyBalls();
	BuildBalls();
	InitPerBallMotionParams();
}

void AWavePath::DestroyBalls()
{
	for (UStaticMeshComponent* Ball : Balls)
	{
		if (!Ball) continue;

		Ball->OnComponentBeginOverlap.Clear();
		Ball->DestroyComponent();
	}

	Balls.Empty();
	BaseLocalLocations.Empty();
	BallDirSign.Empty();
	BallAmpScale.Empty();
}

void AWavePath::BuildBalls()
{
	if (!BallMesh)
	{
		// 메쉬가 없으면 그냥 아무것도 만들지 않음(크래시 방지)
		return;
	}

	Balls.Reserve(BallCount);
	BaseLocalLocations.Reserve(BallCount);

	for (int32 i = 0; i < BallCount; ++i)
	{
		const FName CompName = *FString::Printf(TEXT("Ball_%02d"), i);

		UStaticMeshComponent* Ball = NewObject<UStaticMeshComponent>(this, CompName);
		if (!Ball) continue;

		AddInstanceComponent(Ball);
		Ball->SetupAttachment(Root);

		Ball->SetStaticMesh(BallMesh);
		Ball->SetRelativeScale3D(FVector(BallScale));

		// 기본 위치: 옆으로 일렬(로컬 Y)
		const FVector BaseLocal(0.0f, i * BallSpacing, 0.0f);
		Ball->SetRelativeLocation(BaseLocal);

		// 물리 OFF (연출/판정은 스크립트)
		Ball->SetSimulatePhysics(false);
		Ball->SetEnableGravity(false);

		// Pawn만 Overlap (맞으면 밀리기)
		Ball->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		Ball->SetCollisionObjectType(ECC_WorldDynamic);
		Ball->SetCollisionResponseToAllChannels(ECR_Ignore);
		Ball->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
		Ball->SetGenerateOverlapEvents(true);

		Ball->OnComponentBeginOverlap.AddDynamic(this, &AWavePath::OnBallBeginOverlap);

		// 월드 등록
		if (UWorld* W = GetWorld())
		{
			Ball->RegisterComponentWithWorld(W);
		}
		else
		{
			Ball->RegisterComponent();
		}

		Balls.Add(Ball);
		BaseLocalLocations.Add(BaseLocal);
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
		// 방향: 번갈아 좌/우 (원하면 false로 두고 모두 같은 방향도 가능)
		const float Sign = (bAlternateDirectionPerBall ? ((i % 2 == 0) ? 1.0f : -1.0f) : 1.0f);
		BallDirSign[i] = Sign;

		// 진폭: ±J만큼 랜덤(패턴은 시드로 고정)
		const float Rand01 = Rng.FRandRange(-J, J); // -J..+J
		BallAmpScale[i] = 1.0f + Rand01;          // 0.75~1.25 같은 느낌
	}
}

void AWavePath::UpdateBalls()
{
	if (Balls.Num() == 0) return;
	if (Period <= 0.0f) return;

	const float Now = GetNow();
	const float Omega = 2.0f * PI / Period;
	const FVector Dir = GetMoveDir();

	for (int32 i = 0; i < Balls.Num(); ++i)
	{
		UStaticMeshComponent* Ball = Balls[i];
		if (!Ball) continue;

		// “따라락”을 줄이면서도 흐름은 남기기 위해 약한 위상차만 적용
		const float Phase = Omega * (Now - i * PhaseDelay);
		const float LocalSwing = FMath::Sin(Phase); // -1..+1 (앞/뒤 왕복)

		const float Sign = BallDirSign.IsValidIndex(i) ? BallDirSign[i] : 1.0f;
		const float AmpScale = BallAmpScale.IsValidIndex(i) ? BallAmpScale[i] : 1.0f;

		// ✅ 전체 난이도 완화 + 구슬별 랜덤 진폭
		const float Amp = PushAmplitude * FMath::Clamp(GlobalEasyScale, 0.0f, 1.0f) * AmpScale;

		// ✅ 어떤 구슬은 +, 어떤 구슬은 - 로 움직이며, 범위는 Amp만큼
		const float Offset = Sign * Amp * LocalSwing;

		const FVector NewLocal = BaseLocalLocations[i] + (Dir * Offset);
		Ball->SetRelativeLocation(NewLocal, false, nullptr, ETeleportType::TeleportPhysics);
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

	// 구슬이 움직이는 축(Dir) 방향으로 튕김
	const FVector Dir = GetMoveDir();
	const FVector LaunchVel = (Dir * KnockbackStrength) + FVector(0, 0, UpwardBoost);

	Ch->LaunchCharacter(LaunchVel, true, true);
}
