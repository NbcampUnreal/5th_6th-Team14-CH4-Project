// Fill out your copyright notice in the Description page of Project Settings.


#include "Gimmick/SIC/MakerBlock.h"

// Sets default values
AMakerBlock::AMakerBlock()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	RootComponent = SceneRoot;

	BlockMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BlockMesh"));
	BlockMesh->SetupAttachment(SceneRoot);
}

// Called when the game starts or when spawned
void AMakerBlock::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMakerBlock::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

