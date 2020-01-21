// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "XD_AI_LodSystem_Editor.h"
#include <Editor.h>
#include <Engine/Level.h>
#include <Modules/ModuleManager.h>
#include <ISettingsModule.h>
#include <Internationalization/Regex.h>

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

	PostSaveWorldHandle = FEditorDelegates::PreSaveWorld.AddLambda([this](uint32 SaveFlags, UWorld* World)
		{
			CollectAI_LodUnit(World);
		});
}

void FXD_AI_LodSystem_EditorModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	FEditorDelegates::PreSaveWorld.Remove(PostSaveWorldHandle);

	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->UnregisterSettings(XD_AI_LodSystemSettings::ContainerName, XD_AI_LodSystemSettings::CategoryName, XD_AI_LodSystemSettings::SectionName);
	}
}

void FXD_AI_LodSystem_EditorModule::CollectAI_LodUnit(UWorld* World)
{
	const UXD_AI_LodSystemSettings* AI_LodSystemSettings = GetDefault<UXD_AI_LodSystemSettings>();
	ULevel* Level = World->GetCurrentLevel();
	if (AI_LodSystemSettings->ValidLevelPattern.Len() > 0 && FRegexMatcher(AI_LodSystemSettings->ValidLevelPattern, Level->GetOutermost()->GetPathName()).FindNext() == false)
	{
		return;
	}

	UXD_AI_LodLevelCollection* AI_LodLevelCollection = Level->GetAssetUserData<UXD_AI_LodLevelCollection>();
	if (AI_LodLevelCollection == nullptr)
	{
		AI_LodLevelCollection = NewObject<UXD_AI_LodLevelCollection>(Level);
		Level->AddAssetUserData(AI_LodLevelCollection);
	}

	AI_LodLevelCollection->SavedWorldOrigin = World->OriginLocation;

	TArray<UXD_AI_LodUnitBase*> WorldInitAI_LodUnits;
	TArray<UXD_AI_LodUnitBase*> LevelInitAI_LodUnits;
	for (AActor* Actor : Level->Actors)
	{
		if (Actor && Actor->Implements<UXD_AI_LodInstanceInterface>())
		{
			switch (IXD_AI_LodInstanceInterface::GetRegisterType(Actor))
			{
			case EAI_LodSystemRegisterType::LevelInit:
			{
				if (AI_LodLevelCollection->AI_LodLevelBuiltData == nullptr)
				{
					FString PackageName = Level->GetOutermost()->GetName() + TEXT("_AI_LodData");
					UPackage* BuiltDataPackage = CreatePackage(nullptr, *PackageName);
					// PKG_ContainsMapData required so FEditorFileUtils::GetDirtyContentPackages can treat this as a map package
					BuiltDataPackage->SetPackageFlags(PKG_ContainsMapData);
					FName ShortPackageName = FPackageName::GetShortFName(BuiltDataPackage->GetFName());
					// Top level UObjects have to have both RF_Standalone and RF_Public to be saved into packages
					AI_LodLevelCollection->AI_LodLevelBuiltData = NewObject<UXD_AI_LodLevelBuiltData>(BuiltDataPackage, ShortPackageName, RF_Standalone | RF_Public);
					AI_LodLevelCollection->AI_LodLevelBuiltData->SavedWorldOrigin = World->OriginLocation;
				}
				UXD_AI_LodUnitBase* AI_LodUnit = IXD_AI_LodInstanceInterface::CreateAI_LodUnit(Actor, AI_LodLevelCollection->AI_LodLevelBuiltData);
				WorldInitAI_LodUnits.Add(AI_LodUnit);
				break;
			}
			case EAI_LodSystemRegisterType::WorldInit:
			{
				UXD_AI_LodUnitBase* AI_LodUnit = IXD_AI_LodInstanceInterface::CreateAI_LodUnit(Actor, AI_LodLevelCollection);
				LevelInitAI_LodUnits.Add(AI_LodUnit);
				break;
			}
			default:
				check(0);
				break;
			}
		}
	}

	AI_LodLevelCollection->AI_LodUnits = LevelInitAI_LodUnits;
	AI_LodLevelCollection->AI_LodLevelBuiltData->AI_LodUnits = WorldInitAI_LodUnits;
	AI_LodLevelCollection->AI_LodLevelBuiltData->MarkPackageDirty();

	FName LevelName = FAI_LodSystemUtils::GetLevelName(Level);
	XD_AI_LodSystem_Display_LOG("刷新关卡[%s]的AI_LodUnit配置", *LevelName.ToString());
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FXD_AI_LodSystem_EditorModule, XD_AI_LodSystem_Editor)