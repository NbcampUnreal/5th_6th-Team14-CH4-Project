#include "Gimmick/SIC/StopBridgeJump.h"

#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Gimmick/SIC/DragonBridgeJump.h"

AStopBridgeJump::AStopBridgeJump()
{
	PrimaryActorTick.bCanEverTick = false;

	bReplicates = true;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	RootComponent = SceneRoot;

	TriggerMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TriggerMesh"));
	TriggerMesh->SetupAttachment(SceneRoot);

	TriggerMesh->SetGenerateOverlapEvents(true);
	TriggerMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TriggerMesh->SetCollisionResponseToAllChannels(ECR_Overlap);
}

void AStopBridgeJump::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		TriggerMesh->OnComponentBeginOverlap.AddDynamic(
			this,
			&AStopBridgeJump::OnOverlapBegin
		);
	}
}

void AStopBridgeJump::OnOverlapBegin(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult
)
{
	if (!HasAuthority())
		return;

	if (!OtherActor)
		return;

	TArray<AActor*> Found;
	UGameplayStatics::GetAllActorsOfClass(
		GetWorld(),
		ADragonBridgeJump::StaticClass(),
		Found
	);

	for (AActor* A : Found)
	{
		ADragonBridgeJump* Bridge = Cast<ADragonBridgeJump>(A);
		if (Bridge)
		{
			Bridge->StopBridge();
		}
	}
}
