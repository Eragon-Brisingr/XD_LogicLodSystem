// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include <UObject/NoExportTypes.h>
#include "XD_AI_LodUnitBase.generated.h"

class UXD_AI_LodSystemRuntime;
struct FXD_AI_LodLevelUnit;

UCLASS()
class XD_AI_LODSYSTEM_API UXD_AI_LodUnitBase : public UObject
{
	GENERATED_BODY()

	friend class UXD_AI_LodSystemRuntime;
	friend class IXD_AI_LodInstanceInterface;
public:
	UPROPERTY(SaveGame)
	TSoftObjectPtr<AActor> AI_Instance;

protected:
	// 向实例同步数据，可添加实例不存在时生成的逻辑
	virtual void SyncToInstance(ULevel* Level, bool IsInit, const FXD_AI_LodLevelUnit& AI_LodLevelUnit);
	// 实例数据同步回Lod系统
	virtual void SyncToLodUnit();
	virtual void Tick(float DeltaTime) {}

protected:
	UPROPERTY(Transient)
	UXD_AI_LodSystemRuntime* LodSystemRuntime;

	UWorld* GetWorld() const override;
};
