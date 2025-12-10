#include "Gimmick/BK/NumberPlate.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Character.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "Gimmick/BK/GateActor.h"

int32 ANumberPlate::GlobalAnswerNumber = 1;
UWorld* ANumberPlate::CachedWorld = nullptr;

ANumberPlate::ANumberPlate()
{
	PrimaryActorTick.bCanEverTick = false;
	PlateMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PlateMesh"));
	SetRootComponent(PlateMesh);

	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	TriggerBox->SetupAttachment(RootComponent);
	TriggerBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TriggerBox->SetCollisionObjectType(ECC_WorldDynamic);
	TriggerBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	TriggerBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	TriggerBox->SetGenerateOverlapEvents(true);
}

void ANumberPlate::BeginPlay()
{
	Super::BeginPlay();

	if (TriggerBox)
	{
		TriggerBox->OnComponentBeginOverlap.AddDynamic(
			this, &ANumberPlate::OnOverlapBegin);
	}

	UWorld* World = GetWorld();
	if (World && World != CachedWorld)
	{
		CachedWorld = World;
		GlobalAnswerNumber = FMath::RandRange(1, 6);

		UE_LOG(LogTemp, Warning, TEXT("[NumberPlate] 새 퍼즐 정답 숫자: %d"), GlobalAnswerNumber);

		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1, 3.f, FColor::Yellow,
				FString::Printf(TEXT("퍼즐 정답: %d"), GlobalAnswerNumber));
		}
	}
}

void ANumberPlate::OnOverlapBegin(
	UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (bCleared)
	{
		return;
	}

	if (!OtherActor || OtherActor == this)
	{
		return;
	}

	if (!OtherActor->IsA(ACharacter::StaticClass()))
	{
		return;
	}

	if (PlateNumber == GlobalAnswerNumber)
	{
		bCleared = true;

		UE_LOG(LogTemp, Warning, TEXT("[NumberPlate] 정답 발판입니다! (%d) 퍼즐 클리어!"), PlateNumber);

		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1, 3.f, FColor::Green,
				FString::Printf(TEXT("정답 발판 %d! 퍼즐 클리어!"), PlateNumber));
		}

		if (GateToOpen)
		{
			GateToOpen->OpenGate();
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[NumberPlate] 오답 발판입니다. (누른 번호: %d, 정답: %d)"), PlateNumber, GlobalAnswerNumber);

		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1, 2.f, FColor::Red,
				FString::Printf(TEXT("오답 발판 %d (정답은 %d)"), PlateNumber, GlobalAnswerNumber));
		}
	}
}