#include "AjsfxMetaSoundAuditioner.h"
#include "SMetaSoundAuditionerPanel.h"

#include "Framework/Docking/TabManager.h"
#include "Widgets/Docking/SDockTab.h"
#include "WorkspaceMenuStructure.h"
#include "WorkspaceMenuStructureModule.h"
#include "Styling/AppStyle.h"
#include "Framework/Application/SlateApplication.h"
#include "ToolMenus.h"

#define LOCTEXT_NAMESPACE "AjsfxMetaSoundAuditioner"

DEFINE_LOG_CATEGORY(LogAjsfxMetaSoundAuditioner);

namespace
{
	const FName AjsfxMetaSoundAuditionerTabId(TEXT("AjsfxMetaSoundAuditioner"));
}

void FAjsfxMetaSoundAuditionerModule::StartupModule()
{
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(
			AjsfxMetaSoundAuditionerTabId,
			FOnSpawnTab::CreateRaw(this, &FAjsfxMetaSoundAuditionerModule::OnSpawnTab))
		.SetDisplayName(LOCTEXT("TabTitle", "MetaSound Auditioner"))
		.SetTooltipText(LOCTEXT("TabTooltip", "Audition sounds through a MetaSound with auto-discovered inputs, looping, and a random pool."))
		.SetGroup(WorkspaceMenu::GetMenuStructure().GetToolsCategory())
		.SetIcon(FSlateIcon(FAppStyle::GetAppStyleSetName(), "MetasoundEditor.Source"));
}

void FAjsfxMetaSoundAuditionerModule::ShutdownModule()
{
	if (FSlateApplication::IsInitialized())
	{
		FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(AjsfxMetaSoundAuditionerTabId);
	}
}

TSharedRef<SDockTab> FAjsfxMetaSoundAuditionerModule::OnSpawnTab(const FSpawnTabArgs& /*Args*/)
{
	TSharedRef<SMetaSoundAuditionerPanel> Panel = SNew(SMetaSoundAuditionerPanel);
	ActivePanel = Panel;

	return SNew(SDockTab)
		.TabRole(ETabRole::NomadTab)
		[
			Panel
		];
}

void FAjsfxMetaSoundAuditionerModule::OpenTabWithSource(UMetaSoundSource* Source)
{
	FAjsfxMetaSoundAuditionerModule* Module = GetPtr();
	if (!Module) { return; }

	FGlobalTabmanager::Get()->TryInvokeTab(AjsfxMetaSoundAuditionerTabId);

	if (TSharedPtr<SMetaSoundAuditionerPanel> Panel = Module->ActivePanel.Pin())
	{
		Panel->SetSource(Source);
	}
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FAjsfxMetaSoundAuditionerModule, AjsfxMetaSoundAuditioner)
