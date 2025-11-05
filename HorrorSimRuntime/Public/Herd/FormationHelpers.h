#pragma once

#include "CoreMinimal.h"
#include "FormationHelpers.generated.h"


UENUM(BlueprintType)
enum class EHerdFormationType : uint8
{
	Circle,
	Wedge,
	Grid,
	RandomInRadius
};


struct FHerdFormationLogic
{
public:
	/**
	 * Generates an array of local-space 2D/3D offsets for a given formation.
	 *
	 * @param FormationType The shape to generate.
	 * @param NumMembers    The total number of points to create.
	 * @param Spacing       The distance between members (or radius for Circle).
	 * @return              An array of FVector offsets.
	 */
	static TArray<FVector> GenerateFormationOffsets(
		EHerdFormationType FormationType,
		int32 NumMembers,
		float Spacing
	);

private:
	/**  Places all members randomly within a 2D circle. */
	static void GenerateCircleOffsets(TArray<FVector>& Offsets, int32 Num, float Radius);
	/** A classic "V" formation. */
	static void GenerateWedgeOffsets(TArray<FVector>& Offsets, int32 Num, float Spacing);
	static void GenerateGridOffsets(TArray<FVector>& Offsets, int32 Num, float Spacing);
	static void GenerateRandomOffsets(TArray<FVector>& Offsets, int32 Num, float Radius);
};

