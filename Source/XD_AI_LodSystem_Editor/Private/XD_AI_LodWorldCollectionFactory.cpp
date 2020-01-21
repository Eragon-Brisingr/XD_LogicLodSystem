// Fill out your copyright notice in the Description page of Project Settings.


#include "XD_AI_LodWorldCollectionFactory.h"
#include <AssetTypeCategories.h>

#include "XD_AI_LodWorldCollection.h"

#define LOCTEXT_NAMESPACE "FXD_AutoGenSequencer_EditorModule"

UXD_AI_LodWorldCollectionFactory::UXD_AI_LodWorldCollectionFactory()
{
	// AI_LodWorldCollection现在没用
	SupportedClass = UXD_AI_LodWorldCollection::StaticClass();
	bCreateNew = false;
	bEditAfterNew = true;
}

UObject* UXD_AI_LodWorldCollectionFactory::FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	return NewObject<UXD_AI_LodWorldCollection>(InParent, InClass, InName, Flags);
}

FText UXD_AI_LodWorldCollectionFactory::GetDisplayName() const
{
	return LOCTEXT("AI_Lod世界搜集器", "AI_Lod世界搜集器");
}

uint32 UXD_AI_LodWorldCollectionFactory::GetMenuCategories() const
{
	return EAssetTypeCategories::Gameplay;
}

#undef LOCTEXT_NAMESPACE
