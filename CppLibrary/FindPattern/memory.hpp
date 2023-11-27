#include <cstddef>
#include <cstdint>
#include <optional>
#include <string_view>
#include <type_traits>
#include <vector>

namespace Memory {
template <typename tPointer> struct AddressBase_t {
  tPointer m_pAddress = 0;

  AddressBase_t() : m_pAddress{} {}

  AddressBase_t(tPointer pAddress) : m_pAddress(pAddress) {}

  AddressBase_t(tPointer *pAddress) : m_pAddress(tPointer(pAddress)) {}

  AddressBase_t(void *pAddress) : m_pAddress(tPointer(pAddress)) {}

  AddressBase_t(const void *pAddress) : m_pAddress(tPointer(pAddress)) {}

  ~AddressBase_t() = default;

  inline operator tPointer() const { return m_pAddress; }

  inline operator void *() const {
    return reinterpret_cast<void *>(m_pAddress);
  }

  template <typename tReturn = tPointer> inline tReturn Cast() const {
    return tReturn(m_pAddress);
  }

  template <typename tReturn = AddressBase_t<tPointer>>
  inline tReturn Get(std::uint8_t uDereferenceCount = 1) const {
    tPointer pReturnAddress = m_pAddress;

    while (uDereferenceCount--)
      if (pReturnAddress)
        pReturnAddress = *reinterpret_cast<tPointer *>(pReturnAddress);

    return tReturn(pReturnAddress);
  }

  template <typename tReturn = AddressBase_t<tPointer>>
  inline tReturn Offset(std::ptrdiff_t nOffset) const {
    return tReturn(m_pAddress + nOffset);
  }

  template <typename tReturn = AddressBase_t<tPointer>>
  inline tReturn Jump(std::ptrdiff_t nOffset = 0x1) const {
    tPointer pReturnAddress = m_pAddress + nOffset;

    std::int32_t nDisplacement =
        *reinterpret_cast<std::int32_t *>(pReturnAddress);

    return tReturn(pReturnAddress + nDisplacement + sizeof(std::uint32_t));
  }

  template <typename tReturn = AddressBase_t<tPointer>>
  inline tReturn FindOpcode(std::uint8_t uOpcode,
                            std::ptrdiff_t nOffset = 0x0) const {
    tPointer pReturnAddress = m_pAddress;

    std::uint8_t uOpcodeAtAddress = 0;
    while ((uOpcodeAtAddress =
                *reinterpret_cast<std::uint8_t *>(pReturnAddress))) {
      if (uOpcodeAtAddress == uOpcode)
        break;

      pReturnAddress += 1;
    }

    pReturnAddress += nOffset;

    return tReturn(pReturnAddress);
  }
};

using Address_t = AddressBase_t<std::uintptr_t>;

Address_t FindPattern(const std::string_view szModuleName,
                      const std::string_view szPattern);
Address_t FindPattern(const Address_t pRegionStart,
                      const std::size_t unRegionSize,
                      const std::string_view szPattern);

Address_t GetModuleBaseAddress(const std::string_view szModuleName);
Address_t GetExportAddress(const Address_t pModuleBase,
                           const std::string_view szProcedureName);

std::vector<std::optional<std::uint8_t>>
PatternToBytes(const std::string_view szPattern);
std::string BytesToPattern(const std::uint8_t *pBytes, const std::size_t uSize);

template <typename tReturn, std::size_t nIndex, typename... tArgs>
inline tReturn CallVFunc(void *pInstance, tArgs... argList) {
  using fnVirtual = tReturn(__thiscall *)(void *, tArgs...);
  return (*static_cast<fnVirtual **>(pInstance))[nIndex](pInstance, argList...);
}
} // namespace Memory

#define VFUNC(nIndex, fnVirtual, tArgs, ...)                                   \
  auto fnVirtual { return Memory::CallVFunc<__VA_ARGS__, nIndex> tArgs; }
#define OFFSET(nOffset, varName, ...)                                          \
  std::add_lvalue_reference_t<__VA_ARGS__> varName() {                         \
    return *reinterpret_cast<std::add_pointer_t<__VA_ARGS__>>(                 \
        reinterpret_cast<std::uintptr_t>(this) + nOffset);                     \
  }