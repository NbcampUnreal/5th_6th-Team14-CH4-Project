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

	if (HasAuthority())
	{
		ServerPress();
	}
	else
	{
		ServerPress(); 
	}
}

void AStopBallSpawnButton::OnOverlapEnd(
	UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
	if (!OtherActor || !OtherActor->IsA<ACharacter>())
		return;

	if (HasAuthority())
	{
		ServerRelease();
	}
	else
	{
		ServerRelease(); 
	}
}

void AStopBallSpawnButton::ServerPress_Implementation()
{
	if (!TargetSpawner)
		return;

	if (bIsPressed)
		return;

	bIsPressed = true;
	TargetSpawner->StopSpawn();

	OnRep_ButtonState();
}

void AStopBallSpawnButton::ServerRelease_Implementation()
{
	if (!TargetSpawner)
		return;

	bIsPressed = false;
	TargetSpawner->StartSpawn();

	OnRep_ButtonState();
}

void AStopBallSpawnButton::OnRep_ButtonState()
{
	// 버튼 눌림 애니메이션 / 머티리얼 / 사운드용
}

void AStopBallSpawnButton::GetLifetimeReplicatedProps(
	TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AStopBallSpawnButton, bIsPressed);
}
