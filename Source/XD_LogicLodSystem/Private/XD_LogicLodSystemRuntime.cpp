// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "XD_LogicLodSystemRuntime.h"
#include <Engine/GameInstance.h>
#include <Engine/World.h>
#include <Engine/Level.h>
#include <AssetRegistryModule.h>
#include <IAssetRegistry.h>
#include <Misc/PackageName.h>
#include <Internationalization/Regex.h>


#include "GameSerializerManager.h"
#include "XD_LogicLodSystemSettings.h"
#include "XD_LogicLodWorldCollection.h"
#include "XD_LogicLodUnitBase.h"
#include "XD_LogicLodInstanceInterface.h"
#include "XD_LogicLodSystem_Utils.h"

#define LOCTEXT_NAMESPACE "XD_LogicLodSystem"

UXD_LogicLodSystemRuntime::UXD_LogicLodSystemRuntime()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UXD_LogicLodSystemRuntime::WhenGameInit_Implementation()
{
	XD_LogicLodSystem_Display_LOG("初始化LogicLodSystem");
	{
		FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));
		IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();
		TArray<FAssetData> LogicLodLevelBuiltDataAssets;
		AssetRegistry.GetAssetsByClass(UXD_LogicLodLevelBuiltData::StaticClass()->GetFName(), LogicLodLevelBuiltDataAssets);
		const UXD_LogicLodSystemSettings* LogicLodSystemSettings = GetDefault<UXD_LogicLodSystemSettings>();
		for (const FAssetData& LogicLodLevelBuiltDataAsset : LogicLodLevelBuiltDataAssets)
		{
			UXD_LogicLodLevelBuiltData* LogicLodLevelBuiltData = CastChecked<UXD_LogicLodLevelBuiltData>(LogicLodLevelBuiltDataAsset.GetAsset());
			if (LogicLodSystemSettings->ValidLevelPattern.Len() > 0 && FRegexMatcher(LogicLodSystemSettings->ValidLevelPattern, LogicLodLevelBuiltDataAsset.PackageName.ToString()).FindNext() == false)
			{
				continue;
			}
			FString LevelName = FPackageName::GetShortFName(LogicLodLevelBuiltData->GetOutermost()->GetName()).ToString();
			// 13为_LogicLodData的长度
			constexpr int32 Len_LogicLodData = 13;
			LevelName = LevelName.Left(LevelName.Len() - Len_LogicLodData);
			UXD_LogicLodLevelUnit*& LogicLodLevelUnit = LogicLodLevelUnits.Add(*LevelName);
			LogicLodLevelUnit = NewObject<UXD_LogicLodLevelUnit>(this, GetDefault<UXD_LogicLodSystemSettings>()->LogicLodLevelUnitClass.LoadSynchronous(), *LevelName);
			LogicLodLevelUnit->SavedWorldOrigin = LogicLodLevelBuiltData->SavedWorldOrigin;
			for (UXD_LogicLodUnitBase* LogicLodUnitTemplate : LogicLodLevelBuiltData->LogicLodUnits)
			{
				UXD_LogicLodUnitBase* LogicLodUnit = DuplicateLogicLodUnit(LogicLodUnitTemplate, LogicLodLevelUnit);
				LogicLodLevelUnit->LogicLodUnits.Add(LogicLodUnit);
			}
			XD_LogicLodSystem_Display_LOG("  注册关卡[%s]进LogicLod系统", *LevelName);
		}
	}

	RegisterLogicLodSystem();
}

void UXD_LogicLodSystemRuntime::WhenGamePostLoad_Implementation(const FGameSerializerExtendDataContainer& ExtendData, const FGameSerializerCallRepNotifyFunc& CallRepNotifyFunc)
{
	DefaultWhenGamePostLoad(ExtendData);
	CallRepNotifyFunc.CallRepNotifyFunc();
	RegisterLogicLodSystem();
}

 FGameSerializerExtendDataContainer UXD_LogicLodSystemRuntime::WhenGamePreSave_Implementation()
{
	for (TPair<FName, UXD_LogicLodLevelUnit*>& Pair : LogicLodLevelUnits)
	{
		UXD_LogicLodLevelUnit* LogicLodLevelUnit = Pair.Value;
		if (LogicLodLevelUnit->bIsLevelLoaded)
		{
			for (UXD_LogicLodUnitBase* LogicLodUnit : LogicLodLevelUnit->LogicLodUnits)
			{
				LogicLodUnit->SyncToLodUnit();
			}
		}
	}
	return FGameSerializerExtendDataContainer();
}

