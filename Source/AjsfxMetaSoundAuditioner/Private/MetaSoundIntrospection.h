#pragma once

#include "CoreMinimal.h"
#include "AuditionInputDescriptor.h"

class UMetaSoundSource;

namespace AjsfxAuditioner
{
	/**
	 * Enumerate the public inputs of a MetaSoundSource and return descriptors the UI can render.
	 * Safe to call with nullptr (returns empty). Editor-only data path.
	 */
	TArray<FAuditionInputDescriptor> DiscoverInputs(const UMetaSoundSource* Source);
}
