// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <Factories/Factory.h>
#include "XD_AI_LodWorldCollectionFactory.generated.h"

/**
 * 
 */
UCLASS()
class XD_AI_LODSYSTEM_EDITOR_API UXD_AI_LodWorldCollectionFactory : public UFactory
{
	GENERATED_BODY()
public:
	UXD_AI_LodWorldCollectionFactory();

	UObject* FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;

	FText GetDisplayName() const override;
	uint32 GetMenuCategories() const override;
};
