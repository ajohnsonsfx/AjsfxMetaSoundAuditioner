#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "AuditionInputDescriptor.h"
#include "AuditionSession.h"

class FDragDropOperation;
class SScrollBox;
class SVerticalBox;
class UMetaSoundSource;
class USoundWave;

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

	/** Apply symmetric drop semantics for a list of wave assets dropped on a wave-asset input slot. */
	void HandleWaveAssetsDropped(FName InputName, EAuditionInputType InputType, const TArray<USoundWave*>& Dropped);

	/** Extract USoundWave* entries from an asset drag-drop operation. */
	static TArray<USoundWave*> ExtractWavesFromDrop(const TSharedPtr<FDragDropOperation>& Op);

	TSharedPtr<SScrollBox> InputsScroll;
	TSharedPtr<SVerticalBox> InputsBox;

	TArray<FAuditionInputDescriptor> CachedDescriptors;
	FAuditionSession Session;
};
