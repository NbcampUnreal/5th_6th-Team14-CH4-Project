#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WavePath.generated.h"

// forward declarations
class USceneComponent;
class USphereComponent;
class UStaticMeshComponent;
class UStaticMesh;
class UPrimitiveComponent;

UCLASS()
class PROJECT_14_API AWavePath : public AActor
{
	GENERATED_BODY()

public:
	AWavePath();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	// =========================
	//  Setup
	// =========================
	UPROPERTY(EditAnywhere, Category = "Cradle|Setup", meta = (ClampMin = "1", ClampMax = "50"))
	int32 BallCount = 9;

	UPROPERTY(EditAnywhere, Category = "Cradle|Setup", meta = (ClampMin = "0.0"))
	float BallSpacing = 240.0f;

	// ✅ 높이 내리기: -250, -320...로 더 내릴 수 있음
	UPROPERTY(EditAnywhere, Category = "Cradle|Setup")
	float BallZOffset = -200.0f;

	UPROPERTY(EditAnywhere, Category = "Cradle|Setup", meta = (ClampMin = "0.01"))
	float BallScale = 1.0f;

	// 비주얼 메쉬(비워두면 엔진 Sphere)
	UPROPERTY(EditAnywhere, Category = "Cradle|Setup")
	UStaticMesh* BallMesh = nullptr;

	UPROPERTY(EditAnywhere, Category = "Cradle|Setup", meta = (ClampMin = "0"))
	int32 GapEveryN = 0;

	UPROPERTY(EditAnywhere, Category = "Cradle|Setup", meta = (ClampMin = "0.0"))
	float ExtraGapSize = 0.0f;

	UPROPERTY(EditAnywhere, Category = "Cradle|Setup")
	bool bSkipEveryNBall = false;

	UPROPERTY(EditAnywhere, Category = "Cradle|Setup", meta = (ClampMin = "2"))
	int32 SkipEveryN = 4;

	// =========================
	//  Motion
	// =========================
	UPROPERTY(EditAnywhere, Category = "Cradle|Motion", meta = (ClampMin = "0.0"))
	float PushAmplitude = 360.0f;

	UPROPERTY(EditAnywhere, Category = "Cradle|Motion", meta = (ClampMin = "0.05"))
	float Period = 2.6f;

	UPROPERTY(EditAnywhere, Category = "Cradle|Motion", meta = (ClampMin = "0.0"))
	float PhaseDelay = 0.10f;

	UPROPERTY(EditAnywhere, Category = "Cradle|Motion", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float GlobalEasyScale = 0.75f;

	// true=좌우(Right), false=앞뒤(Forward)
	UPROPERTY(EditAnywhere, Category = "Cradle|Motion")
	bool bUseRightVector = false;

	UPROPERTY(EditAnywhere, Category = "Cradle|Motion")
	bool bInvertDirection = false;

	// 공마다 방향 번갈아(+1/-1)
	UPROPERTY(EditAnywhere, Category = "Cradle|Motion")
	bool bAlternateDirectionPerBall = true;

	// 공마다 진폭 랜덤(0~1)
	UPROPERTY(EditAnywhere, Category = "Cradle|Motion", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float AmplitudeJitter = 0.15f;

	UPROPERTY(EditAnywhere, Category = "Cradle|Motion")
	int32 RandomSeed = 12345;

	// 물리 공을 목표 위치로 끌어가는 최대 속도(너무 크면 튐/불안정)
	UPROPERTY(EditAnywhere, Category = "Cradle|Motion", meta = (ClampMin = "100.0"))
	float MaxDriveSpeed = 2200.0f;

	// =========================
	//  Hit / Push
	// =========================
	// ✅ 박치기 순간 확 날리는 힘
	UPROPERTY(EditAnywhere, Category = "Cradle|Hit", meta = (ClampMin = "0.0"))
	float KnockbackStrength = 4200.0f;

	// ✅ 공 진행방향으로 더 “펀치” 느낌
	UPROPERTY(EditAnywhere, Category = "Cradle|Hit", meta = (ClampMin = "0.0"))
	float ForwardBoost = 1400.0f;

	// ✅ 아래로 떨어뜨리기(낙사 유도)
	UPROPERTY(EditAnywhere, Category = "Cradle|Hit", meta = (ClampMin = "0.0"))
	float DownwardForce = 1600.0f;

	// 연타 방지
	UPROPERTY(EditAnywhere, Category = "Cradle|Hit", meta = (ClampMin = "0.0"))
	float HitCooldown = 0.25f;

	// ✅ 충돌 판정 크기만 줄이기 (기본 0.8)
	UPROPERTY(EditAnywhere, Category = "Cradle|Hit", meta = (ClampMin = "0.1", ClampMax = "1.0"))
	float CollisionScale = 0.80f;

	// ✅ 공이 무겁게 밀어내는 느낌
	UPROPERTY(EditAnywhere, Category = "Cradle|Hit", meta = (ClampMin = "1.0"))
	float MassKg = 250.0f;

	UPROPERTY(EditAnywhere, Category = "Cradle|Hit", meta = (ClampMin = "0.0"))
	float LinearDamping = 2.0f;

	UPROPERTY(EditAnywhere, Category = "Cradle|Hit", meta = (ClampMin = "0.0"))
	float AngularDamping = 4.0f;

	// ✅ 버벅임 제거용: 히트 후 잠깐 Pawn 충돌 무시
	UPROPERTY(EditAnywhere, Category = "Cradle|Hit", meta = (ClampMin = "0.0"))
	float IgnorePawnAfterHitTime = 0.08f;

private:
	UPROPERTY()
	USceneComponent* Root = nullptr;

	UPROPERTY()
	TArray<USphereComponent*> BallBodies;

	UPROPERTY()
	TArray<UStaticMeshComponent*> VisualMeshes;

	TArray<FVector> BaseLocalLocations;
	TArray<float> BallDirSign;
	TArray<float> BallAmpScale;

	TMap<TWeakObjectPtr<AActor>, float> LastHitTime;

	// 캐시(재빌드)
	int32 CachedBallCount = -1;
	float CachedBallSpacing = -1.f;
	float CachedBallScale = -1.f;
	float CachedBallZOffset = -999999.f;
	float CachedCollisionScale = -1.f;
	UStaticMesh* CachedBallMesh = nullptr;
	int32 CachedGapEveryN = -1;
	float CachedExtraGapSize = -1.f;
	bool CachedSkipEnabled = false;
	int32 CachedSkipEveryN = -1;
	bool CachedAltDir = false;
	float CachedJitter = -1.f;
	int32 CachedSeed = INT32_MIN;

	// 유틸
	float GetNow() const;
	FVector GetMoveDirWorld() const;
	float CalcCollisionRadius() const;

	// 라이프사이클
	void RefreshIfNeeded();
	void DestroyBalls();
	void BuildBalls();
	void InitPerBallMotionParams();
	void UpdateBalls(float DeltaTime);

	// 충돌 처리
	UFUNCTION()
	void OnBallHit(
		UPrimitiveComponent* HitComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		FVector NormalImpulse,
		const FHitResult& Hit
	);

	bool CanHit(AActor* OtherActor) const;
	void MarkHit(AActor* OtherActor);

	// ✅ 반드시 2개 인자(Actor + 위치)
	void ApplyKnockbackToCharacter(AActor* OtherActor, const FVector& FromBallWorld) const;

	// ✅ 히트 직후 잠깐 Pawn 충돌 끊기(버벅임 제거)
	void TemporarilyIgnorePawn(USphereComponent* BallBody) const;
};
