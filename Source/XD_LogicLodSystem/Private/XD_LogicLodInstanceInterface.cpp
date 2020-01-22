// Fill out your copyright notice in the Description page of Project Settings.


#include "XD_LogicLodInstanceInterface.h"
#include "XD_LogicLodUnitBase.h"

UXD_LogicLodUnitBase* IXD_LogicLodInstanceInterface::CreateLogicLodUnit(AActor* Actor, UObject* Outer)
{
#if WITH_EDITOR
	FEditorScriptExecutionGuard ScriptGuard;
#endif

	UXD_LogicLodUnitBase* LogicLodUnit = Execute_CreateLogicLodUnit((UObject*)Actor, Outer);
	LogicLodUnit->AI_Instance = Actor;
	LogicLodUnit->SyncToLodUnit();
	return LogicLodUnit;
}
