#include "Gimmick/BK/ButtonActor_A.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Gimmick/BK/GateActor.h"

AButtonActor_A::AButtonActor_A()
{
	PrimaryActorTick.bCanEverTick = false;

	ButtonMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ButtonMesh"));
	RootComponent = ButtonMesh;

	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	TriggerBox->SetupAttachment(RootComponent);
	TriggerBox->SetCollisionProfileName(TEXT("Trigger"));
}

void AButtonActor_A::BeginPlay()
{
	Super::BeginPlay();

	if (bToggleMode && bOneTimeActivation)
	{
		UE_LOG(LogTemp, Warning,
			TEXT("ToggleMode와 OneTimeActivation은 동시에 사용할 수 없습니다. OneTimeActivation을 false로 변경합니다."));
		bOneTimeActivation = false;
	}

	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &AButtonActor_A::OnOverlapBegin);
	TriggerBox->OnComponentEndOverlap.AddDynamic(this, &AButtonActor_A::OnOverlapEnd);
}

void AButtonActor_A::OpenAllGates()
{
	for (AGateActor* Gate : TargetGates)
	{
		if (Gate)
		{
			Gate->OpenGate();
		}
	}
}

void AButtonActor_A::CloseAllGates()
{
	for (AGateActor* Gate : TargetGates)
	{
		if (Gate)
		{
			Gate->CloseGate();
		}
	}
}

void AButtonActor_A::OnOverlapBegin(
	UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (TargetGates.Num() == 0)
		return;

	if (bToggleMode)
	{
		if (bIsGateOpen)
		{
			CloseAllGates();
			bIsGateOpen = false;
		}
		else
		{
			OpenAllGates();
			bIsGateOpen = true;
		}

		if (bOneTimeActivation)
		{
			TriggerBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}

		return;
	}

	/* Normal Mode */
	OpenAllGates();

	if (bOneTimeActivation)
	{
		TriggerBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void AButtonActor_A::OnOverlapEnd(
	UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
	if (TargetGates.Num() == 0)
		return;

	if (bToggleMode)
		return;

	if (!bOneTimeActivation)
	{
		CloseAllGates();
	}
}
