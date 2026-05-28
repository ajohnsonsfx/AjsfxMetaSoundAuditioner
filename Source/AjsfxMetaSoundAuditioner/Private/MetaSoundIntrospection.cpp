#include "MetaSoundIntrospection.h"

#include "MetasoundSource.h"
#include "MetasoundFrontendDocument.h"

namespace AjsfxAuditioner
{
	TArray<FAuditionInputDescriptor> DiscoverInputs(const UMetaSoundSource* Source)
	{
		TArray<FAuditionInputDescriptor> Out;
		if (!Source)
		{
			return Out;
		}

		const FMetasoundFrontendDocument& Doc = Source->GetConstDocument();
		const TArray<FMetasoundFrontendClassInput>& Inputs = Doc.RootGraph.GetDefaultInterface().Inputs;

		Out.Reserve(Inputs.Num());
		for (const FMetasoundFrontendClassInput& In : Inputs)
		{
			FAuditionInputDescriptor D;
			D.Name = In.Name;
			D.TypeNameRaw = In.TypeName.ToString();
			D.Type = AjsfxAuditionerPrivate::ClassifyTypeName(D.TypeNameRaw, D.bIsArray);
			Out.Add(MoveTemp(D));
		}

		return Out;
	}
}
