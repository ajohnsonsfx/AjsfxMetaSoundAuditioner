#include "AuditionSession.h"

#include "AjsfxMetaSoundAuditioner.h"
#include "Components/AudioComponent.h"
#include "Editor.h"
#include "Editor/EditorEngine.h"
#include "MetasoundSource.h"
#include "Sound/SoundWave.h"

FAuditionSession::FAuditionSession()
	: Rng(FRandomStream(FPlatformTime::Cycles()))
{
}

FAuditionSession::~FAuditionSession()
{
	Stop();
}

void FAuditionSession::SetSource(UMetaSoundSource* InSource)
{
	if (Source.Get() != InSource)
	{
		Stop();
		Source.Reset(InSource);
		Pickers.Reset();
		FloatValues.Reset();
		IntValues.Reset();
		BoolValues.Reset();
	}
}

FAuditionPoolPicker& FAuditionSession::GetOrCreatePicker(FName InputName)
{
	return Pickers.FindOrAdd(InputName);
}

UAudioComponent* FAuditionSession::GetOrCreateComponent()
{
	if (!GEditor || !Source.IsValid())
	{
		return nullptr;
	}
	UAudioComponent* AC = GEditor->ResetPreviewAudioComponent(Source.Get());
	AudioComponent.Reset(AC);
	if (AC)
	{
		// Rebind finish callback.
		AC->OnAudioFinishedNative.RemoveAll(this);
		AC->OnAudioFinishedNative.AddRaw(this, &FAuditionSession::OnAudioFinished);
	}
	return AC;
}

void FAuditionSession::ApplyAllPickers()
{
	UAudioComponent* AC = AudioComponent.Get();
	if (!AC) { return; }

	for (TPair<FName, FAuditionPoolPicker>& Kvp : Pickers)
	{
		if (!Kvp.Value.IsActive()) { continue; }
		USoundWave* Picked = Kvp.Value.Pick(Rng, bReRandomizeOnLoop);
		if (Picked)
		{
			AC->SetObjectParameter(Kvp.Key, Picked);
		}
	}
}

void FAuditionSession::SetFloatParam(FName Name, float Value)
{
	FloatValues.Add(Name, Value);
	if (UAudioComponent* AC = AudioComponent.Get()) { AC->SetFloatParameter(Name, Value); }
}

void FAuditionSession::SetIntParam(FName Name, int32 Value)
{
	IntValues.Add(Name, Value);
	if (UAudioComponent* AC = AudioComponent.Get()) { AC->SetIntParameter(Name, Value); }
}

void FAuditionSession::SetBoolParam(FName Name, bool Value)
{
	BoolValues.Add(Name, Value);
	if (UAudioComponent* AC = AudioComponent.Get()) { AC->SetBoolParameter(Name, Value); }
}

void FAuditionSession::SetObjectParam(FName Name, UObject* Value)
{
	if (UAudioComponent* AC = AudioComponent.Get()) { AC->SetObjectParameter(Name, Value); }
}

void FAuditionSession::SetObjectArrayParam(FName Name, const TArray<UObject*>& Values)
{
	if (UAudioComponent* AC = AudioComponent.Get()) { AC->SetObjectArrayParameter(Name, Values); }
}

void FAuditionSession::FireTrigger(FName Name)
{
	if (UAudioComponent* AC = AudioComponent.Get()) { AC->SetTriggerParameter(Name); }
}

void FAuditionSession::Play()
{
	if (!Source.IsValid())
	{
		UE_LOG(LogAjsfxMetaSoundAuditioner, Warning, TEXT("Play() called with no source"));
		return;
	}

	UAudioComponent* AC = GetOrCreateComponent();
	if (!AC) { return; }

	ApplyAllPickers();
	AC->Play();
	bPlaying = true;
}

void FAuditionSession::Stop()
{
	if (UAudioComponent* AC = AudioComponent.Get())
	{
		AC->OnAudioFinishedNative.RemoveAll(this);
		AC->Stop();
	}
	AudioComponent.Reset();
	bPlaying = false;
}

bool FAuditionSession::IsPlaying() const
{
	if (UAudioComponent* AC = AudioComponent.Get())
	{
		return AC->IsPlaying();
	}
	return false;
}

void FAuditionSession::OnAudioFinished(UAudioComponent* /*AC*/)
{
	if (!bLoop)
	{
		bPlaying = false;
		return;
	}

	// Re-apply pickers (re-randomizes if bReRandomizeOnLoop is true; otherwise keeps last pick).
	ApplyAllPickers();
	if (UAudioComponent* Current = AudioComponent.Get())
	{
		Current->Play();
	}
}
