// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include <Modules/ModuleManager.h>

class FXD_AI_LodSystem_EditorModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	void StartupModule() override;

	void ShutdownModule() override;

private:
	FDelegateHandle PostSaveWorldHandle;
	void CollectAI_LodUnit(UWorld* World);

};
