#include "../libk/pagetables.hh"

using namespace pagetables;
using namespace allocator;

extern BuddyAllocator k_allocator;

template<typename T>
auto PageDirectory<T>::map(u32 const va, u32 const pa, u8 const perm) -> Result<T> const {
    auto const idx = va_to_index(va);
    auto const& pt = _entries[idx];
    return pt.map(va, pa, perm);
}

template<typename T>
auto PageDirectory<T>::try_map(u32 const va, u32 const pa, u8 const perm) -> Result<T> const {
    auto const pd_idx = va_to_index(va);
    auto const& pd = _entries[pd_idx];
    if (pd.get_entry_address() == 0) {
        auto possible_pt = k_allocator.kmalloc(PAGESIZE);
        if (possible_pt.none()) {
            return { };
        }

        auto const pt = possible_pt.unwrap();
        auto const res = pd.add_pagetable(pt, perm);
        if (!res.ok) {
            k_allocator.kfree(pt);
        }
        
        auto ptr = reinterpret_cast<u32*>(pt);
        auto constexpr const num_entries = PAGESIZE / sizeof(signed);
        for (u16 i = 0; i < num_entries; ++i) {
            *ptr = 0;
            ++ptr;
        }
    }
    return pd.try_map(va, pa, perm);
}

template<typename T>
auto PageDirectory<T>::va_to_pa(uptr const addr) const -> Option<T> const {
    auto const i = va_to_index(addr);
    auto const pt = _entries[i].get_entry_pagetable();
    if (pt.none()) {
        return { };
    }
    auto const pa = pt.unwrap().va_to_pa(addr);
    return Option<T>(pa);
}

template<typename T>
auto PageDirectoryEntry<T>::add_pagetable(uptr const pt_addr, u8 const perm) -> Result<T> const {
    auto const entry_addr = get_entry_address();
    if (entry_addr != 0) [[unlikely]] {
        return { };
    }
    _data = pt_addr | PTE_P | PTE_W;
    return Result<T>(0);
}

template<typename T>
void PageDirectory<T>::set_page_directory() {
    __asm__ __volatile__(
        "mov %0, %%cr3;"
        :
        : "r"(this)
        : "memory");
}

void pagetables::enable_paging() {
    u32 cr0 = 0;
    __asm__ __volatile__(
        "mov %%cr0, %0;"
        "or $0x80000000, %0;"
        "mov %0, %%cr0;"
        : "=r"(cr0)
        : "0"(cr0)
        : "memory"
    );
}