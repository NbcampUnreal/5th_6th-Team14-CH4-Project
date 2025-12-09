#include "Gimmick/BK/ButtonActor_A.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Gimmick/BK/GateActor.h"

AButtonActor_A::AButtonActor_A()
{
	ButtonMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ButtonMesh"));
	RootComponent = ButtonMesh;

	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	TriggerBox->SetupAttachment(RootComponent);
	TriggerBox->SetCollisionProfileName("Trigger");
}

void AButtonActor_A::BeginPlay()
{
	Super::BeginPlay();

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
	if (TargetGate)
		TargetGate->OpenGate();

	if (bOneTimeActivation)
		TriggerBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AButtonActor_A::OnOverlapEnd(
	UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
	if (!bOneTimeActivation && TargetGate)
		TargetGate->CloseGate();
}





