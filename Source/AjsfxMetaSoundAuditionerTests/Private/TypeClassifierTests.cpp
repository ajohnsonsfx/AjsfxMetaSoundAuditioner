#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"
#include "AuditionInputDescriptor.h"

#if WITH_DEV_AUTOMATION_TESTS

using namespace AjsfxAuditionerPrivate;

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FAjsfxAuditioner_Classify_PrimitiveScalars,
	"Ajsfx.MetaSoundAuditioner.Classify.PrimitiveScalars",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FAjsfxAuditioner_Classify_PrimitiveScalars::RunTest(const FString&)
{
	bool bArr = false;
	TestEqual(TEXT("Float"),   (int)ClassifyTypeName(TEXT("Float"),   bArr), (int)EAuditionInputType::Float);   TestFalse(TEXT("Float not arr"), bArr);
	TestEqual(TEXT("Int32"),   (int)ClassifyTypeName(TEXT("Int32"),   bArr), (int)EAuditionInputType::Int32);   TestFalse(TEXT("Int not arr"),   bArr);
	TestEqual(TEXT("Bool"),    (int)ClassifyTypeName(TEXT("Bool"),    bArr), (int)EAuditionInputType::Bool);    TestFalse(TEXT("Bool not arr"),  bArr);
	TestEqual(TEXT("String"),  (int)ClassifyTypeName(TEXT("String"),  bArr), (int)EAuditionInputType::String);  TestFalse(TEXT("String not arr"),bArr);
	TestEqual(TEXT("Trigger"), (int)ClassifyTypeName(TEXT("Trigger"), bArr), (int)EAuditionInputType::Trigger); TestFalse(TEXT("Trigger not arr"),bArr);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FAjsfxAuditioner_Classify_WaveAndArrays,
	"Ajsfx.MetaSoundAuditioner.Classify.WaveAndArrays",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FAjsfxAuditioner_Classify_WaveAndArrays::RunTest(const FString&)
{
	bool bArr = false;
	TestEqual(TEXT("WaveAsset"),       (int)ClassifyTypeName(TEXT("WaveAsset"),       bArr), (int)EAuditionInputType::WaveAsset);      TestFalse(TEXT("WA not arr"), bArr);
	TestEqual(TEXT("WaveAsset:Array"), (int)ClassifyTypeName(TEXT("WaveAsset:Array"), bArr), (int)EAuditionInputType::WaveAssetArray); TestTrue (TEXT("WA arr flag"), bArr);
	TestEqual(TEXT("Float:Array"),     (int)ClassifyTypeName(TEXT("Float:Array"),     bArr), (int)EAuditionInputType::OtherArray);     TestTrue (TEXT("Float arr"),   bArr);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FAjsfxAuditioner_Classify_UnknownFallsBackToObject,
	"Ajsfx.MetaSoundAuditioner.Classify.UnknownFallback",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FAjsfxAuditioner_Classify_UnknownFallsBackToObject::RunTest(const FString&)
{
	bool bArr = false;
	TestEqual(TEXT("Unknown scalar -> OtherObject"),
		(int)ClassifyTypeName(TEXT("SomeCustomType"), bArr), (int)EAuditionInputType::OtherObject);
	TestFalse(TEXT("Scalar not array"), bArr);

	TestEqual(TEXT("Unknown array -> OtherArray"),
		(int)ClassifyTypeName(TEXT("SomeCustomType:Array"), bArr), (int)EAuditionInputType::OtherArray);
	TestTrue(TEXT("Array flag set"), bArr);
	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
