#include "Herd/FormationHelpers.h"


TArray<FVector> FHerdFormationLogic::GenerateFormationOffsets(
    EHerdFormationType FormationType,
    int32 NumMembers,
    float Spacing)
{
    TArray<FVector> Offsets;
    if (NumMembers <= 0)
    {
        return Offsets;
    }

    Offsets.Reserve(NumMembers); // Pre-allocate memory

    // Call the correct private helper
    switch (FormationType)
    {
        case EHerdFormationType::Circle:
            // For Circle, "Spacing" is better interpreted as the "Radius"
            GenerateCircleOffsets(Offsets, NumMembers, Spacing);
            break;

        case EHerdFormationType::Wedge:
            GenerateWedgeOffsets(Offsets, NumMembers, Spacing);
            break;

        case EHerdFormationType::Grid:
            GenerateGridOffsets(Offsets, NumMembers, Spacing);
            break;

        case EHerdFormationType::RandomInRadius:
        default:
            // For Random, "Spacing" is also the "Radius"
            GenerateRandomOffsets(Offsets, NumMembers, Spacing);
            break;
    }

    return Offsets;
}


void FHerdFormationLogic::GenerateRandomOffsets(TArray<FVector>& Offsets, int32 Num, float Radius)
{
    for (int32 i = 0; i < Num; ++i)
    {
        const FVector2D Offset2D = FMath::RandPointInCircle(Radius);
        Offsets.Add(FVector(Offset2D.X, Offset2D.Y, 0.f));
    }
}


void FHerdFormationLogic::GenerateCircleOffsets(TArray<FVector>& Offsets, int32 Num, float Radius)
{
    const float AngleIncrement = 2.0f * PI / Num; // 360 degrees / NumMembers

    for (int32 i = 0; i < Num; ++i)
    {
        const float Angle = i * AngleIncrement;

        // Calculate offset in local space (X, Y)
        // Here, X is "Forward" and Y is "Right"
        const float LocalX = FMath::Cos(Angle) * Radius;
        const float LocalY = FMath::Sin(Angle) * Radius;

        Offsets.Add(FVector(LocalX, LocalY, 0.f));
    }
}

void FHerdFormationLogic::GenerateWedgeOffsets(TArray<FVector>& Offsets, int32 Num, float Spacing)
{
    // Member 0 is the "point" of the V, right at the center.
    if (Num > 0)
    {
        Offsets.Add(FVector::ZeroVector);
    }

    // i=1 is 1st right, i=2 is 1st left, i=3 is 2nd right, i=4 is 2nd left...
    for (int32 i = 1; i < Num; ++i)
    {
        // (i+1)/2 gives the "rank" or "step" back (1, 1, 2, 2, 3, 3...)
        const int32 Rank = (i + 1) / 2;
        
        // LocalX (Forward/Back)
        const float BackwardOffset = Rank * Spacing * -1.0f; // Negative X is "Backward"
        
        // LocalY (Left/Right)
        const float SideSign = (i % 2 == 1) ? 1.0f : -1.0f; // 1 = Right, -1 = Left
        const float SideOffset = Rank * Spacing * SideSign;

        Offsets.Add(FVector(BackwardOffset, SideOffset, 0.f));
    }
}


void FHerdFormationLogic::GenerateGridOffsets(TArray<FVector>& Offsets, int32 Num, float Spacing)
{
    // Try to make the grid as square as possible
    const int32 NumCols = FMath::CeilToInt(FMath::Sqrt(static_cast<float>(Num)));
    if (NumCols == 0) return; // Avoid division by zero

    // Calculate the total width of the formation to center it
    const float TotalWidth = (NumCols - 1) * Spacing;
    const float HalfWidth = TotalWidth / 2.0f;

    for (int32 i = 0; i < Num; ++i)
    {
        const int32 Row = i / NumCols;
        const int32 Col = i % NumCols;

        // LocalX (Forward/Back)
        const float BackwardOffset = Row * Spacing * -1.0f; // Negative X is "Backward"

        // LocalY (Left/Right)
        // (Col * Spacing) = position from left edge
        // - Half width = centers the entire row
        const float SideOffset = (Col * Spacing) - HalfWidth;

        Offsets.Add(FVector(BackwardOffset, SideOffset, 0.f));
    }
}