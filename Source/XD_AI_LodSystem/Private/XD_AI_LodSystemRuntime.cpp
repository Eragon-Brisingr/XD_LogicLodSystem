// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "XD_AI_LodSystemRuntime.h"
#include <Engine/GameInstance.h>
#include <Engine/World.h>
#include <Engine/Level.h>
#include <AssetRegistryModule.h>
#include <IAssetRegistry.h>
#include <Misc/PackageName.h>

#include "XD_AI_LodSystemSettings.h"
#include "XD_AI_LodWorldCollection.h"
#include "XD_AI_LodUnitBase.h"
#include "XD_AI_LodInstanceInterface.h"
#include "XD_SaveGameSystemBase.h"
#include "XD_AI_LodSystem_Log.h"

#define LOCTEXT_NAMESPACE "XD_AI_LodSystem"

void UXD_AI_LodSystemRuntime::WhenGameInit_Implementation()
{
	InitAI_LodSystem();
}

void UXD_AI_LodSystemRuntime::WhenPostLoad_Implementation()
{
	InitAI_LodSystem();
}

void UXD_AI_LodSystemRuntime::BeginPlay()
{
	Super::BeginPlay();

}

void UXD_AI_LodSystemRuntime::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (!IsServer())
	{
		return;
	}

	UXD_SaveGameSystemBase* SaveGameSystem = UXD_SaveGameSystemBase::Get(this);
	SaveGameSystem->OnLoadLevelCompleted.RemoveAll(this);
	SaveGameSystem->OnInitLevelCompleted.RemoveAll(this);
	SaveGameSystem->OnPreLevelUnload.RemoveAll(this);

	UWorld* World = GetWorld();
	World->RemoveOnActorSpawnedHandler(OnActorSpawnedHandler);
}

void UXD_AI_LodSystemRuntime::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!IsServer())
	{
		return;
	}

	for (TPair<FName, FXD_AI_LodLevelUnit>& Pair : AI_LodLevelUnits)
	{
		FXD_AI_LodLevelUnit& AI_LodLevelUnit = Pair.Value;
		if (AI_LodLevelUnit.bIsLevelLoaded == false)
		{
			for (UXD_AI_LodUnitBase* AI_LodUnit : AI_LodLevelUnit.AI_LodUnits)
			{
				AI_LodUnit->Tick(DeltaTime);
			}
		}
	}
}

void UXD_AI_LodSystemRuntime::InitAI_LodSystem()
{
	UWorld* World = GetWorld();

	XD_AI_LodSystem_Display_LOG("初始化AI_LodSystem");
	{
		FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));
		IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();
		TArray<FAssetData> AI_LodLevelBuiltDataAssets;
		AssetRegistry.GetAssetsByClass(UXD_AI_LodLevelBuiltData::StaticClass()->GetFName(), AI_LodLevelBuiltDataAssets);
		for (const FAssetData& AI_LodLevelBuiltDataAsset : AI_LodLevelBuiltDataAssets)
		{
			UXD_AI_LodLevelBuiltData* AI_LodLevelBuiltData = CastChecked<UXD_AI_LodLevelBuiltData>(AI_LodLevelBuiltDataAsset.GetAsset());
			FString LevelName = FPackageName::GetShortFName(AI_LodLevelBuiltData->GetOutermost()->GetName()).ToString();
			// 17为_AI_LodBuiltData的长度
			constexpr int32 Len_AI_LodBuiltData = 17;
			LevelName = LevelName.Left(LevelName.Len() - Len_AI_LodBuiltData + 1);
			FXD_AI_LodLevelUnit& AI_LodLevelUnit = AI_LodLevelUnits.Add(*LevelName);
			for (UXD_AI_LodUnitBase* AI_LodUnitTemplate : AI_LodLevelBuiltData->AI_LodUnits)
			{
				UXD_AI_LodUnitBase* AI_LodUnit = ::DuplicateObject(AI_LodUnitTemplate, this, AI_LodUnitTemplate->GetFName());
				AI_LodLevelUnit.AI_LodUnits.Add(AI_LodUnit);
			}
			XD_AI_LodSystem_Display_LOG("  注册关卡[%s]进AI_Lod系统", *LevelName);
		}
	}

	UXD_SaveGameSystemBase* SaveGameSystem = UXD_SaveGameSystemBase::Get(this);
	SaveGameSystem->OnInitLevelCompleted.AddUObject(this, &UXD_AI_LodSystemRuntime::WhenLevelInited);
	SaveGameSystem->OnLoadLevelCompleted.AddUObject(this, &UXD_AI_LodSystemRuntime::WhenLevelLoaded);
	SaveGameSystem->OnPreLevelUnload.AddUObject(this, &UXD_AI_LodSystemRuntime::WhenLevelPreUnload);
	OnActorSpawnedHandler = World->AddOnActorSpawnedHandler(FOnActorSpawned::FDelegate::CreateUObject(this, &UXD_AI_LodSystemRuntime::WhenActorSpawned));
}

