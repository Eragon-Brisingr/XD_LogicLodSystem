// Fill out your copyright notice in the Description page of Project Settings.


#include "XD_LogicLodWorldCollectionFactory.h"
#include <AssetTypeCategories.h>

#include "XD_LogicLodWorldCollection.h"

#define LOCTEXT_NAMESPACE "FXD_AutoGenSequencer_EditorModule"

UXD_LogicLodWorldCollectionFactory::UXD_LogicLodWorldCollectionFactory()
{
	// LogicLodWorldCollection现在没用
	SupportedClass = UXD_LogicLodWorldCollection::StaticClass();
	bCreateNew = false;
	bEditAfterNew = true;
}

UObject* UXD_LogicLodWorldCollectionFactory::FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	return NewObject<UXD_LogicLodWorldCollection>(InParent, InClass, InName, Flags);
}

FText UXD_LogicLodWorldCollectionFactory::GetDisplayName() const
{
	return LOCTEXT("LogicLod世界搜集器", "LogicLod世界搜集器");
}

uint32 UXD_LogicLodWorldCollectionFactory::GetMenuCategories() const
{
	return EAssetTypeCategories::Gameplay;
}

#undef LOCTEXT_NAMESPACE
