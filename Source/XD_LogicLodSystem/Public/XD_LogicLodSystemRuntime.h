// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include <Components/ActorComponent.h>
#include "XD_SaveGameInterface.h"
#include "XD_LogicLodSystemRuntime.generated.h"

class UXD_LogicLodUnitBase;
class UXD_LogicLodLevelUnit;

UCLASS()
class XD_LOGICLODSYSTEM_API UXD_LogicLodSystemRuntime : public UActorComponent,
	public IXD_SaveGameInterface
{
	GENERATED_BODY()
public:
	UXD_LogicLodSystemRuntime();

	void WhenGameInit_Implementation() override;
	void WhenPostLoad_Implementation() override;
	void WhenPreSave_Implementation() override;

	void BeginPlay() override;
	void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;
private:
	void RegisterLogicLodSystem();
	void WhenLevelInited(ULevel* Level);
	void WhenLevelLoaded(ULevel* Level);
	void SyncLevelUnitToInstance(ULevel* Level, bool IsInit);

	void WhenLevelPreUnload(ULevel* Level);

	FDelegateHandle OnActorSpawnedHandler;
	void WhenActorSpawned(AActor* Actor);
	UFUNCTION()
	void WhenInstanceDestroyed(AActor* Actor);

	UPROPERTY(EditAnywhere, SaveGame)
	TMap<FName, UXD_LogicLodLevelUnit*> LogicLodLevelUnits;

	UXD_LogicLodUnitBase* DuplicateLogicLodUnit(UXD_LogicLodUnitBase* LogicLodUnitTemplate, UXD_LogicLodLevelUnit* LogicLodLevelUnit);
public:
	FORCEINLINE bool IsServer() const { return GetOwner()->HasAuthority(); }

	struct FSpawnNoAIGuard
	{
		TGuardValue<bool> Guard;
		FSpawnNoAIGuard()
			:Guard(bAutoRegisterSpawnedActor, false)
		{}
	};

	static bool bAutoRegisterSpawnedActor;
};
