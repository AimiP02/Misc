#pragma once

#include <optional>

#include "memory.hpp"

#define ADD_ADDRESS(pAddress, szModuleName, szSignature)                       \
  Memory::Address_t pAddress = Memory::FindPattern(szModuleName, szSignature)

namespace CTX {
struct Memory_t {
  struct {
    ADD_ADDRESS(pfnPresent, "gameoverlayrenderer64.dll",
                "4C 8D 05 ? ? ? ? 41 B9 ? ? ? ? 48 8D 15 ? ? ? ? E8 ? ? ? ? 48 "
                "8B 4F 50")
        .Jump(0x3)
        .Get();
    ADD_ADDRESS(pfnResizeBuffers, "gameoverlayrenderer64.dll",
                "48 8B 4F 68 4C 8D 05")
        .Jump(0x7)
        .Get();
  } IDXGISwapChain;

  struct {
    ADD_ADDRESS(pfnOnColorChanged, "client.dll",
                "E8 ? ? ? ? 80 BF ? ? ? ? ? 74 1E")
        .Jump();
  } C_BaseModelEntity;
};

inline std::optional<Memory_t> Memory =
    std::nullopt; // @note xnxkzeu: std::optional is used here to delay
                  // initialization.
} // namespace CTX

#undef ADD_ADDRESS