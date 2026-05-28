#include "SMetaSoundAuditionerPanel.h"

#include "MetaSoundIntrospection.h"
#include "MetasoundSource.h"
#include "Sound/SoundWave.h"

#include "PropertyCustomizationHelpers.h"
#include "SDropTarget.h"
#include "DragAndDrop/AssetDragDropOp.h"

#include "Widgets/Layout/SBorder.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Layout/SUniformGridPanel.h"
#include "Widgets/Layout/SSeparator.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SCheckBox.h"
#include "Widgets/Input/SNumericEntryBox.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "Widgets/Input/SComboBox.h"
#include "Styling/AppStyle.h"

#define LOCTEXT_NAMESPACE "AjsfxMetaSoundAuditioner"

namespace
{
	FText TypeToText(EAuditionInputType T)
	{
		switch (T)
		{
		case EAuditionInputType::Float:          return LOCTEXT("T_Float", "Float");
		case EAuditionInputType::Int32:          return LOCTEXT("T_Int", "Int32");
		case EAuditionInputType::Bool:           return LOCTEXT("T_Bool", "Bool");
		case EAuditionInputType::String:         return LOCTEXT("T_String", "String");
		case EAuditionInputType::Trigger:        return LOCTEXT("T_Trigger", "Trigger");
		case EAuditionInputType::WaveAsset:      return LOCTEXT("T_Wave", "WaveAsset");
		case EAuditionInputType::WaveAssetArray: return LOCTEXT("T_WaveArr", "WaveAsset[]");
		case EAuditionInputType::OtherObject:    return LOCTEXT("T_Obj", "Object");
		case EAuditionInputType::OtherArray:     return LOCTEXT("T_Arr", "Array");
		default:                                 return LOCTEXT("T_Unsupp", "Unsupported");
		}
	}
}

void SMetaSoundAuditionerPanel::Construct(const FArguments& /*InArgs*/)
{
	InputsBox = SNew(SVerticalBox);

	ChildSlot
	[
		SNew(SVerticalBox)
		// Header: MetaSound picker
		+ SVerticalBox::Slot().AutoHeight().Padding(6)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center).Padding(0, 0, 8, 0)
			[
				SNew(STextBlock).Text(LOCTEXT("MetaSoundLabel", "MetaSound:"))
			]
			+ SHorizontalBox::Slot().FillWidth(1.f)
			[
				SNew(SObjectPropertyEntryBox)
					.AllowedClass(UMetaSoundSource::StaticClass())
					.OnObjectChanged(this, &SMetaSoundAuditionerPanel::OnSourceChanged)
					.AllowClear(true)
					.DisplayUseSelected(true)
					.DisplayBrowse(true)
			]
		]
		// Transport bar
		+ SVerticalBox::Slot().AutoHeight().Padding(6, 2)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot().AutoWidth().Padding(0, 0, 4, 0)
			[
				SNew(SButton)
					.Text(LOCTEXT("Play", "Play"))
					.OnClicked(this, &SMetaSoundAuditionerPanel::OnPlayClicked)
			]
			+ SHorizontalBox::Slot().AutoWidth().Padding(0, 0, 12, 0)
			[
				SNew(SButton)
					.Text(LOCTEXT("Stop", "Stop"))
					.OnClicked(this, &SMetaSoundAuditionerPanel::OnStopClicked)
			]
			+ SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center).Padding(0, 0, 12, 0)
			[
				SNew(SCheckBox)
					.IsChecked_Lambda([this]() { return Session.GetLoop() ? ECheckBoxState::Checked : ECheckBoxState::Unchecked; })
					.OnCheckStateChanged_Lambda([this](ECheckBoxState S) { Session.SetLoop(S == ECheckBoxState::Checked); })
					[ SNew(STextBlock).Text(LOCTEXT("Loop", "Loop")) ]
			]
			+ SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center)
			[
				SNew(SCheckBox)
					.IsChecked_Lambda([this]() { return Session.GetReRandomizeOnLoop() ? ECheckBoxState::Checked : ECheckBoxState::Unchecked; })
					.OnCheckStateChanged_Lambda([this](ECheckBoxState S) { Session.SetReRandomizeOnLoop(S == ECheckBoxState::Checked); })
					[ SNew(STextBlock).Text(LOCTEXT("ReRand", "Re-randomize on loop")) ]
			]
		]
		+ SVerticalBox::Slot().AutoHeight().Padding(6, 2) [ SNew(SSeparator) ]
		// Inputs list
		+ SVerticalBox::Slot().FillHeight(1.f).Padding(6)
		[
			SAssignNew(InputsScroll, SScrollBox)
			+ SScrollBox::Slot() [ InputsBox.ToSharedRef() ]
		]
	];
}

