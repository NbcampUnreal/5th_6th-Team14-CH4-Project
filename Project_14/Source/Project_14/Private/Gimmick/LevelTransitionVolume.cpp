#include "Gimmick/LevelTransitionVolume.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/GameStateBase.h"
#include "Engine/World.h"

ALevelTransitionVolume::ALevelTransitionVolume()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	SetRootComponent(TriggerBox);

	TriggerBox->SetBoxExtent(FVector(200.f, 200.f, 100.f));
	TriggerBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TriggerBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	TriggerBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	TriggerBox->SetGenerateOverlapEvents(true);
}

void ALevelTransitionVolume::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		TriggerBox->OnComponentBeginOverlap.AddDynamic(
			this,
			&ThisClass::OnOverlapBegin
		);

		TriggerBox->OnComponentEndOverlap.AddDynamic(
			this,
			&ThisClass::OnOverlapEnd
		);
	}
}

void ALevelTransitionVolume::OnOverlapBegin(
	UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (!HasAuthority()) return;

	APawn* Pawn = Cast<APawn>(OtherActor);
	if (!Pawn) return;

	APlayerState* PS = Pawn->GetPlayerState();
	if (!PS) return;

	OverlappedPlayerStates.Add(PS);

	UE_LOG(LogTemp, Log,
		TEXT("[LevelTransition] Player Entered. Count = %d"),
		OverlappedPlayerStates.Num()
	);

	TryServerTravel();
}

void ALevelTransitionVolume::OnOverlapEnd(
	UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex
)
{
	if (!HasAuthority()) return;

	APawn* Pawn = Cast<APawn>(OtherActor);
	if (!Pawn) return;

	APlayerState* PS = Pawn->GetPlayerState();
	if (!PS) return;

	OverlappedPlayerStates.Remove(PS);

	UE_LOG(LogTemp, Log,
		TEXT("[LevelTransition] Player Left. Count = %d"),
		OverlappedPlayerStates.Num()
	);
}

void ALevelTransitionVolume::TryServerTravel()
{
	if (!HasAuthority()) return;

	if (OverlappedPlayerStates.Num() < RequiredPlayerCount)
	{
		return;
	}

	if (NextLevelName.IsNone())
	{
		UE_LOG(LogTemp, Warning, TEXT("[LevelTransition] NextLevelName is None"));
		return;
	}

	UWorld* World = GetWorld();
	if (!World) return;

	const FString LevelPath =
		FString::Printf(TEXT("/Game/Level/%s"), *NextLevelName.ToString());

	if (AGameStateBase* GS = World->GetGameState())
	{
		UE_LOG(LogTemp, Warning,
			TEXT("[LevelTransition] ServerTravel: %s | ConnectedPlayers=%d"),
			*LevelPath,
			GS->PlayerArray.Num()
		);
	}

	const bool bSuccess = World->ServerTravel(LevelPath);

	UE_LOG(LogTemp, Warning,
		TEXT("[LevelTransition] ServerTravel Result: %s"),
		bSuccess ? TEXT("SUCCESS") : TEXT("FAIL")
	);
}
