// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "XD_AI_LodUnitBase.h"
#include "XD_AI_LodInstanceInterface.h"

#define LOCTEXT_NAMESPACE "XD_AI_LodSystem"

void UXD_AI_LodUnitBase::SyncToInstance(ULevel* Level)
{
	// TODO：添加实例不存在时生成的逻辑
	// TODO: 提供实例在运行时被销毁的自动删除AI_LodUnit机制

	if (AActor* Instance = AI_Instance.Get())
	{
		check(Instance->Implements<UXD_AI_LodInstanceInterface>());

		IXD_AI_LodInstanceInterface::SyncToInstance(Instance, this);
	}
}

void UXD_AI_LodUnitBase::SyncToLodUnit()
{
	if (AActor* Instance = AI_Instance.Get())
	{
		check(Instance->Implements<UXD_AI_LodInstanceInterface>());

		IXD_AI_LodInstanceInterface::SyncToLodUnit(Instance, this);
	}
}

#undef LOCTEXT_NAMESPACE