void SMetaSoundAuditionerPanel::OnSourceChanged(const FAssetData& AssetData)
{
	UObject* Obj = AssetData.GetAsset();
	SetSource(Cast<UMetaSoundSource>(Obj));
}

void SMetaSoundAuditionerPanel::SetSource(UMetaSoundSource* InSource)
{
	Session.SetSource(InSource);
	CachedDescriptors = AjsfxAuditioner::DiscoverInputs(InSource);

	// Seed scalar caches from declared MetaSound defaults so the UI shows the graph's intended values.
	for (const FAuditionInputDescriptor& Desc : CachedDescriptors)
	{
		if (Desc.DefaultFloat.IsSet()) { Session.SeedFloatDefault(Desc.Name, Desc.DefaultFloat.GetValue()); }
		if (Desc.DefaultInt.IsSet())   { Session.SeedIntDefault(Desc.Name,   Desc.DefaultInt.GetValue());   }
		if (Desc.DefaultBool.IsSet())  { Session.SeedBoolDefault(Desc.Name,  Desc.DefaultBool.GetValue());  }
	}

	RefreshInputRows();
}

void SMetaSoundAuditionerPanel::RefreshInputRows()
{
	if (!InputsBox.IsValid()) { return; }
	InputsBox->ClearChildren();

	for (const FAuditionInputDescriptor& Desc : CachedDescriptors)
	{
		InputsBox->AddSlot().AutoHeight().Padding(0, 2)
		[
			SNew(SBorder)
				.BorderImage(FAppStyle::GetBrush("ToolPanel.GroupBorder"))
				.Padding(6)
				[ BuildRowForInput(Desc) ]
		];
	}
}

