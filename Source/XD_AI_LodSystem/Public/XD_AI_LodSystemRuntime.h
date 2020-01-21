// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include <Components/ActorComponent.h>
#include "XD_SaveGameInterface.h"
#include "XD_AI_LodSystemRuntime.generated.h"

class UXD_AI_LodUnitBase;

USTRUCT()
struct XD_AI_LODSYSTEM_API FXD_AI_LodLevelUnit
{
	GENERATED_BODY()
public:
	UPROPERTY(VisibleAnywhere, SaveGame)
	TArray<UXD_AI_LodUnitBase*> AI_LodUnits;

	UPROPERTY(VisibleAnywhere, SaveGame)
	FIntVector SavedWorldOrigin;

	uint8 bIsLevelLoaded : 1;
};

UCLASS()
class XD_AI_LODSYSTEM_API UXD_AI_LodSystemRuntime : public UActorComponent,
	public IXD_SaveGameInterface
{
	GENERATED_BODY()
public:
	UXD_AI_LodSystemRuntime();

	void WhenGameInit_Implementation() override;
	void WhenPostLoad_Implementation() override;

	void BeginPlay() override;
	void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;
private:
	void RegisterAI_LodSystem();
	void WhenLevelInited(ULevel* Level);
	void WhenLevelLoaded(ULevel* Level);
	void SyncLevelUnitToInstance(ULevel* Level, bool IsInit);

	void WhenLevelPreUnload(ULevel* Level);

	FDelegateHandle OnActorSpawnedHandler;
	void WhenActorSpawned(AActor* Actor);
	UFUNCTION()
	void WhenInstanceDestroyed(AActor* Actor);

	UPROPERTY(EditAnywhere, SaveGame)
	TMap<FName, FXD_AI_LodLevelUnit> AI_LodLevelUnits;

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
