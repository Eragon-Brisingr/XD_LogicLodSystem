// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "XD_LogicLodUnitBase.h"
#include "XD_LogicLodInstanceInterface.h"
#include "XD_SaveGameInterface.h"
#include "XD_LogicLod_Example.generated.h"

UCLASS()
class XD_LOGICLODSYSTEM_API UXD_LogicLodUnit_Example : public UXD_LogicLodUnitBase
{
	GENERATED_BODY()
public:
	UPROPERTY(VisibleAnywhere, SaveGame)
	float AI_SyncValue;

	void SyncToInstance(ULevel* Level, bool IsInit) override;
	void SyncToLodUnit() override;
	void Tick(float DeltaTime) override;
};

UCLASS()
class XD_LOGICLODSYSTEM_API AXD_LogicLod_Example : public AActor, public IXD_LogicLodInstanceInterface, public IXD_SaveGameInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AXD_LogicLod_Example();

protected:
	// Called when the game starts or when spawned
	void BeginPlay() override;

public:	
	// Called every frame
	void Tick(float DeltaTime) override;

	UXD_LogicLodUnitBase* CreateLogicLodUnit_Implementation(UObject* Outer) const override;
	ELogicLodSystemRegisterType GetLogicLodRegisterType_Implementation() const override { return ELogicLodSystemRegisterType::WorldInit; }

	UPROPERTY(VisibleAnywhere, SaveGame, BlueprintReadOnly)
	float AI_SyncValue;
};
