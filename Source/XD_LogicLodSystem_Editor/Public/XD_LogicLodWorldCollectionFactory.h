// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <Factories/Factory.h>
#include "XD_LogicLodWorldCollectionFactory.generated.h"

/**
 * 
 */
UCLASS()
class XD_LOGICLODSYSTEM_EDITOR_API UXD_LogicLodWorldCollectionFactory : public UFactory
{
	GENERATED_BODY()
public:
	UXD_LogicLodWorldCollectionFactory();

	UObject* FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;

	FText GetDisplayName() const override;
	uint32 GetMenuCategories() const override;
};