TSharedRef<SWidget> SMetaSoundAuditionerPanel::BuildRowForInput(const FAuditionInputDescriptor& Desc)
{
	const FName InputName = Desc.Name;
	TSharedRef<SVerticalBox> Box = SNew(SVerticalBox);

	// Header row: input name + type label
	Box->AddSlot().AutoHeight().Padding(0, 0, 0, 4)
	[
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot().FillWidth(1.f).VAlign(VAlign_Center)
		[
			SNew(STextBlock)
				.Text(FText::FromName(Desc.Name))
				.Font(FAppStyle::Get().GetFontStyle("BoldFont"))
		]
		+ SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center)
		[
			SNew(STextBlock)
				.Text(TypeToText(Desc.Type))
				.ColorAndOpacity(FSlateColor::UseSubduedForeground())
		]
	];

	switch (Desc.Type)
	{
	case EAuditionInputType::Float:
	{
		Box->AddSlot().AutoHeight()
		[
			SNew(SNumericEntryBox<float>)
				.AllowSpin(true)
				.Value_Lambda([this, InputName]() { return Session.GetFloatParam(InputName); })
				.OnValueCommitted_Lambda([this, InputName](float V, ETextCommit::Type) { Session.SetFloatParam(InputName, V); })
				.OnValueChanged_Lambda([this, InputName](float V) { Session.SetFloatParam(InputName, V); })
		];
		break;
	}
	case EAuditionInputType::Int32:
	{
		Box->AddSlot().AutoHeight()
		[
			SNew(SNumericEntryBox<int32>)
				.AllowSpin(true)
				.Value_Lambda([this, InputName]() { return Session.GetIntParam(InputName); })
				.OnValueCommitted_Lambda([this, InputName](int32 V, ETextCommit::Type) { Session.SetIntParam(InputName, V); })
				.OnValueChanged_Lambda([this, InputName](int32 V) { Session.SetIntParam(InputName, V); })
		];
		break;
	}
	case EAuditionInputType::Bool:
	{
		Box->AddSlot().AutoHeight()
		[
			SNew(SCheckBox)
				.IsChecked_Lambda([this, InputName]()
					{
						const TOptional<bool> V = Session.GetBoolParam(InputName);
						return (V.IsSet() && V.GetValue()) ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
					})
				.OnCheckStateChanged_Lambda([this, InputName](ECheckBoxState S)
					{ Session.SetBoolParam(InputName, S == ECheckBoxState::Checked); })
		];
		break;
	}
	case EAuditionInputType::String:
	{
		Box->AddSlot().AutoHeight()
		[
			SNew(SEditableTextBox)
				.OnTextCommitted_Lambda([this, InputName](const FText& T, ETextCommit::Type)
					{ /* String params are runtime-only on FAudioParameter; left as a visual stub. */ })
		];
		break;
	}
	case EAuditionInputType::Trigger:
	{
		Box->AddSlot().AutoHeight()
		[
			SNew(SButton)
				.Text(LOCTEXT("Fire", "Fire"))
				.OnClicked_Lambda([this, InputName]() { Session.FireTrigger(InputName); return FReply::Handled(); })
		];
		break;
	}
	case EAuditionInputType::WaveAsset:
	{
		FAuditionPoolPicker& Picker = Session.GetOrCreatePicker(InputName);
		TSharedRef<SVerticalBox> WaveContent = SNew(SVerticalBox);

		// Mode toggle: Single / Randomize Pool
		TSharedRef<SHorizontalBox> ModeRow = SNew(SHorizontalBox);
		ModeRow->AddSlot().AutoWidth().Padding(0, 0, 8, 0)
		[
			SNew(SCheckBox)
				.Style(FAppStyle::Get(), "RadioButton")
				.IsChecked_Lambda([this, InputName]()
					{
						const FAuditionPoolPicker* P = Session.GetPickers().Find(InputName);
						return (P && P->Mode == FAuditionPoolPicker::EMode::Single) ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
					})
				.OnCheckStateChanged_Lambda([this, InputName](ECheckBoxState S)
					{
						if (S == ECheckBoxState::Checked)
						{
							Session.GetOrCreatePicker(InputName).Mode = FAuditionPoolPicker::EMode::Single;
							RefreshInputRows();
						}
					})
				[ SNew(STextBlock).Text(LOCTEXT("ModeSingle", "Single")) ]
		];
		ModeRow->AddSlot().AutoWidth()
		[
			SNew(SCheckBox)
				.Style(FAppStyle::Get(), "RadioButton")
				.IsChecked_Lambda([this, InputName]()
					{
						const FAuditionPoolPicker* P = Session.GetPickers().Find(InputName);
						return (P && P->Mode == FAuditionPoolPicker::EMode::Randomize) ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
					})
				.OnCheckStateChanged_Lambda([this, InputName](ECheckBoxState S)
					{
						if (S == ECheckBoxState::Checked)
						{
							Session.GetOrCreatePicker(InputName).Mode = FAuditionPoolPicker::EMode::Randomize;
							RefreshInputRows();
						}
					})
				[ SNew(STextBlock).Text(LOCTEXT("ModeRand", "Randomize Pool")) ]
		];
		WaveContent->AddSlot().AutoHeight().Padding(0, 0, 0, 4) [ ModeRow ];

		if (Picker.Mode == FAuditionPoolPicker::EMode::Single)
		{
			WaveContent->AddSlot().AutoHeight()
			[
				SNew(SObjectPropertyEntryBox)
					.AllowedClass(USoundWave::StaticClass())
					.ObjectPath_Lambda([this, InputName]()
						{
							const FAuditionPoolPicker* P = Session.GetPickers().Find(InputName);
							return (P && P->SingleWave) ? P->SingleWave->GetPathName() : FString();
						})
					.OnObjectChanged_Lambda([this, InputName](const FAssetData& AD)
						{
							Session.GetOrCreatePicker(InputName).SingleWave = Cast<USoundWave>(AD.GetAsset());
						})
			];
		}
		else
		{
			// Randomize pool: list current picks + an Add row.
			TSharedRef<SVerticalBox> PoolList = SNew(SVerticalBox);
			for (int32 i = 0; i < Picker.Pool.Num(); ++i)
			{
				const int32 Index = i;
				USoundWave* Wave = Picker.Pool[i];
				PoolList->AddSlot().AutoHeight().Padding(0, 1)
				[
					SNew(SHorizontalBox)
					+ SHorizontalBox::Slot().FillWidth(1.f)
					[
						SNew(SObjectPropertyEntryBox)
							.AllowedClass(USoundWave::StaticClass())
							.ObjectPath(Wave ? Wave->GetPathName() : FString())
							.OnObjectChanged_Lambda([this, InputName, Index](const FAssetData& AD)
								{
									TArray<USoundWave*>& Pool = Session.GetOrCreatePicker(InputName).Pool;
									if (Pool.IsValidIndex(Index)) { Pool[Index] = Cast<USoundWave>(AD.GetAsset()); }
								})
					]
					+ SHorizontalBox::Slot().AutoWidth()
					[
						SNew(SButton)
							.Text(LOCTEXT("RemovePoolEntry", "X"))
							.OnClicked_Lambda([this, InputName, Index]()
								{
									TArray<USoundWave*>& Pool = Session.GetOrCreatePicker(InputName).Pool;
									if (Pool.IsValidIndex(Index)) { Pool.RemoveAt(Index); }
									RefreshInputRows();
									return FReply::Handled();
								})
					]
				];
			}
			PoolList->AddSlot().AutoHeight().Padding(0, 4, 0, 0)
			[
				SNew(SButton)
					.Text(LOCTEXT("AddPoolEntry", "+ Add Wave"))
					.OnClicked_Lambda([this, InputName]()
						{
							Session.GetOrCreatePicker(InputName).Pool.Add(nullptr);
							RefreshInputRows();
							return FReply::Handled();
						})
			];
			WaveContent->AddSlot().AutoHeight() [ PoolList ];
		}

		Box->AddSlot().AutoHeight()
		[
			SNew(SDropTarget)
				.OnAllowDrop_Lambda([](TSharedPtr<FDragDropOperation> Op)
					{ return SMetaSoundAuditionerPanel::ExtractWavesFromDrop(Op).Num() > 0; })
				.OnIsRecognized_Lambda([](TSharedPtr<FDragDropOperation> Op)
					{ return SMetaSoundAuditionerPanel::ExtractWavesFromDrop(Op).Num() > 0; })
				.OnDropped_Lambda([this, InputName](const FGeometry&, const FDragDropEvent& Ev) -> FReply
					{
						TArray<USoundWave*> Waves = ExtractWavesFromDrop(Ev.GetOperation());
						if (Waves.Num() > 0)
						{
							HandleWaveAssetsDropped(InputName, EAuditionInputType::WaveAsset, Waves);
						}
						return FReply::Handled();
					})
				[ WaveContent ]
		];
		break;
	}
	case EAuditionInputType::WaveAssetArray:
	{
		// The graph itself selects from the array, so this is just a plain array editor.
		// Reuse the pool list UI but always-active and pushed straight to the AudioComponent as an object array on Play.
		FAuditionPoolPicker& Picker = Session.GetOrCreatePicker(InputName);
		Picker.Mode = FAuditionPoolPicker::EMode::Randomize;

		TSharedRef<SVerticalBox> ArrayContent = SNew(SVerticalBox);

		// Add a small label clarifying the semantics.
		ArrayContent->AddSlot().AutoHeight().Padding(0, 0, 0, 4)
		[
			SNew(STextBlock)
				.Text(LOCTEXT("ArrayHint", "Array is passed to the MetaSound as-is. The graph picks."))
				.ColorAndOpacity(FSlateColor::UseSubduedForeground())
		];

		// Reuse: same UI as the pool list above.
		TSharedRef<SVerticalBox> List = SNew(SVerticalBox);
		for (int32 i = 0; i < Picker.Pool.Num(); ++i)
		{
			const int32 Index = i;
			USoundWave* Wave = Picker.Pool[i];
			List->AddSlot().AutoHeight().Padding(0, 1)
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot().FillWidth(1.f)
				[
					SNew(SObjectPropertyEntryBox)
						.AllowedClass(USoundWave::StaticClass())
						.ObjectPath(Wave ? Wave->GetPathName() : FString())
						.OnObjectChanged_Lambda([this, InputName, Index](const FAssetData& AD)
							{
								TArray<USoundWave*>& Pool = Session.GetOrCreatePicker(InputName).Pool;
								if (Pool.IsValidIndex(Index)) { Pool[Index] = Cast<USoundWave>(AD.GetAsset()); }
							})
				]
				+ SHorizontalBox::Slot().AutoWidth()
				[
					SNew(SButton)
						.Text(LOCTEXT("RemoveArrEntry", "X"))
						.OnClicked_Lambda([this, InputName, Index]()
							{
								TArray<USoundWave*>& Pool = Session.GetOrCreatePicker(InputName).Pool;
								if (Pool.IsValidIndex(Index)) { Pool.RemoveAt(Index); }
								RefreshInputRows();
								return FReply::Handled();
							})
				]
			];
		}
		List->AddSlot().AutoHeight().Padding(0, 4, 0, 0)
		[
			SNew(SButton)
				.Text(LOCTEXT("AddArrEntry", "+ Add Wave"))
				.OnClicked_Lambda([this, InputName]()
					{
						Session.GetOrCreatePicker(InputName).Pool.Add(nullptr);
						RefreshInputRows();
						return FReply::Handled();
					})
		];
		ArrayContent->AddSlot().AutoHeight() [ List ];

		Box->AddSlot().AutoHeight()
		[
			SNew(SDropTarget)
				.OnAllowDrop_Lambda([](TSharedPtr<FDragDropOperation> Op)
					{ return SMetaSoundAuditionerPanel::ExtractWavesFromDrop(Op).Num() > 0; })
				.OnIsRecognized_Lambda([](TSharedPtr<FDragDropOperation> Op)
					{ return SMetaSoundAuditionerPanel::ExtractWavesFromDrop(Op).Num() > 0; })
				.OnDropped_Lambda([this, InputName](const FGeometry&, const FDragDropEvent& Ev) -> FReply
					{
						TArray<USoundWave*> Waves = ExtractWavesFromDrop(Ev.GetOperation());
						if (Waves.Num() > 0)
						{
							HandleWaveAssetsDropped(InputName, EAuditionInputType::WaveAssetArray, Waves);
						}
						return FReply::Handled();
					})
				[ ArrayContent ]
		];
		break;
	}
	default:
	{
		Box->AddSlot().AutoHeight()
		[
			SNew(STextBlock)
				.Text(FText::Format(LOCTEXT("UnsupportedType", "Unsupported type: {0}"), FText::FromString(Desc.TypeNameRaw)))
				.ColorAndOpacity(FSlateColor::UseSubduedForeground())
		];
		break;
	}
	}

	return Box;
}

