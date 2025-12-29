#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WavePath.generated.h"

class USceneComponent;
class USphereComponent;
class UStaticMeshComponent;
class UStaticMesh;

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
	//  Cradle | Setup
	// =========================

	UPROPERTY(EditAnywhere, Category = "Cradle|Setup", meta = (ClampMin = "1", ClampMax = "50"))
	int32 BallCount = 9;

	UPROPERTY(EditAnywhere, Category = "Cradle|Setup", meta = (ClampMin = "0.0"))
	float BallSpacing = 160.0f;

	// 비주얼 크기(보이는 공 크기)
	UPROPERTY(EditAnywhere, Category = "Cradle|Setup", meta = (ClampMin = "0.01"))
	float BallScale = 1.0f;

	// 비주얼 메쉬(비워두면 기본 Sphere)
	UPROPERTY(EditAnywhere, Category = "Cradle|Setup")
	UStaticMesh* BallMesh = nullptr;

	// ✅ 길로틴처럼 듬성듬성 배치: N개마다 큰 간격 추가
	UPROPERTY(EditAnywhere, Category = "Cradle|Setup", meta = (ClampMin = "0"))
	int32 GapEveryN = 3;

	UPROPERTY(EditAnywhere, Category = "Cradle|Setup", meta = (ClampMin = "0.0"))
	float ExtraGapSize = 260.0f;

	// ✅ 진짜 “빈 칸” 만들기: N번째마다 공을 아예 생성하지 않음
	UPROPERTY(EditAnywhere, Category = "Cradle|Setup")
	bool bSkipEveryNBall = true;

	UPROPERTY(EditAnywhere, Category = "Cradle|Setup", meta = (ClampMin = "2"))
	int32 SkipEveryN = 4;

	// =========================
	//  Cradle | Motion
	// =========================

	UPROPERTY(EditAnywhere, Category = "Cradle|Motion", meta = (ClampMin = "0.0"))
	float PushAmplitude = 240.0f;

	UPROPERTY(EditAnywhere, Category = "Cradle|Motion", meta = (ClampMin = "0.05"))
	float Period = 2.2f;

	UPROPERTY(EditAnywhere, Category = "Cradle|Motion", meta = (ClampMin = "0.0"))
	float PhaseDelay = 0.14f;

	UPROPERTY(EditAnywhere, Category = "Cradle|Motion", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float GlobalEasyScale = 0.70f;

	UPROPERTY(EditAnywhere, Category = "Cradle|Motion")
	bool bUseRightVector = false;

	UPROPERTY(EditAnywhere, Category = "Cradle|Motion")
	bool bInvertDirection = false;

	// ✅ 구슬마다 좌/우 방향 번갈아
	UPROPERTY(EditAnywhere, Category = "Cradle|Motion")
	bool bAlternateDirectionPerBall = true;

	// ✅ 구슬별 진폭 약간 랜덤
	UPROPERTY(EditAnywhere, Category = "Cradle|Motion", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float AmplitudeJitter = 0.20f;

	UPROPERTY(EditAnywhere, Category = "Cradle|Motion")
	int32 RandomSeed = 12345;

	// =========================
	//  Cradle | Hit
	// =========================

	UPROPERTY(EditAnywhere, Category = "Cradle|Hit", meta = (ClampMin = "0.0"))
	float KnockbackStrength = 1050.0f;

	UPROPERTY(EditAnywhere, Category = "Cradle|Hit", meta = (ClampMin = "0.0"))
	float UpwardBoost = 450.0f;

	UPROPERTY(EditAnywhere, Category = "Cradle|Hit", meta = (ClampMin = "0.0"))
	float HitCooldown = 0.25f;

	// ✅ 핵심: "충돌 판정만" 줄이는 비율 (1.0=그대로, 0.8=20% 축소)
	UPROPERTY(EditAnywhere, Category = "Cradle|Hit", meta = (ClampMin = "0.1", ClampMax = "1.0"))
	float CollisionScale = 0.80f;

private:
	UPROPERTY()
	USceneComponent* Root = nullptr;

	// BallCount 길이로 유지 (빈 칸이면 nullptr)
	UPROPERTY()
	TArray<USphereComponent*> HitSpheres;

	UPROPERTY()
	TArray<UStaticMeshComponent*> VisualMeshes;

	// BallCount 길이로 유지 (빈 칸도 위치는 기록)
	TArray<FVector> BaseLocalLocations;

	// BallCount 길이로 유지
	TArray<float> BallDirSign;   // +1 / -1
	TArray<float> BallAmpScale;  // 0.8~1.2 등

	// 히트 쿨타임
	TMap<TWeakObjectPtr<AActor>, float> LastHitTime;

	// ====== 캐시(값이 바뀌면 재구성/재초기화) ======
	int32 CachedBallCount = -1;
	float CachedBallSpacing = -1.0f;
	float CachedBallScale = -1.0f;
	float CachedCollisionScale = -1.0f;
	UStaticMesh* CachedBallMesh = nullptr;

	int32 CachedGapEveryN = -1;
	float CachedExtraGapSize = -1.0f;
	bool CachedSkipEnabled = false;
	int32 CachedSkipEveryN = -1;

	bool CachedAltDir = false;
	float CachedJitter = -1.0f;
	int32 CachedSeed = INT32_MIN;

	// 유틸
	float GetNow() const;
	FVector GetMoveDir() const;

	// 갱신
	void RefreshIfNeeded();
	void DestroyBalls();
	void BuildBalls();
	void InitPerBallMotionParams();
	void UpdateBalls();

	// 충돌 반경 계산(UE 기본 Sphere는 반지름 50 기준)
	float CalcCollisionRadius() const;

	UFUNCTION()
	void OnBallBeginOverlap(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

	bool CanHit(AActor* OtherActor) const;
	void MarkHit(AActor* OtherActor);
	void ApplyKnockbackToCharacter(AActor* OtherActor) const;
};
