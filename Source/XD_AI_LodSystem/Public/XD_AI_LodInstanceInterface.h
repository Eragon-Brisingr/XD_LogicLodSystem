// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "XD_AI_LodInstanceInterface.generated.h"

class UXD_AI_LodUnitBase;

UENUM()
enum class EAI_LodSystemRegisterType : uint8
{
	// 关卡初始化时模拟
	LevelInit,
	// 世界初始化时模拟
	WorldInit
};

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UXD_AI_LodInstanceInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class XD_AI_LODSYSTEM_API IXD_AI_LodInstanceInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintnativeEvent, Category = "AI|Lod")
	UXD_AI_LodUnitBase* CreateAI_LodUnit(UObject* Outer) const;
	virtual UXD_AI_LodUnitBase* CreateAI_LodUnit_Implementation(UObject* Outer) const { return nullptr; }
	static UXD_AI_LodUnitBase* CreateAI_LodUnit(AActor* Actor, UObject* Outer);

	UFUNCTION(BlueprintnativeEvent, Category = "AI|Lod")
	EAI_LodSystemRegisterType GetRegisterType() const;
	virtual EAI_LodSystemRegisterType GetRegisterType_Implementation() const { return EAI_LodSystemRegisterType::LevelInit; }
	static EAI_LodSystemRegisterType GetRegisterType(AActor* Actor) { return Execute_GetRegisterType((UObject*)Actor); }

	UFUNCTION(BlueprintnativeEvent, Category = "AI|Lod")
	void SyncToInstance(const UXD_AI_LodUnitBase* AI_LodUnit);
	virtual void SyncToInstance_Implementation(const UXD_AI_LodUnitBase* AI_LodUnit) {}
	static void SyncToInstance(AActor* Actor, const UXD_AI_LodUnitBase* AI_LodUnit) { Execute_SyncToInstance((UObject*)Actor, AI_LodUnit); }

	UFUNCTION(BlueprintnativeEvent, Category = "AI|Lod")
	void SyncToLodUnit(UXD_AI_LodUnitBase* AI_LodUnit) const;
	virtual void SyncToLodUnit_Implementation(UXD_AI_LodUnitBase* AI_LodUnit) const {}
	static void SyncToLodUnit(const AActor* Actor, UXD_AI_LodUnitBase* AI_LodUnit) { Execute_SyncToLodUnit((UObject*)Actor, AI_LodUnit); }
};
