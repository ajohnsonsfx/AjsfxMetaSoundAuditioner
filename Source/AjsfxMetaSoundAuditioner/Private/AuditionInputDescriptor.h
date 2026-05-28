#pragma once

#include "CoreMinimal.h"

/** Coarse-grained type buckets the UI knows how to render. */
enum class EAuditionInputType : uint8
{
	Float,
	Int32,
	Bool,
	String,
	Trigger,
	WaveAsset,
	WaveAssetArray,
	OtherObject,
	OtherArray,
	Unsupported,
};

/** Describes one public input on a MetaSound, in a UI/test-friendly shape. */
struct FAuditionInputDescriptor
{
	FName Name;
	EAuditionInputType Type = EAuditionInputType::Unsupported;
	bool bIsArray = false;
	/** Raw MetaSound TypeName as a string, for display and fallback handling. */
	FString TypeNameRaw;
};

namespace AjsfxAuditionerPrivate
{
	/** Pure mapping from a MetaSound TypeName string to the UI bucket. Exposed for tests. */
	FORCEINLINE EAuditionInputType ClassifyTypeName(const FString& TypeName, bool& bOutIsArray)
	{
		bOutIsArray = false;

		// MetaSound arrays use ":Array" suffix on the type name string.
		FString Base = TypeName;
		if (Base.EndsWith(TEXT(":Array")))
		{
			bOutIsArray = true;
			Base = Base.LeftChop(6);
		}

		if (Base.Equals(TEXT("Float"), ESearchCase::IgnoreCase))    { return bOutIsArray ? EAuditionInputType::OtherArray : EAuditionInputType::Float; }
		if (Base.Equals(TEXT("Int32"), ESearchCase::IgnoreCase))    { return bOutIsArray ? EAuditionInputType::OtherArray : EAuditionInputType::Int32; }
		if (Base.Equals(TEXT("Bool"),  ESearchCase::IgnoreCase))    { return bOutIsArray ? EAuditionInputType::OtherArray : EAuditionInputType::Bool;  }
		if (Base.Equals(TEXT("String"),ESearchCase::IgnoreCase))    { return bOutIsArray ? EAuditionInputType::OtherArray : EAuditionInputType::String; }
		if (Base.Equals(TEXT("Trigger"), ESearchCase::IgnoreCase))  { return EAuditionInputType::Trigger; }
		if (Base.Equals(TEXT("WaveAsset"), ESearchCase::IgnoreCase)) { return bOutIsArray ? EAuditionInputType::WaveAssetArray : EAuditionInputType::WaveAsset; }

		return bOutIsArray ? EAuditionInputType::OtherArray : EAuditionInputType::OtherObject;
	}
}
