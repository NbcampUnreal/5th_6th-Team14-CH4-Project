#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WavePath.generated.h"

class USceneComponent;
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

	// ====== 구슬 설정 ======
	UPROPERTY(EditAnywhere, Category = "Cradle|Setup", meta = (ClampMin = "1", ClampMax = "20"))
	int32 BallCount = 6;

	UPROPERTY(EditAnywhere, Category = "Cradle|Setup", meta = (ClampMin = "0.0"))
	float BallSpacing = 120.0f;

	UPROPERTY(EditAnywhere, Category = "Cradle|Setup", meta = (ClampMin = "0.01"))
	float BallScale = 1.0f;

	// 비워두면 기본 Sphere 사용
	UPROPERTY(EditAnywhere, Category = "Cradle|Setup")
	UStaticMesh* BallMesh = nullptr;

	// ====== 움직임 ======
	UPROPERTY(EditAnywhere, Category = "Cradle|Motion", meta = (ClampMin = "0.0"))
	float PushAmplitude = 160.0f;

	// 주기(느릴수록 피하기 쉬움) 추천: 1.6~2.2
	UPROPERTY(EditAnywhere, Category = "Cradle|Motion", meta = (ClampMin = "0.05"))
	float Period = 1.8f;

	// 구슬별 위상차(너무 크면 따라락 느낌) 추천: 0.12~0.20
	UPROPERTY(EditAnywhere, Category = "Cradle|Motion", meta = (ClampMin = "0.0"))
	float PhaseDelay = 0.15f;

	// 전체 난이도 완화 스케일(0.5~0.8 추천)
	UPROPERTY(EditAnywhere, Category = "Cradle|Motion", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float GlobalEasyScale = 0.65f;

	// 방향 축(Forward가 옆으로 잡혔을 때)
	UPROPERTY(EditAnywhere, Category = "Cradle|Motion")
	bool bUseRightVector = false;

	// 방향 반전(필요시)
	UPROPERTY(EditAnywhere, Category = "Cradle|Motion")
	bool bInvertDirection = false;

	// ✅ 구슬마다 좌/우 방향을 번갈아 적용
	UPROPERTY(EditAnywhere, Category = "Cradle|Motion")
	bool bAlternateDirectionPerBall = true;

	// ✅ 구슬마다 진폭을 약간씩 다르게(안전 구간/피할 여지)
	UPROPERTY(EditAnywhere, Category = "Cradle|Motion", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float AmplitudeJitter = 0.25f; // ±25%

	// ✅ 랜덤 패턴 고정용 시드(항상 같은 패턴 원하면 값 고정)
	UPROPERTY(EditAnywhere, Category = "Cradle|Motion")
	int32 RandomSeed = 12345;

	// ====== 타격(넉백) ======
	UPROPERTY(EditAnywhere, Category = "Cradle|Hit", meta = (ClampMin = "0.0"))
	float KnockbackStrength = 1050.0f;

	UPROPERTY(EditAnywhere, Category = "Cradle|Hit", meta = (ClampMin = "0.0"))
	float UpwardBoost = 450.0f;

	UPROPERTY(EditAnywhere, Category = "Cradle|Hit", meta = (ClampMin = "0.0"))
	float HitCooldown = 0.25f;

private:
	UPROPERTY()
	USceneComponent* Root = nullptr;

	UPROPERTY()
	TArray<UStaticMeshComponent*> Balls;

	TArray<FVector> BaseLocalLocations;

	// 구슬별 방향(+1/-1), 구슬별 진폭 스케일(0.8~1.2 등)
	TArray<float> BallDirSign;
	TArray<float> BallAmpScale;

	// 히트 쿨타임
	TMap<TWeakObjectPtr<AActor>, float> LastHitTime;

	// 유틸
	FVector GetMoveDir() const;
	float GetNow() const;

	void RebuildBallsIfNeeded();
	void DestroyBalls();
	void BuildBalls();
	void InitPerBallMotionParams();
	void UpdateBalls();

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
