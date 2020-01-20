// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include <UObject/NoExportTypes.h>
#include "XD_AI_LodWorldCollection.generated.h"

class UXD_AI_LodUnitBase;

USTRUCT()
struct XD_AI_LODSYSTEM_API FXD_AI_LodLevelCollection
{
	GENERATED_BODY()
public:
	UPROPERTY(VisibleAnywhere)
	TArray<UXD_AI_LodUnitBase*> AI_LodUnits;
};

UCLASS()
class XD_AI_LODSYSTEM_API UXD_AI_LodWorldCollection : public UObject
{
	GENERATED_BODY()
public:
    UPROPERTY(VisibleAnywhere)
	TMap<FName, FXD_AI_LodLevelCollection> AI_LodLevelCollections;
};
