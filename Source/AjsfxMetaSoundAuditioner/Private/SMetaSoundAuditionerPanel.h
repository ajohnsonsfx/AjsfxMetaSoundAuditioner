#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "AuditionInputDescriptor.h"
#include "AuditionSession.h"

class SScrollBox;
class SVerticalBox;
class UMetaSoundSource;

class SMetaSoundAuditionerPanel : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SMetaSoundAuditionerPanel) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

	/** Load a MetaSound into the panel; re-discovers inputs and rebuilds the rows. */
	void SetSource(UMetaSoundSource* InSource);

private:
	void OnSourceChanged(const FAssetData& AssetData);
	void RefreshInputRows();
	TSharedRef<SWidget> BuildRowForInput(const FAuditionInputDescriptor& Desc);

	FReply OnPlayClicked();
	FReply OnStopClicked();

	TSharedPtr<SScrollBox> InputsScroll;
	TSharedPtr<SVerticalBox> InputsBox;

	TArray<FAuditionInputDescriptor> CachedDescriptors;
	FAuditionSession Session;
};
