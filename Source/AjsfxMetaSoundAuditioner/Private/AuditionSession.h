#pragma once

#include "CoreMinimal.h"
#include "UObject/StrongObjectPtr.h"
#include "Math/RandomStream.h"

class UAudioComponent;
class UMetaSoundSource;
class USoundWave;

/**
 * Pure-logic random pool picker. Splitting this out of FAuditionSession lets us unit-test the
 * "single vs. pool" + lock-on-loop semantics without spinning up an editor audio device.
 */
struct FAuditionPoolPicker
{
	enum class EMode : uint8
	{
		Single,
		Randomize,
	};

	EMode Mode = EMode::Single;
	USoundWave* SingleWave = nullptr;
	TArray<USoundWave*> Pool;

	/** Cached last pick — used when loop holds the same wave (bReRandomizeOnLoop=false). */
	USoundWave* LastPick = nullptr;

	bool IsActive() const
	{
		return Mode == EMode::Randomize ? (Pool.Num() > 0) : (SingleWave != nullptr);
	}

	/**
	 * Returns the wave to play given the current mode + lock state. Updates LastPick.
	 * Inline so the tests module (which links against this module's interface only) can call it.
	 */
	FORCEINLINE USoundWave* Pick(FRandomStream& Rng, bool bReRandomize)
	{
		if (Mode == EMode::Single)
		{
			LastPick = SingleWave;
			return SingleWave;
		}
		if (Pool.Num() == 0)
		{
			LastPick = nullptr;
			return nullptr;
		}
		if (!bReRandomize && LastPick != nullptr && Pool.Contains(LastPick))
		{
			return LastPick;
		}
		const int32 Index = Rng.RandRange(0, Pool.Num() - 1);
		LastPick = Pool[Index];
		return LastPick;
	}
};

/**
 * Owns a single editor UAudioComponent and applies discovered MetaSound input values to it.
 * Handles the WaveAsset randomize-pool + loop + re-randomize-on-loop logic.
 */
class FAuditionSession
{
public:
	FAuditionSession();
	~FAuditionSession();

	void SetSource(UMetaSoundSource* InSource);
	UMetaSoundSource* GetSource() const { return Source.Get(); }

	void SetLoop(bool bIn)               { bLoop = bIn; }
	void SetReRandomizeOnLoop(bool bIn)  { bReRandomizeOnLoop = bIn; }
	bool GetLoop() const                 { return bLoop; }
	bool GetReRandomizeOnLoop() const    { return bReRandomizeOnLoop; }

	/** Per-input WaveAsset config (single or pool). Keyed by input name. */
	FAuditionPoolPicker& GetOrCreatePicker(FName InputName);
	const TMap<FName, FAuditionPoolPicker>& GetPickers() const { return Pickers; }

	/** Parameter setters (passthrough to UAudioComponent param API). */
	void SetFloatParam(FName Name, float Value);
	void SetIntParam(FName Name, int32 Value);
	void SetBoolParam(FName Name, bool Value);
	void SetObjectParam(FName Name, UObject* Value);
	void SetObjectArrayParam(FName Name, const TArray<UObject*>& Values);
	void FireTrigger(FName Name);

	void Play();
	void Stop();
	bool IsPlaying() const;

private:
	void OnAudioFinished(UAudioComponent* AC);
	void ApplyAllPickers();
	UAudioComponent* GetOrCreateComponent();

	TStrongObjectPtr<UMetaSoundSource> Source;
	TStrongObjectPtr<UAudioComponent> AudioComponent;

	TMap<FName, FAuditionPoolPicker> Pickers;

	bool bLoop = false;
	bool bReRandomizeOnLoop = true;
	bool bPlaying = false;

	FRandomStream Rng;
};
