// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include <UObject/NoExportTypes.h>
#include <Engine/AssetUserData.h>
#include "XD_LogicLodWorldCollection.generated.h"

class UXD_LogicLodUnitBase;

// 世界初始化时就要进行计算的，用于搜集每个关卡的LogicLodUnit
UCLASS()
class XD_LOGICLODSYSTEM_API UXD_LogicLodLevelBuiltData : public UObject
{
	GENERATED_BODY()
public:
	UPROPERTY(VisibleAnywhere)
	TArray<UXD_LogicLodUnitBase*> LogicLodUnits;

	UPROPERTY(VisibleAnywhere)
	FIntVector SavedWorldOrigin;
};

// 用于搜集每个关卡的LogicLodUnit
UCLASS()
class XD_LOGICLODSYSTEM_API UXD_LogicLodLevelCollection : public UAssetUserData
{
	GENERATED_BODY()
public:
	UPROPERTY(VisibleAnywhere)
	TArray<UXD_LogicLodUnitBase*> LogicLodUnits;

	UPROPERTY(NonPIEDuplicateTransient)
	UXD_LogicLodLevelBuiltData* LogicLodLevelBuiltData;

	UPROPERTY(VisibleAnywhere)
	FIntVector SavedWorldOrigin;
};

// 整个世界的LogicLod配置
UCLASS()
class XD_LOGICLODSYSTEM_API UXD_LogicLodWorldCollection : public UObject
{
	GENERATED_BODY()
public:
};

namespace FLogicLodSystemUtils
{
	XD_LOGICLODSYSTEM_API FName GetLevelName(const ULevel* Level);
}
