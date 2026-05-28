#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"
#include "AuditionSession.h"
#include "Sound/SoundWave.h"

#if WITH_DEV_AUTOMATION_TESTS

namespace
{
	/** Build a transient USoundWave just for pointer identity in tests; never plays audio. */
	USoundWave* MakeStubWave()
	{
		return NewObject<USoundWave>(GetTransientPackage(), NAME_None, RF_Transient);
	}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FAjsfxAuditioner_Pool_SingleModeReturnsSingle,
	"Ajsfx.MetaSoundAuditioner.Pool.SingleMode",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FAjsfxAuditioner_Pool_SingleModeReturnsSingle::RunTest(const FString&)
{
	FAuditionPoolPicker P;
	P.Mode = FAuditionPoolPicker::EMode::Single;
	USoundWave* W = MakeStubWave();
	P.SingleWave = W;

	FRandomStream Rng(/*seed*/ 1234);
	TestEqual(TEXT("Single returns SingleWave"), P.Pick(Rng, /*bReRand*/true), W);
	TestEqual(TEXT("Single LastPick updated"),   P.LastPick,                   W);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FAjsfxAuditioner_Pool_EmptyPoolReturnsNull,
	"Ajsfx.MetaSoundAuditioner.Pool.EmptyPool",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FAjsfxAuditioner_Pool_EmptyPoolReturnsNull::RunTest(const FString&)
{
	FAuditionPoolPicker P;
	P.Mode = FAuditionPoolPicker::EMode::Randomize;
	FRandomStream Rng(7);
	TestNull(TEXT("Empty pool picks nullptr"), P.Pick(Rng, true));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FAjsfxAuditioner_Pool_ReRandomizeProducesVariety,
	"Ajsfx.MetaSoundAuditioner.Pool.ReRandomizeVariety",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FAjsfxAuditioner_Pool_ReRandomizeProducesVariety::RunTest(const FString&)
{
	FAuditionPoolPicker P;
	P.Mode = FAuditionPoolPicker::EMode::Randomize;
	for (int i = 0; i < 4; ++i) { P.Pool.Add(MakeStubWave()); }

	FRandomStream Rng(/*seed*/ 42);
	TSet<USoundWave*> Seen;
	for (int i = 0; i < 50; ++i)
	{
		Seen.Add(P.Pick(Rng, /*bReRand*/true));
	}
	TestTrue(TEXT(">=2 distinct picks over 50 trials"), Seen.Num() >= 2);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FAjsfxAuditioner_Pool_LockedPickStaysSame,
	"Ajsfx.MetaSoundAuditioner.Pool.LockedPick",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FAjsfxAuditioner_Pool_LockedPickStaysSame::RunTest(const FString&)
{
	FAuditionPoolPicker P;
	P.Mode = FAuditionPoolPicker::EMode::Randomize;
	for (int i = 0; i < 4; ++i) { P.Pool.Add(MakeStubWave()); }

	FRandomStream Rng(/*seed*/ 99);
	USoundWave* First = P.Pick(Rng, /*bReRand*/true);   // initial pick
	TestNotNull(TEXT("First pick non-null"), First);

	for (int i = 0; i < 20; ++i)
	{
		USoundWave* Next = P.Pick(Rng, /*bReRand*/false); // locked
		TestEqual(TEXT("Locked pick stays same across iterations"), Next, First);
	}
	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
