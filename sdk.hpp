#pragma once
#include <Windows.h>
#include <cstdint>

struct FVector {
    float X, Y, Z;
};

struct FRotator {
    float Pitch, Yaw, Roll;
};

struct USceneComponent {
    char pad[0x11C];
    FVector RelativeLocation;
};

struct AActor {
    char pad[0x140];
    USceneComponent* RootComponent;
};

struct APlayerController {
    char pad[0x248];
    FVector RotationInput;
};

struct UWorld {
    char pad[0x30];
    uintptr_t PersistentLevel;
    uintptr_t OwningGameInstance;
};