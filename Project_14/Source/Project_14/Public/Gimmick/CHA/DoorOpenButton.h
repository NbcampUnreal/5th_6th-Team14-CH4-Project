#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DoorOpenButton.generated.h"

class UStaticMeshComponent;
class UBoxComponent;
class UPrimitiveComponent;

UCLASS()
class PROJECT_14_API ADoorOpenButton : public AActor
{
    GENERATED_BODY()

public:
    ADoorOpenButton();

protected:
    virtual void BeginPlay() override;

    // ===== Components =====
    UPROPERTY(VisibleAnywhere, Category = "Components")
    TObjectPtr<UStaticMeshComponent> ButtonMesh;

    UPROPERTY(VisibleAnywhere, Category = "Components")
    TObjectPtr<UBoxComponent> TriggerBox;

    // ===== Targets (레벨에 배치한 큐브 2개 지정) =====
    UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Button")
    TObjectPtr<AActor> TargetCube1;

    UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Button")
    TObjectPtr<AActor> TargetCube2;

    // 한 번만 작동(원하면 false로)
    UPROPERTY(EditAnywhere, Category = "Button")
    bool bPressOnce = true;

    // ✅ 서버가 true로 만들면, 클라에도 복제되어 OnRep가 호출됨
    UPROPERTY(ReplicatedUsing = OnRep_Opened)
    bool bOpened = false;

    UFUNCTION()
    void OnRep_Opened();

    // ===== Overlap =====
    UFUNCTION()
    void OnOverlapBegin(
        UPrimitiveComponent* OverlappedComp,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComp,
        int32 OtherBodyIndex,
        bool bFromSweep,
        const FHitResult& SweepResult);

    void PressOnServer(AActor* PressingActor);

    // 실제로 "사라진 것처럼" 처리(숨김 + 충돌 끔)
    void ApplyOpenedState();

public:
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
