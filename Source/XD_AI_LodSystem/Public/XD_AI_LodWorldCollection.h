// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include <UObject/NoExportTypes.h>
#include <Engine/AssetUserData.h>
#include "XD_AI_LodWorldCollection.generated.h"

class UXD_AI_LodUnitBase;

// 世界初始化时就要进行计算的，用于搜集每个关卡的AI_LodUnit
UCLASS()
class XD_AI_LODSYSTEM_API UXD_AI_LodLevelBuiltData : public UObject
{
	GENERATED_BODY()
public:
	UPROPERTY(VisibleAnywhere)
	TArray<UXD_AI_LodUnitBase*> AI_LodUnits;

	UPROPERTY(VisibleAnywhere)
	FIntVector SavedWorldOrigin;
};

// 用于搜集每个关卡的AI_LodUnit
UCLASS()
class XD_AI_LODSYSTEM_API UXD_AI_LodLevelCollection : public UAssetUserData
{
	GENERATED_BODY()
public:
	UPROPERTY(VisibleAnywhere)
	TArray<UXD_AI_LodUnitBase*> AI_LodUnits;

	UPROPERTY(NonPIEDuplicateTransient)
	UXD_AI_LodLevelBuiltData* AI_LodLevelBuiltData;

	UPROPERTY(VisibleAnywhere)
	FIntVector SavedWorldOrigin;
};

// 整个世界的AI_Lod配置
UCLASS()
class XD_AI_LODSYSTEM_API UXD_AI_LodWorldCollection : public UObject
{
	GENERATED_BODY()
public:
};

namespace FAI_LodSystemUtils
{
	XD_AI_LODSYSTEM_API FName GetLevelName(const ULevel* Level);
}
