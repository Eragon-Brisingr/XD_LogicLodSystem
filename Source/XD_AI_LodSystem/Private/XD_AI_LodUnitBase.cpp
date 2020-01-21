// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "XD_AI_LodUnitBase.h"
#include "XD_AI_LodInstanceInterface.h"
#include "XD_AI_LodSystemRuntime.h"

#define LOCTEXT_NAMESPACE "XD_AI_LodSystem"

void UXD_AI_LodUnitBase::SyncToInstance(ULevel* Level, bool IsInit, const FXD_AI_LodLevelUnit& AI_LodLevelUnit)
{
	unimplemented();
}

void UXD_AI_LodUnitBase::SyncToLodUnit()
{
	unimplemented();
}

UWorld* UXD_AI_LodUnitBase::GetWorld() const
{
	return LodSystemRuntime->GetWorld();
}

#undef LOCTEXT_NAMESPACE
