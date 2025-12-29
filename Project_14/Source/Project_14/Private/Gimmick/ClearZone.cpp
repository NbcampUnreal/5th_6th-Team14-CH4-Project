#include "Gimmick/ClearZone.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Character.h"
#include "GameManager/StageGameModeBase.h"
#include "GameManager/ProjectGameStateBase.h"
#include "Net/UnrealNetwork.h"
#include "UI/MH/InGameHUDWidget.h"

AClearZone::AClearZone()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	ClearBox = CreateDefaultSubobject<UBoxComponent>(TEXT("ClearBox"));
	ClearBox->SetupAttachment(Root);
	ClearBox->SetCollisionProfileName(TEXT("Trigger"));
}

void AClearZone::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		ClearBox->OnComponentBeginOverlap.AddDynamic(
			this, &AClearZone::OnOverlapBegin);

		ClearBox->OnComponentEndOverlap.AddDynamic(
			this, &AClearZone::OnOverlapEnd);
	}
}

void AClearZone::OnOverlapBegin(
	UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (bIsCleared)
		return;

	ACharacter* Character = Cast<ACharacter>(OtherActor);
	if (!Character)
		return;

	OverlappingCharacters.Add(Character);

	CheckClearCondition();
}

void AClearZone::OnOverlapEnd(
	UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
	ACharacter* Character = Cast<ACharacter>(OtherActor);
	if (!Character)
		return;

	OverlappingCharacters.Remove(Character);
}

void AClearZone::CheckClearCondition()
{
	if (bIsCleared)
		return;

	if (OverlappingCharacters.Num() >= RequiredPlayerCount)
	{
		AProjectGameStateBase* GS = GetWorld()->GetGameState<AProjectGameStateBase>();

		if (GS->ClearPlayTime < 0.f)
		{
			GS->ClearPlayTime = GS->GetPlayTime();
			int32 TotalSeconds = FMath::FloorToInt(GS->ClearPlayTime);
			int32 Minutes = TotalSeconds / 60;
			int32 Seconds = TotalSeconds % 60;

			GS->ClearTimeString =
				FString::Printf(TEXT("Play Time : %02d:%02d"), Minutes, Seconds);
		}
		OnGameClear();
	}
}

void AClearZone::OnGameClear()
{
	bIsCleared = true;

	UE_LOG(LogTemp, Warning, TEXT("GAME CLEAR!"));

	AStageGameModeBase* SGM = GetWorld()->GetAuthGameMode<AStageGameModeBase>();
	SGM->NotifyGameClear();
	//  여기서 GameMode, UI 호출
}

