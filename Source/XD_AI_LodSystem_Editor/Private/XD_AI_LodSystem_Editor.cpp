// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "XD_AI_LodSystem_Editor.h"
#include <Editor.h>
#include <Engine/Level.h>
#include <Modules/ModuleManager.h>
#include <ISettingsModule.h>

#include "XD_AI_LodInstanceInterface.h"
#include "XD_AI_LodWorldCollection.h"
#include "XD_AI_LodSystemSettings.h"
#include "XD_AI_LodSystem_Log.h"

#define LOCTEXT_NAMESPACE "FXD_AI_LodSystem_EditorModule"

struct FBehaviorTreeInstantiatable;
struct FBehaviorTreeWithSubTree;

namespace XD_AI_LodSystemSettings
{
	const FName ContainerName = TEXT("Project");
	const FName CategoryName = TEXT("Plugins");
	const FName SectionName = TEXT("XD_AI_LodSystemSettings");
}

void FXD_AI_LodSystem_EditorModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->RegisterSettings(XD_AI_LodSystemSettings::ContainerName, XD_AI_LodSystemSettings::CategoryName, XD_AI_LodSystemSettings::SectionName,
			LOCTEXT("XD_AI_LodSystemSettings", "XD_AI_LodSystemSettings"),
			LOCTEXT("XD_AI_LodSystemSettingsDescription", "Configure the XD_AI_LodSystem plug-in."),
			GetMutableDefault<UXD_AI_LodSystemSettings>()
		);
	}

	PostSaveWorldHandle = FEditorDelegates::PostSaveWorld.AddLambda([this](uint32 SaveFlags, UWorld* World, bool bSuccess)
		{
			if (bSuccess)
			{
				CollectAI_LodUnit(World);
			}
		});
}

void FXD_AI_LodSystem_EditorModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	FEditorDelegates::PostSaveWorld.Remove(PostSaveWorldHandle);

	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->UnregisterSettings(XD_AI_LodSystemSettings::ContainerName, XD_AI_LodSystemSettings::CategoryName, XD_AI_LodSystemSettings::SectionName);
	}
}

void FXD_AI_LodSystem_EditorModule::CollectAI_LodUnit(UWorld* World)
{
	if (UXD_AI_LodWorldCollection* AI_LodWorldCollection = GetDefault<UXD_AI_LodSystemSettings>()->AI_LodWorldCollection.LoadSynchronous())
	{
		ULevel* Level = World->GetCurrentLevel();
		FName LevelFullName = Level->GetOutermost()->GetFName();
		TMap<FName, FXD_AI_LodLevelCollection>& AI_LodLevelCollections = AI_LodWorldCollection->AI_LodLevelCollections;
		FXD_AI_LodLevelCollection& AI_LodLevelCollection = AI_LodLevelCollections.FindOrAdd(LevelFullName);
		TArray<UXD_AI_LodUnitBase*>& AI_LodUnits = AI_LodLevelCollection.AI_LodUnits;
		AI_LodUnits.Empty();
		for (AActor* Actor : Level->Actors)
		{
			if (Actor && Actor->Implements<UXD_AI_LodInstanceInterface>())
			{
				UXD_AI_LodUnitBase* AI_LodUnit = IXD_AI_LodInstanceInterface::CreateAI_LodUnit(Actor, AI_LodWorldCollection);
				AI_LodUnits.Add(AI_LodUnit);
			}
		}

		AI_LodWorldCollection->Modify();
		AI_LodWorldCollection->MarkPackageDirty();

		XD_AI_LodSystem_Display_LOG("刷新关卡[%s]的AI_LodUnit配置", *LevelFullName.ToString());
	}
	else
	{
		XD_AI_LodSystem_Warning_LOG("未配置XD_AI_LodSystemSettings中的AI_LodWorldCollection");
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FXD_AI_LodSystem_EditorModule, XD_AI_LodSystem_Editor)