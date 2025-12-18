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

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UStaticMeshComponent> ButtonMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UBoxComponent> TriggerBox;

    // ✅ 버튼 인스턴스(레벨에 놓인 버튼)마다 다르게 지정하기 좋음
    UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Button")
    TArray<TObjectPtr<AActor>> TargetWalls;

    // ✅ 중복 발동 방지 (서버에서 true로 바꾸면 클라에도 반영됨)
    UPROPERTY(Replicated)
    bool bAlreadyActivated = false;

    UFUNCTION()
    void OnOverlapBegin(
        UPrimitiveComponent* OverlappedComp,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComp,
        int32 OtherBodyIndex,
        bool bFromSweep,
        const FHitResult& SweepResult);

    UFUNCTION()
    void OnOverlapEnd(
        UPrimitiveComponent* OverlappedComp,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComp,
        int32 OtherBodyIndex);

    // ✅ 클라에서 밟았으면 서버에게 “삭제 실행” 요청
    UFUNCTION(Server, Reliable)
    void ServerActivate(AActor* Activator);
    void ServerActivate_Implementation(AActor* Activator);

    // 서버에서만 실제 삭제 수행
    void ActivateOnServer(AActor* Activator);

public:
    virtual void Tick(float DeltaTime) override;

    // bAlreadyActivated 복제용
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
