// Fill out your copyright notice in the Description page of Project Settings.


#include "XD_AI_LodInstanceInterface.h"
#include "XD_AI_LodUnitBase.h"

UXD_AI_LodUnitBase* IXD_AI_LodInstanceInterface::CreateAI_LodUnit(AActor* Actor, UObject* Outer)
{
#if WITH_EDITOR
	FEditorScriptExecutionGuard ScriptGuard;
#endif

	UXD_AI_LodUnitBase* AI_LodUnit = Execute_CreateAI_LodUnit((UObject*)Actor, Outer);
	AI_LodUnit->AI_Instance = Actor;
	AI_LodUnit->SyncToLodUnit();
	return AI_LodUnit;
}
