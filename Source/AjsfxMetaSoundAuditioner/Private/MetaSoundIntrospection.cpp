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

			const TArray<FMetasoundFrontendClassInputDefault>& Defs = In.GetDefaults();
			if (Defs.Num() > 0)
			{
				const FMetasoundFrontendLiteral& Lit = Defs[0].Literal;
				switch (D.Type)
				{
				case EAuditionInputType::Float:
				{
					float V = 0.f;
					if (Lit.TryGet(V)) { D.DefaultFloat = V; }
					break;
				}
				case EAuditionInputType::Int32:
				{
					int32 V = 0;
					if (Lit.TryGet(V)) { D.DefaultInt = V; }
					break;
				}
				case EAuditionInputType::Bool:
				{
					bool V = false;
					if (Lit.TryGet(V)) { D.DefaultBool = V; }
					break;
				}
				default: break;
				}
			}

			Out.Add(MoveTemp(D));
		}

		return Out;
	}
}