void UXD_LogicLodSystemRuntime::BeginPlay()
{
	Super::BeginPlay();

	SetComponentTickEnabled(IsServer());
}

void UXD_LogicLodSystemRuntime::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (!IsServer())
	{
		return;
	}

	UGameSerializerManager* GameSerializerManager = GetWorld()->GetGameInstance()->GetSubsystem<UGameSerializerManager>();
	GameSerializerManager->OnLevelInitedNative.RemoveAll(this);
	GameSerializerManager->OnLevelLoadedNative.RemoveAll(this);
	GameSerializerManager->OnLevelPreSaveNative.RemoveAll(this);

	UWorld* World = GetWorld();
	World->RemoveOnActorSpawnedHandler(OnActorSpawnedHandler);
}

DECLARE_CYCLE_STAT(TEXT("LogicLodUpdate"), STAT_LogicLodUpdate, STATGROUP_LOGIC_LOD);
void UXD_LogicLodSystemRuntime::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	SCOPE_CYCLE_COUNTER(STAT_LogicLodUpdate);
	for (TPair<FName, UXD_LogicLodLevelUnit*>& Pair : LogicLodLevelUnits)
	{
		UXD_LogicLodLevelUnit* LogicLodLevelUnit = Pair.Value;
		LogicLodLevelUnit->Tick(DeltaTime);
		if (LogicLodLevelUnit->bIsLevelLoaded == false)
		{
			for (UXD_LogicLodUnitBase* LogicLodUnit : LogicLodLevelUnit->LogicLodUnits)
			{
				LogicLodUnit->Tick(DeltaTime);
			}
		}
	}
}

void UXD_LogicLodSystemRuntime::RegisterLogicLodSystem()
{
	UWorld* World = GetWorld();

	UGameSerializerManager* GameSerializerManager = GetWorld()->GetGameInstance()->GetSubsystem<UGameSerializerManager>();
	GameSerializerManager->OnLevelInitedNative.AddUObject(this, &UXD_LogicLodSystemRuntime::WhenLevelInited);
	GameSerializerManager->OnLevelLoadedNative.AddUObject(this, &UXD_LogicLodSystemRuntime::WhenLevelLoaded);
	GameSerializerManager->OnLevelPreSaveNative.AddUObject(this, &UXD_LogicLodSystemRuntime::WhenLevelPreSave);
	OnActorSpawnedHandler = World->AddOnActorSpawnedHandler(FOnActorSpawned::FDelegate::CreateUObject(this, &UXD_LogicLodSystemRuntime::WhenActorSpawned));

	XD_LogicLodSystem_Display_LOG("LogicLodSystem完成注册");
}

