// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include <UObject/NoExportTypes.h>
#include "XD_AI_LodUnitBase.generated.h"

UCLASS()
class XD_AI_LODSYSTEM_API UXD_AI_LodUnitBase : public UObject
{
	GENERATED_BODY()

	friend class UXD_AI_LodSystemRuntime;
public:
	UPROPERTY()
	TSoftObjectPtr<AActor> AI_Instance;

protected:
	virtual void SyncToInstance(ULevel* Level);
	virtual void SyncToLodUnit();
	virtual void Tick(float DeltaTime) {}
};
