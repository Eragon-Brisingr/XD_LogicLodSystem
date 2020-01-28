// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "XD_LogicLodSystem_Editor.h"
#include <Editor.h>
#include <Engine/Level.h>
#include <Modules/ModuleManager.h>
#include <ISettingsModule.h>
#include <Internationalization/Regex.h>
#include <AssetRegistryModule.h>

#include "XD_LogicLodInstanceInterface.h"
#include "XD_LogicLodWorldCollection.h"
#include "XD_LogicLodSystemSettings.h"
#include "XD_LogicLodSystem_Utils.h"

#define LOCTEXT_NAMESPACE "FXD_LogicLodSystem_EditorModule"

struct FBehaviorTreeInstantiatable;
struct FBehaviorTreeWithSubTree;

namespace XD_LogicLodSystemSettings
{
	const FName ContainerName = TEXT("Project");
	const FName CategoryName = TEXT("Plugins");
	const FName SectionName = TEXT("XD_LogicLodSystemSettings");
}

void FXD_LogicLodSystem_EditorModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->RegisterSettings(XD_LogicLodSystemSettings::ContainerName, XD_LogicLodSystemSettings::CategoryName, XD_LogicLodSystemSettings::SectionName,
			LOCTEXT("XD_LogicLodSystemSettings", "XD_LogicLodSystemSettings"),
			LOCTEXT("XD_LogicLodSystemSettingsDescription", "Configure the XD_LogicLodSystem plug-in."),
			GetMutableDefault<UXD_LogicLodSystemSettings>()
		);
	}

	PostSaveWorldHandle = FEditorDelegates::PreSaveWorld.AddLambda([this](uint32 SaveFlags, UWorld* World)
		{
			// 防止打包时报错
			constexpr ESaveFlags PackageFlags = ESaveFlags(SAVE_KeepGUID | SAVE_Async | SAVE_Unversioned | SAVE_ComputeHash);
			if (SaveFlags != PackageFlags)
			{
				CollectLogicLodUnit(World);
			}
		});
}

void FXD_LogicLodSystem_EditorModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	FEditorDelegates::PreSaveWorld.Remove(PostSaveWorldHandle);

	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->UnregisterSettings(XD_LogicLodSystemSettings::ContainerName, XD_LogicLodSystemSettings::CategoryName, XD_LogicLodSystemSettings::SectionName);
	}
}

void FXD_LogicLodSystem_EditorModule::CollectLogicLodUnit(UWorld* World)
{
	const UXD_LogicLodSystemSettings* LogicLodSystemSettings = GetDefault<UXD_LogicLodSystemSettings>();
	ULevel* Level = World->GetCurrentLevel();
	if (LogicLodSystemSettings->ValidLevelPattern.Len() > 0 && FRegexMatcher(LogicLodSystemSettings->ValidLevelPattern, Level->GetOutermost()->GetPathName()).FindNext() == false)
	{
		return;
	}

	UXD_LogicLodLevelCollection* LogicLodLevelCollection = Level->GetAssetUserData<UXD_LogicLodLevelCollection>();
	if (LogicLodLevelCollection == nullptr)
	{
		LogicLodLevelCollection = NewObject<UXD_LogicLodLevelCollection>(Level);
	}

	LogicLodLevelCollection->SavedWorldOrigin = World->OriginLocation;

	TArray<UXD_LogicLodUnitBase*> WorldInitLogicLodUnits;
	TArray<UXD_LogicLodUnitBase*> LevelInitLogicLodUnits;
	for (AActor* Actor : Level->Actors)
	{
		if (Actor && Actor->Implements<UXD_LogicLodInstanceInterface>())
		{
			switch (IXD_LogicLodInstanceInterface::GetLogicLodRegisterType(Actor))
			{
			case ELogicLodSystemRegisterType::LevelInit:
			{
				if (LogicLodLevelCollection->LogicLodLevelBuiltData == nullptr)
				{
					const FString PackagePath = Level->GetOutermost()->GetName() + TEXT("_LogicLodData");
					UPackage* BuiltDataPackage = CreatePackage(nullptr, *PackagePath);
					// PKG_ContainsMapData required so FEditorFileUtils::GetDirtyContentPackages can treat this as a map package
					BuiltDataPackage->SetPackageFlags(PKG_ContainsMapData);
					FName ShortPackageName = FPackageName::GetShortFName(BuiltDataPackage->GetFName());
					// Top level UObjects have to have both RF_Standalone and RF_Public to be saved into packages
					UXD_LogicLodLevelBuiltData* LogicLodLevelBuiltData = NewObject<UXD_LogicLodLevelBuiltData>(BuiltDataPackage, ShortPackageName, RF_Standalone | RF_Public);
					LogicLodLevelCollection->LogicLodLevelBuiltData = LogicLodLevelBuiltData;
					LogicLodLevelBuiltData->SavedWorldOrigin = World->OriginLocation;

					FAssetRegistryModule::AssetCreated(LogicLodLevelBuiltData);
				}
				UXD_LogicLodUnitBase* LogicLodUnit = IXD_LogicLodInstanceInterface::CreateLogicLodUnit(Actor, LogicLodLevelCollection->LogicLodLevelBuiltData);
				WorldInitLogicLodUnits.Add(LogicLodUnit);
				break;
			}
			case ELogicLodSystemRegisterType::WorldInit:
			{
				UXD_LogicLodUnitBase* LogicLodUnit = IXD_LogicLodInstanceInterface::CreateLogicLodUnit(Actor, LogicLodLevelCollection);
				LevelInitLogicLodUnits.Add(LogicLodUnit);
				break;
			}
			default:
				check(0);
				break;
			}
		}
	}

	LogicLodLevelCollection->LogicLodUnits = LevelInitLogicLodUnits;
	if (LogicLodLevelCollection->LogicLodLevelBuiltData)
	{
		LogicLodLevelCollection->LogicLodLevelBuiltData->LogicLodUnits = WorldInitLogicLodUnits;
		LogicLodLevelCollection->LogicLodLevelBuiltData->MarkPackageDirty();
	}

	if (LevelInitLogicLodUnits.Num() > 0 || LogicLodLevelCollection->LogicLodLevelBuiltData != nullptr)
	{
		Level->AddAssetUserData(LogicLodLevelCollection);
	}
	else
	{
		Level->RemoveUserDataOfClass(UXD_LogicLodLevelCollection::StaticClass());
	}

	FName LevelName = FLogicLodSystemUtils::GetLevelName(Level);
	XD_LogicLodSystem_Display_LOG("刷新关卡[%s]的LogicLodUnit配置", *LevelName.ToString());
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FXD_LogicLodSystem_EditorModule, XD_LogicLodSystem_Editor)