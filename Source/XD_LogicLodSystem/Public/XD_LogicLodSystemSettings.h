// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include <UObject/NoExportTypes.h>
#include <UObject/SoftObjectPtr.h>
#include "XD_LogicLodSystemSettings.generated.h"

class UXD_LogicLodWorldCollection;
class UXD_LogicLodLevelUnit;

UCLASS(Config = "LogicLodSystemSettings", defaultconfig)
class XD_LOGICLODSYSTEM_API UXD_LogicLodSystemSettings : public UObject
{
	GENERATED_BODY()
public:
	UXD_LogicLodSystemSettings();

	UPROPERTY(EditAnywhere, Config)
	FString ValidLevelPattern;

	UPROPERTY(EditAnywhere, Config)
	TSoftClassPtr<UXD_LogicLodLevelUnit> LogicLodLevelUnitClass;
};