void UXD_LogicLodSystemRuntime::WhenLevelInited(ULevel* Level)
{
	check(Level->GetWorld()->IsServer());

	const UXD_LogicLodSystemSettings* LogicLodSystemSettings = GetDefault<UXD_LogicLodSystemSettings>();
	if (LogicLodSystemSettings->ValidLevelPattern.Len() > 0 && FRegexMatcher(LogicLodSystemSettings->ValidLevelPattern, Level->GetOuter()->GetFullName()).FindNext() == false)
	{
		return;
	}

	if (UXD_LogicLodLevelCollection* LogicLodLevelCollection = Level->GetAssetUserData<UXD_LogicLodLevelCollection>())
	{
		FName LevelName = FLogicLodSystemUtils::GetLevelName(Level);
		UXD_LogicLodLevelUnit*& LogicLodLevelUnit = LogicLodLevelUnits.FindOrAdd(LevelName);
		if (LogicLodLevelUnit == nullptr)
		{
			LogicLodLevelUnit = NewObject<UXD_LogicLodLevelUnit>(this, GetDefault<UXD_LogicLodSystemSettings>()->LogicLodLevelUnitClass.LoadSynchronous(), LevelName);
		}
		LogicLodLevelUnit->SavedWorldOrigin = LogicLodLevelCollection->SavedWorldOrigin;
		for (UXD_LogicLodUnitBase* LogicLodUnitTemplate : LogicLodLevelCollection->LogicLodUnits)
		{
			UXD_LogicLodUnitBase* LogicLodUnit = DuplicateLogicLodUnit(LogicLodUnitTemplate, LogicLodLevelUnit);
			LogicLodLevelUnit->LogicLodUnits.Add(LogicLodUnit);
		}
		if (LogicLodLevelCollection->LogicLodUnits.Num() > 0)
		{
			XD_LogicLodSystem_Display_LOG("关卡[%s]初始化，注册LogicLod系统", *LevelName.ToString());
		}
	}

	SyncLevelUnitToInstance(Level, true);
}

void UXD_LogicLodSystemRuntime::WhenLevelLoaded(ULevel* Level)
{
	check(Level->GetWorld()->IsServer());

	for (const TPair<FName, UXD_LogicLodLevelUnit*>& Pair : LogicLodLevelUnits)
	{
		for (UXD_LogicLodUnitBase* LogicLodUnit : Pair.Value->LogicLodUnits)
		{
			LogicLodUnit->LogicLodLevelUnit = Pair.Value;
		}
	}

	SyncLevelUnitToInstance(Level, false);
}

void UXD_LogicLodSystemRuntime::SyncLevelUnitToInstance(ULevel* Level, bool IsInit)
{
	FName LevelName = FLogicLodSystemUtils::GetLevelName(Level);
	if (UXD_LogicLodLevelUnit** P_LogicLodLevelUnit = LogicLodLevelUnits.Find(LevelName))
	{
		UXD_LogicLodLevelUnit* LogicLodLevelUnit = *P_LogicLodLevelUnit;
		FEditorScriptExecutionGuard ScriptGuard;
		LogicLodLevelUnit->bIsLevelLoaded = true;
		XD_LogicLodSystem_Display_LOG("关卡[%s]完成加载，更新的所有LogicLodUnit至实例", *LevelName.ToString());

		for (UXD_LogicLodUnitBase* LogicLodUnit : LogicLodLevelUnit->LogicLodUnits)
		{
			LogicLodUnit->SyncToInstance(Level, IsInit);
			AActor* Instance = LogicLodUnit->AI_Instance.Get();
			check(Instance && !Instance->OnDestroyed.Contains(this, GET_FUNCTION_NAME_CHECKED(UXD_LogicLodSystemRuntime, WhenInstanceDestroyed)));
			Instance->OnDestroyed.AddDynamic(this, &UXD_LogicLodSystemRuntime::WhenInstanceDestroyed);
		}
	}
}

void UXD_LogicLodSystemRuntime::WhenLevelPreSave(ULevel* Level)
{
	check(Level->GetWorld()->IsServer());

	UWorld* World = Level->GetWorld();
	FName LevelName = FLogicLodSystemUtils::GetLevelName(Level);
	if (UXD_LogicLodLevelUnit** P_LogicLodLevelUnit = LogicLodLevelUnits.Find(LevelName))
	{
		UXD_LogicLodLevelUnit* LogicLodLevelUnit = *P_LogicLodLevelUnit;
		FEditorScriptExecutionGuard ScriptGuard;
		LogicLodLevelUnit->bIsLevelLoaded = false;
		LogicLodLevelUnit->SavedWorldOrigin = World->OriginLocation;
		XD_LogicLodSystem_Display_LOG("关卡[%s]将要保存，更新的所有实例至LogicLodUnit", *LevelName.ToString());

		for (UXD_LogicLodUnitBase* LogicLodUnit : LogicLodLevelUnit->LogicLodUnits)
		{
			LogicLodUnit->SyncToLodUnit();
			AActor* Instance = LogicLodUnit->AI_Instance.Get();
			check(Instance);
			Instance->OnDestroyed.RemoveDynamic(this, &UXD_LogicLodSystemRuntime::WhenInstanceDestroyed);
		}
	}
}

