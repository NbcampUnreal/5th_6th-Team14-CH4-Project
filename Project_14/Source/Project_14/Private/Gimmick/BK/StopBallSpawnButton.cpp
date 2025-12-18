#include "Gimmick/BK/StopBallSpawnButton.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Gimmick/BK/BallSpawner.h"
#include "GameFramework/Character.h"
#include "Net/UnrealNetwork.h"

AStopBallSpawnButton::AStopBallSpawnButton()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	ButtonMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ButtonMesh"));
	SetRootComponent(ButtonMesh);

	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	TriggerBox->SetupAttachment(RootComponent);
	TriggerBox->SetCollisionProfileName(TEXT("Trigger"));
}

void AStopBallSpawnButton::BeginPlay()
{
	Super::BeginPlay();

	TriggerBox->OnComponentBeginOverlap.AddDynamic(
		this, &AStopBallSpawnButton::OnOverlapBegin);

	TriggerBox->OnComponentEndOverlap.AddDynamic(
		this, &AStopBallSpawnButton::OnOverlapEnd);
}

void AStopBallSpawnButton::OnOverlapBegin(
	UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (!OtherActor || !OtherActor->IsA<ACharacter>())
		return;

	ServerPress();
}

void AStopBallSpawnButton::OnOverlapEnd(
	UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
	if (!OtherActor || !OtherActor->IsA<ACharacter>())
		return;

	ServerRelease();
}

void AStopBallSpawnButton::ServerPress_Implementation()
{
	if (bIsPressed)
		return;

	bIsPressed = true;

	for (ABallSpawner* Spawner : TargetSpawners)
	{
		if (Spawner)
		{
			Spawner->StopSpawn();
		}
	}

	OnRep_ButtonState();
}

void AStopBallSpawnButton::ServerRelease_Implementation()
{
	bIsPressed = false;

	for (ABallSpawner* Spawner : TargetSpawners)
	{
		if (Spawner)
		{
			Spawner->StartSpawn();
		}
	}

	OnRep_ButtonState();
}

void AStopBallSpawnButton::OnRep_ButtonState()
{
}

void AStopBallSpawnButton::GetLifetimeReplicatedProps(
	TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AStopBallSpawnButton, bIsPressed);
}
