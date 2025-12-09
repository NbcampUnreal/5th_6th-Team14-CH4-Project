#include "Gimmick/BK/StopBallSpawnButton.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Gimmick/BK/BallSpawner.h"

AStopBallSpawnButton::AStopBallSpawnButton()
{
	ButtonMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ButtonMesh"));
	RootComponent = ButtonMesh;

	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	TriggerBox->SetupAttachment(RootComponent);
	TriggerBox->SetCollisionProfileName("Trigger");
}

void AStopBallSpawnButton::BeginPlay()
{
	Super::BeginPlay();

	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &AStopBallSpawnButton::OnOverlapBegin);
	TriggerBox->OnComponentEndOverlap.AddDynamic(this, &AStopBallSpawnButton::OnOverlapEnd);
}

void AStopBallSpawnButton::OnOverlapBegin(
	UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult& SweepResult)
{
	if (TargetSpawner)
		TargetSpawner->StopSpawn();   
}

void AStopBallSpawnButton::OnOverlapEnd(
	UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (TargetSpawner)
		TargetSpawner->StartSpawn();  
}

