#include "Gimmick/BK/NumberPlatePuzzleManager.h"
#include "Net/UnrealNetwork.h"
#include "Gimmick/BK/GateActor.h"

ANumberPlatePuzzleManager::ANumberPlatePuzzleManager()
{
	bReplicates = true;
}

void ANumberPlatePuzzleManager::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		TArray<int32> Pool = { 1,2,3,4,5,6 };

		int32 IndexA = FMath::RandRange(0, Pool.Num() - 1);
		int32 IndexB;

		do
		{
			IndexB = FMath::RandRange(0, Pool.Num() - 1);
		} while (IndexA == IndexB);

		AnswerNumbers = { Pool[IndexA], Pool[IndexB] };

		UE_LOG(LogTemp, Warning,
			TEXT("[Puzzle] 정답 발판: %d, %d"),
			AnswerNumbers[0], AnswerNumbers[1]);
	}
}

void ANumberPlatePuzzleManager::NotifyPlateStepped(int32 PlateNumber)
{
	if (!HasAuthority()) return;

	CurrentSteppedPlates.Add(PlateNumber);
	CheckClear();
}

void ANumberPlatePuzzleManager::NotifyPlateReleased(int32 PlateNumber)
{
	if (!HasAuthority()) return;

	CurrentSteppedPlates.Remove(PlateNumber);
}

void ANumberPlatePuzzleManager::CheckClear()
{
	for (int32 Answer : AnswerNumbers)
	{
		if (!CurrentSteppedPlates.Contains(Answer))
		{
			return;
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("퍼즐 클리어!"));

	if (GateToOpen)
	{
		GateToOpen->OpenGate();
	}
}

void ANumberPlatePuzzleManager::GetLifetimeReplicatedProps(
	TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ANumberPlatePuzzleManager, AnswerNumbers);
}
