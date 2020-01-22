// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include <UObject/NoExportTypes.h>
#include "XD_LogicLodUnitBase.generated.h"

class UXD_LogicLodSystemRuntime;
class UXD_LogicLodUnitBase;

UCLASS()
class XD_LOGICLODSYSTEM_API UXD_LogicLodLevelUnit : public UObject
{
	GENERATED_BODY()

	friend class UXD_LogicLodSystemRuntime;
public:
	UPROPERTY(VisibleAnywhere, SaveGame)
	TArray<UXD_LogicLodUnitBase*> LogicLodUnits;

	UPROPERTY(VisibleAnywhere, SaveGame)
	FIntVector SavedWorldOrigin;

	uint8 bIsLevelLoaded : 1;

	UWorld* GetWorld() const override;
protected:
	virtual void Tick(float DeltaTime) {}
};

UCLASS()
class XD_LOGICLODSYSTEM_API UXD_LogicLodUnitBase : public UObject
{
	GENERATED_BODY()

	friend class UXD_LogicLodSystemRuntime;
	friend class IXD_LogicLodInstanceInterface;
public:
	UPROPERTY(VisibleAnywhere, SaveGame)
	TSoftObjectPtr<AActor> AI_Instance;

protected:
	// 向实例同步数据，可添加实例不存在时生成的逻辑
	virtual void SyncToInstance(ULevel* Level, bool IsInit);
	// 实例数据同步回Lod系统
	virtual void SyncToLodUnit();
	virtual void Tick(float DeltaTime) {}

protected:
	UPROPERTY(Transient)
	UXD_LogicLodLevelUnit* LogicLodLevelUnit;

	UWorld* GetWorld() const override;
};
