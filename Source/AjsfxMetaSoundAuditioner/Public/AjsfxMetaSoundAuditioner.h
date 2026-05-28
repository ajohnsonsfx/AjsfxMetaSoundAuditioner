#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleInterface.h"
#include "Modules/ModuleManager.h"
#include "Logging/LogMacros.h"

class SDockTab;
class SMetaSoundAuditionerPanel;
class FSpawnTabArgs;
class UMetaSoundSource;

DECLARE_LOG_CATEGORY_EXTERN(LogAjsfxMetaSoundAuditioner, Log, All);

class FAjsfxMetaSoundAuditionerModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	static FAjsfxMetaSoundAuditionerModule* GetPtr()
	{
		return FModuleManager::GetModulePtr<FAjsfxMetaSoundAuditionerModule>(TEXT("AjsfxMetaSoundAuditioner"));
	}

	/** Spawn or focus the tab and load the given MetaSound into it. */
	static void OpenTabWithSource(UMetaSoundSource* Source);

private:
	TSharedRef<SDockTab> OnSpawnTab(const FSpawnTabArgs& Args);

	TWeakPtr<SMetaSoundAuditionerPanel> ActivePanel;
};
