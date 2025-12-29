#include "Gimmick/ClearZone.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Character.h"
#include "GameManager/StageGameModeBase.h"
#include "Net/UnrealNetwork.h"

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

