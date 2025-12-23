#include "Gimmick/PortalActor.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"

APortalActor::APortalActor()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	PortalMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PortalMesh"));
	PortalMesh->SetupAttachment(Root);

	PortalFX = CreateDefaultSubobject<UNiagaraComponent>(TEXT("PortalFX"));
	PortalFX->SetupAttachment(Root);

	PortalFX->SetAutoActivate(true);
	PortalFX->SetIsReplicated(false); 

	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	TriggerBox->SetupAttachment(Root);
	TriggerBox->SetCollisionProfileName(TEXT("Trigger"));
}

void APortalActor::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		TriggerBox->OnComponentBeginOverlap.AddDynamic(
			this, &APortalActor::OnOverlapBegin);
	}
}

void APortalActor::OnOverlapBegin(
	UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (!HasAuthority())
		return;

	ACharacter* Character = Cast<ACharacter>(OtherActor);
	if (!Character || !LinkedPortal)
		return;

	ServerTeleport(Character);
}

void APortalActor::ServerTeleport_Implementation(ACharacter* Character)
{
	if (!Character || !LinkedPortal)
		return;

	float* LastTime = LastTeleportTime.Find(Character);
	float CurrentTime = GetWorld()->GetTimeSeconds();

	if (LastTime && CurrentTime - *LastTime < TeleportCooldown)
		return;

	LastTeleportTime.Add(Character, CurrentTime);

	TeleportCharacter(Character);
}

void APortalActor::TeleportCharacter(ACharacter* Character)
{
	if (!Character || !LinkedPortal)
		return;

	FVector TargetLocation =
		LinkedPortal->GetActorLocation() +
		LinkedPortal->GetActorForwardVector() * 120.f;

	FRotator TargetRotation = LinkedPortal->GetActorRotation();

	Character->TeleportTo(TargetLocation, TargetRotation, false, true);

	UE_LOG(LogTemp, Warning,
		TEXT("%s Teleported to %s"),
		*Character->GetName(),
		*LinkedPortal->GetName());
}