void UXD_AI_LodSystemRuntime::WhenLevelInited(ULevel* Level)
{
	if (UXD_AI_LodLevelCollection* AI_LodLevelCollection = Level->GetAssetUserData<UXD_AI_LodLevelCollection>())
	{
		FName LevelName = FAI_LodSystemUtils::GetLevelName(Level);
		FXD_AI_LodLevelUnit& AI_LodLevelUnit = AI_LodLevelUnits.FindOrAdd(LevelName);
		for (UXD_AI_LodUnitBase* AI_LodUnitTemplate : AI_LodLevelCollection->AI_LodUnits)
		{
			UXD_AI_LodUnitBase* AI_LodUnit = ::DuplicateObject(AI_LodUnitTemplate, this, AI_LodUnitTemplate->GetFName());
			AI_LodLevelUnit.AI_LodUnits.Add(AI_LodUnit);
		}
		if (AI_LodLevelCollection->AI_LodUnits.Num() > 0)
		{
			XD_AI_LodSystem_Display_LOG("关卡[%s]初始化，注册AI_Lod系统", *LevelName.ToString());
		}
	}

	WhenLevelLoaded(Level);
}

void UXD_AI_LodSystemRuntime::WhenLevelLoaded(ULevel* Level)
{
	check(Level->GetWorld()->IsServer());

	FName LevelName = FAI_LodSystemUtils::GetLevelName(Level);
	if (FXD_AI_LodLevelUnit* AI_LodLevelUnit = AI_LodLevelUnits.Find(LevelName))
	{
		FEditorScriptExecutionGuard ScriptGuard;
		AI_LodLevelUnit->bIsLevelLoaded = true;
		XD_AI_LodSystem_Display_LOG("关卡[%s]完成加载，更新的所有AI_LodUnit至实例", *LevelName.ToString());

		for (UXD_AI_LodUnitBase* AI_LodUnit : AI_LodLevelUnit->AI_LodUnits)
		{
			AI_LodUnit->SyncToInstance(Level);
			AActor* Instance = AI_LodUnit->AI_Instance.Get();
			check(Instance);
			Instance->OnDestroyed.AddDynamic(this, &UXD_AI_LodSystemRuntime::WhenInstanceDestroyed);
		}
	}
}

void UXD_AI_LodSystemRuntime::WhenLevelPreUnload(ULevel* Level)
{
	check(Level->GetWorld()->IsServer());

	FName LevelName = FAI_LodSystemUtils::GetLevelName(Level);
	if (FXD_AI_LodLevelUnit* AI_LodLevelUnit = AI_LodLevelUnits.Find(LevelName))
	{
		FEditorScriptExecutionGuard ScriptGuard;
		AI_LodLevelUnit->bIsLevelLoaded = false;
		XD_AI_LodSystem_Display_LOG("关卡[%s]将要卸载，更新的所有实例至AI_LodUnit", *LevelName.ToString());

		for (UXD_AI_LodUnitBase* AI_LodUnit : AI_LodLevelUnit->AI_LodUnits)
		{
			AI_LodUnit->SyncToLodUnit();
			AActor* Instance = AI_LodUnit->AI_Instance.Get();
			check(Instance);
			Instance->OnDestroyed.RemoveDynamic(this, &UXD_AI_LodSystemRuntime::WhenInstanceDestroyed);
		}
	}
}

void UXD_AI_LodSystemRuntime::WhenActorSpawned(AActor* Actor)
{
	if (bAutoRegisterSpawnedActor && Actor->Implements<UXD_AI_LodInstanceInterface>())
	{
		ULevel* Level = Actor->GetLevel();
		FName LevelName = FAI_LodSystemUtils::GetLevelName(Level);
		FXD_AI_LodLevelUnit& AI_LodLevelUnit = AI_LodLevelUnits.FindOrAdd(LevelName);
		if (AI_LodLevelUnit.bIsLevelLoaded)
		{
			check(!AI_LodLevelUnit.AI_LodUnits.ContainsByPredicate([&](UXD_AI_LodUnitBase* E) {return E->AI_Instance.Get() == Actor; }));
			UXD_AI_LodUnitBase* AI_LodUnit = IXD_AI_LodInstanceInterface::CreateAI_LodUnit(Actor, this);
			AI_LodLevelUnit.AI_LodUnits.Add(AI_LodUnit);
			XD_AI_LodSystem_Display_LOG("AI实例[%s]实例化，自动注册进AI_Lod系统的关卡[%s]", *Actor->GetName(), *LevelName.ToString());
			Actor->OnDestroyed.AddDynamic(this, &UXD_AI_LodSystemRuntime::WhenInstanceDestroyed);
		}
	}
}

void UXD_AI_LodSystemRuntime::WhenInstanceDestroyed(AActor* Actor)
{
	ULevel* Level = Actor->GetLevel();
	FName LevelName = FAI_LodSystemUtils::GetLevelName(Level);
	FXD_AI_LodLevelUnit* AI_LodLevelUnit = AI_LodLevelUnits.Find(LevelName);
	int32 RemoveNum = AI_LodLevelUnit->AI_LodUnits.RemoveAll([&](UXD_AI_LodUnitBase* AI_LodUnit) {return AI_LodUnit->AI_Instance.Get() == Actor; });
	check(RemoveNum > 0);
	XD_AI_LodSystem_Display_LOG("AI实例[%s]被销毁，自动从AI_Lod系统的关卡[%s]反注册", *Actor->GetName(), *LevelName.ToString());
}

bool UXD_AI_LodSystemRuntime::bAutoRegisterSpawnedActor = true;

#undef LOCTEXT_NAMESPACE