FReply SMetaSoundAuditionerPanel::OnPlayClicked()
{
	// For WaveAssetArray entries, push the array to the audio component just-in-time.
	// (Single/Randomize WaveAsset is handled inside Session::Play via Pick.)
	Session.Play();

	for (const FAuditionInputDescriptor& Desc : CachedDescriptors)
	{
		if (Desc.Type != EAuditionInputType::WaveAssetArray) { continue; }
		const FAuditionPoolPicker* P = Session.GetPickers().Find(Desc.Name);
		if (!P) { continue; }
		TArray<UObject*> AsObjects;
		AsObjects.Reserve(P->Pool.Num());
		for (USoundWave* W : P->Pool) { AsObjects.Add(W); }
		Session.SetObjectArrayParam(Desc.Name, AsObjects);
	}
	return FReply::Handled();
}

FReply SMetaSoundAuditionerPanel::OnStopClicked()
{
	Session.Stop();
	return FReply::Handled();
}

TArray<USoundWave*> SMetaSoundAuditionerPanel::ExtractWavesFromDrop(const TSharedPtr<FDragDropOperation>& Op)
{
	TArray<USoundWave*> Out;
	if (!Op.IsValid() || !Op->IsOfType<FAssetDragDropOp>())
	{
		return Out;
	}
	const TSharedPtr<FAssetDragDropOp> AssetOp = StaticCastSharedPtr<FAssetDragDropOp>(Op);
	for (const FAssetData& AD : AssetOp->GetAssets())
	{
		if (USoundWave* W = Cast<USoundWave>(AD.GetAsset()))
		{
			Out.Add(W);
		}
	}
	return Out;
}

void SMetaSoundAuditionerPanel::HandleWaveAssetsDropped(FName InputName, EAuditionInputType InputType, const TArray<USoundWave*>& Dropped)
{
	if (Dropped.Num() == 0) { return; }

	FAuditionPoolPicker& Picker = Session.GetOrCreatePicker(InputName);

	if (InputType == EAuditionInputType::WaveAssetArray)
	{
		// MetaSound input is an array — pass entries straight through; graph handles selection internally.
		Picker.Mode = FAuditionPoolPicker::EMode::Randomize;
		Picker.Pool.Append(Dropped);
	}
	else // WaveAsset (single MetaSound input)
	{
		if (Dropped.Num() == 1)
		{
			// Single asset onto single input: respect current mode.
			if (Picker.Mode == FAuditionPoolPicker::EMode::Single)
			{
				Picker.SingleWave = Dropped[0];
			}
			else
			{
				Picker.Pool.Add(Dropped[0]);
			}
		}
		else
		{
			// Multiple assets onto a single input: auto-switch to Randomize Pool for external randomization.
			Picker.Mode = FAuditionPoolPicker::EMode::Randomize;
			Picker.Pool = Dropped;
		}
	}

	RefreshInputRows();
}

#undef LOCTEXT_NAMESPACE
