// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "XD_LogicLodUnitBase.h"
#include "XD_LogicLodInstanceInterface.h"
#include "XD_LogicLodSystemRuntime.h"

#define LOCTEXT_NAMESPACE "XD_LogicLodSystem"

UWorld* UXD_LogicLodLevelUnit::GetWorld() const
{
	return GetOuter()->GetWorld();
}

void UXD_LogicLodUnitBase::SyncToInstance(ULevel* Level, bool IsInit)
{
	unimplemented();
}

void UXD_LogicLodUnitBase::SyncToLodUnit()
{
	unimplemented();
}

UWorld* UXD_LogicLodUnitBase::GetWorld() const
{
	return LogicLodLevelUnit->GetWorld();
}

#undef LOCTEXT_NAMESPACE
