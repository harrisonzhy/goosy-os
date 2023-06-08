#include "../libk/pagetables.hh"
#include "../libk/allocator.hh"
#include "../libk/int.hh"

using namespace pagetables;
using namespace allocator;

extern BuddyAllocator k_allocator;

auto PageDirectory::map(u32 const va, u32 const pa, u8 const perm) -> signed {
    auto idx = va_to_index(va);
    auto& pt = _entries[idx];
    return pt.map(va, pa, perm);
}

auto PageDirectory::try_map(u32 const va, u32 const pa, u8 const perm) -> signed {
    auto const idx = va_to_index(va);
    auto& pt = _entries[idx];
    if (pt.get_entry_address() == 0) {
        auto const k = k_allocator.kmalloc(PAGESIZE);
        if (!k) {
            return -1;
        }
        
        // TODO: buddy allocation
        // 1. kalloc a page
        // 2. if null, return -1
        // 3. if adding pagetable fails, free everything and return -1
        // 4. zero pages
        // 5. return pd.trymap(va, pa, perm)
    }
    return -1;
}

auto PageDirectory::va_to_pa(uptr const addr) const -> uptr {
    auto const i = va_to_index(addr);
    auto pt = _entries[i].get_entry_pagetable();
    if (pt.none()) {
        return uptr(-1);
    }
    return pt.unwrap().va_to_pa(addr);
}

auto PageDirectoryEntry::add_pagetable(uptr const pt_addr, u8 const perm) -> signed {
    auto const entry_addr = get_entry_address();
    if (entry_addr != 0) [[unlikely]] {
        return -1;
    }
    _data = pt_addr | PTE_P | PTE_W;
    return 0;
}

void PageDirectory::set_page_directory() const {
    asm volatile(
        "mov %0, %%cr3;"
        :
        : "r"(this)
        : "memory"
    );
}

void pagetables::enable_paging() {
    u32 cr0 = 0;
    asm volatile(
        "mov %%cr0, %0;"
        "or $0x80000000, %0;"
        "mov %0, %%cr0;"
        : "=r"(cr0)
        : "0"(cr0)
        : "memory"
    );
}