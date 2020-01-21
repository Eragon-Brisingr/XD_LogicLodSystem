// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include <UObject/NoExportTypes.h>
#include <UObject/SoftObjectPtr.h>
#include "XD_AI_LodSystemSettings.generated.h"

class UXD_AI_LodWorldCollection;

UCLASS(Config = "AI_LodSystemSettings", defaultconfig)
class XD_AI_LODSYSTEM_API UXD_AI_LodSystemSettings : public UObject
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, Config)
	FString ValidLevelPattern;
};