void UXD_LogicLodSystemRuntime::WhenActorSpawned(AActor* Actor)
{
	if (bAutoRegisterSpawnedActor && Actor->Implements<UXD_LogicLodInstanceInterface>())
	{
		ULevel* Level = Actor->GetLevel();
		FName LevelName = FLogicLodSystemUtils::GetLevelName(Level);
		UXD_LogicLodLevelUnit*& LogicLodLevelUnit = LogicLodLevelUnits.FindOrAdd(LevelName);
		if (LogicLodLevelUnit == nullptr)
		{
			LogicLodLevelUnit = NewObject<UXD_LogicLodLevelUnit>(this, GetDefault<UXD_LogicLodSystemSettings>()->LogicLodLevelUnitClass.LoadSynchronous(), LevelName);
		}
		if (LogicLodLevelUnit->bIsLevelLoaded)
		{
			check(!LogicLodLevelUnit->LogicLodUnits.ContainsByPredicate([&](UXD_LogicLodUnitBase* E) {return E->AI_Instance.Get() == Actor; }));
			UXD_LogicLodUnitBase* LogicLodUnit = IXD_LogicLodInstanceInterface::CreateLogicLodUnit(Actor, LogicLodLevelUnit);
			LogicLodUnit->LogicLodLevelUnit = LogicLodLevelUnit;
			LogicLodLevelUnit->LogicLodUnits.Add(LogicLodUnit);
			XD_LogicLodSystem_Display_LOG("AI实例[%s]实例化，自动注册进LogicLod系统的关卡[%s]", *Actor->GetName(), *LevelName.ToString());
			Actor->OnDestroyed.AddDynamic(this, &UXD_LogicLodSystemRuntime::WhenInstanceDestroyed);
		}
	}
}

void UXD_LogicLodSystemRuntime::WhenInstanceDestroyed(AActor* Actor)
{
	ULevel* Level = Actor->GetLevel();
	FName LevelName = FLogicLodSystemUtils::GetLevelName(Level);
	UXD_LogicLodLevelUnit* LogicLodLevelUnit = LogicLodLevelUnits.FindRef(LevelName);
	int32 RemoveNum = LogicLodLevelUnit->LogicLodUnits.RemoveAll([&](UXD_LogicLodUnitBase* LogicLodUnit) {return LogicLodUnit->AI_Instance.Get() == Actor; });
	check(RemoveNum > 0);
	XD_LogicLodSystem_Display_LOG("AI实例[%s]被销毁，自动从LogicLod系统的关卡[%s]反注册", *Actor->GetName(), *LevelName.ToString());
}

UXD_LogicLodUnitBase* UXD_LogicLodSystemRuntime::DuplicateLogicLodUnit(UXD_LogicLodUnitBase* LogicLodUnitTemplate, UXD_LogicLodLevelUnit* LogicLodLevelUnit)
{
	UXD_LogicLodUnitBase* LogicLodUnit = ::DuplicateObject(LogicLodUnitTemplate, LogicLodLevelUnit, LogicLodUnitTemplate->GetFName());
	LogicLodUnit->ClearFlags(RF_WasLoaded | RF_LoadCompleted);
#if WITH_EDITOR
	// 编辑器下修复SoftObject运行时的指向
	const int32 PIEInstanceID = GetWorld()->GetOutermost()->PIEInstanceID;
	FSoftObjectPath SoftObjectPath = LogicLodUnit->AI_Instance.ToSoftObjectPath();
	if (SoftObjectPath.FixupForPIE(PIEInstanceID))
	{
		LogicLodUnit->AI_Instance = SoftObjectPath;
	}
#endif
	LogicLodUnit->LogicLodLevelUnit = LogicLodLevelUnit;
	return LogicLodUnit;
}

bool UXD_LogicLodSystemRuntime::bAutoRegisterSpawnedActor = true;

#undef LOCTEXT_NAMESPACE
