#include "Gimmick/BK/ButtonActor_A.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Gimmick/BK/GateActor.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/Character.h"

AButtonActor_A::AButtonActor_A()
{
	PrimaryActorTick.bCanEverTick = false;

	bReplicates = true;   

	ButtonMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ButtonMesh"));
	SetRootComponent(ButtonMesh);

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
			TEXT("ToggleMode와 OneTimeActivation은 동시에 사용할 수 없습니다."));
		bOneTimeActivation = false;
	}

	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &AButtonActor_A::OnOverlapBegin);
	TriggerBox->OnComponentEndOverlap.AddDynamic(this, &AButtonActor_A::OnOverlapEnd);
}

void AButtonActor_A::OnOverlapBegin(
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
		ServerHandlePress();
	}
	else
	{
		ServerHandlePress(); 
	}
}

void AButtonActor_A::OnOverlapEnd(
	UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
	if (!OtherActor || !OtherActor->IsA<ACharacter>())
		return;

	if (!bToggleMode && !bOneTimeActivation)
	{
		if (HasAuthority())
		{
			ServerHandleRelease();
		}
		else
		{
			ServerHandleRelease(); 
		}
	}
}

void AButtonActor_A::ServerHandlePress_Implementation()
{
	if (TargetGates.Num() == 0)
		return;

	if (bToggleMode)
	{
		bIsGateOpen = !bIsGateOpen;
	}
	else
	{
		bIsGateOpen = true;
	}

	if (bIsGateOpen)
	{
		OpenAllGates();
	}
	else
	{
		CloseAllGates();
	}

	if (bOneTimeActivation)
	{
		TriggerBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	OnRep_ButtonState();
}

void AButtonActor_A::ServerHandleRelease_Implementation()
{
	if (TargetGates.Num() == 0)
		return;

	bIsGateOpen = false;
	CloseAllGates();

	OnRep_ButtonState();
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

void AButtonActor_A::OnRep_ButtonState()
{
	// 사운드 / 애니메이션 / 머티리얼 변경용
}


void AButtonActor_A::GetLifetimeReplicatedProps(
	TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AButtonActor_A, bIsGateOpen);
}
