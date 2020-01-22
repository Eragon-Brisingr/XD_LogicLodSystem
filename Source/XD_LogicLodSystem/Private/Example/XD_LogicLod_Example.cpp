// Fill out your copyright notice in the Description page of Project Settings.


#include "Example/XD_LogicLod_Example.h"

void UXD_LogicLodUnit_Example::SyncToInstance(ULevel* Level, bool IsInit)
{
	AXD_LogicLod_Example* LogicLod_Example = CastChecked<AXD_LogicLod_Example>(AI_Instance.Get());
	LogicLod_Example->AI_SyncValue = AI_SyncValue;
}

void UXD_LogicLodUnit_Example::SyncToLodUnit()
{
	AXD_LogicLod_Example* LogicLod_Example = CastChecked<AXD_LogicLod_Example>(AI_Instance.Get());
	AI_SyncValue = LogicLod_Example->AI_SyncValue;
}

void UXD_LogicLodUnit_Example::Tick(float DeltaTime)
{
	AI_SyncValue += DeltaTime;
}

// Sets default values
AXD_LogicLod_Example::AXD_LogicLod_Example()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AXD_LogicLod_Example::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AXD_LogicLod_Example::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	AI_SyncValue += DeltaTime;
}

UXD_LogicLodUnitBase* AXD_LogicLod_Example::CreateLogicLodUnit_Implementation(UObject* Outer) const
{
	return NewObject<UXD_LogicLodUnit_Example>(Outer, *GetFullName());
}
