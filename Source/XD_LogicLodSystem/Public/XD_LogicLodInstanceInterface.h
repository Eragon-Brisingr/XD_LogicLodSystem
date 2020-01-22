// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "XD_LogicLodInstanceInterface.generated.h"

class UXD_LogicLodUnitBase;

UENUM()
enum class ELogicLodSystemRegisterType : uint8
{
	// 关卡初始化时模拟
	LevelInit,
	// 世界初始化时模拟
	WorldInit
};

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UXD_LogicLodInstanceInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class XD_LOGICLODSYSTEM_API IXD_LogicLodInstanceInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintnativeEvent, Category = "AI|Lod")
	UXD_LogicLodUnitBase* CreateLogicLodUnit(UObject* Outer) const;
	virtual UXD_LogicLodUnitBase* CreateLogicLodUnit_Implementation(UObject* Outer) const { return nullptr; }
	static UXD_LogicLodUnitBase* CreateLogicLodUnit(AActor* Actor, UObject* Outer);

	UFUNCTION(BlueprintnativeEvent, Category = "AI|Lod")
	ELogicLodSystemRegisterType GetRegisterType() const;
	virtual ELogicLodSystemRegisterType GetRegisterType_Implementation() const { return ELogicLodSystemRegisterType::LevelInit; }
	static ELogicLodSystemRegisterType GetRegisterType(AActor* Actor) { return Execute_GetRegisterType((UObject*)Actor); }
};
