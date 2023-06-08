#pragma once
#include "int.hh"
#include "array.hh"
#include "option.hh"

auto constexpr const PAGESIZE = 0x1000;
auto constexpr const PTE_P    = 0b001;
auto constexpr const PTE_W    = 0b010;
auto constexpr const PTE_U    = 0b100;
auto constexpr const PTE_PWU  = PTE_P | PTE_W | PTE_U;
auto constexpr const PTE_PW   = PTE_P | PTE_W;
auto constexpr const PTE_PU   = PTE_P | PTE_U;

namespace pagetables {
    class PagetableEntry {
        public :
            PagetableEntry(PagetableEntry const& _) = delete;
            PagetableEntry() : _data(0) {}

            // [P] return whether this PTE is present
            [[nodiscard]] auto constexpr present()       const -> bool { return _data & 1; }

            // [R/W] return whether this PTE is writable
            [[nodiscard]] auto constexpr writable()      const -> bool { return _data & 0b10; }

            // [U/S] return whether this PTE is user-accessible
            [[nodiscard]] auto constexpr user()          const -> bool { return _data & 0b100; }
            
            // [PWT] return whether write-through caching or write-back is enabled
            [[nodiscard]] auto constexpr write_through() const -> bool { return _data & 0b1000; }
            // [PCD] return whether this page will be cached
            [[nodiscard]] auto constexpr cache_disable() const -> bool { return _data & 0b10000; }
            
            // [A] return whether this PTE was read during virtual address translation
            [[nodiscard]] auto constexpr accessed()      const -> bool { return _data & 0b100000; }
            // [D] return whether this page is dirty (has been written to)
            [[nodiscard]] auto constexpr dirty()         const -> bool { return _data & 0b1000000; }
            // [G] return whether to invalidate the TLB entry for this page upon `mov %cr3'
            [[nodiscard]] auto constexpr global()        const -> bool { return _data & 0b10000000; }
            // [PAT] return whether page attribute table is supported
            [[nodiscard]] auto constexpr using_pat()     const -> bool { return _data & 0b100000000; }

            // return the address of the page pointed to by this PTE (lower 24 bits)
            [[nodiscard]] auto constexpr get_entry_address()  const -> uptr { return _data & 0xFFFFFF00; }
        
            // create physical address from virtual address `addr' and permissions `perm'
            auto map(uptr addr, u8 perm) -> signed {
                _data = addr | perm;
                return 0;
            }

            // try to create physical address from virtual address `addr' and permissions `perm'
            [[nodiscard]] auto try_map(uptr addr, u8 perm) -> signed {
                return map(addr, perm);
            }

        private :
            u32 _data;
    }__attribute__((packed));

    class Pagetable {
        public :
            Pagetable(Pagetable const& _) = delete;
            Pagetable() {}

            // get index of the PTE with virtual address `addr' (bits 21-12)
            auto constexpr get_pagetable_index(u32 addr) const -> usize { return (addr & 0x3FF000) >> 12; }

            // get physical address of the PTE with virtual address `addr'
            auto constexpr va_to_pa(u32 addr) const -> uptr { auto idx = get_pagetable_index(addr); return _entries[idx].get_entry_address(); }

            constexpr PagetableEntry& operator [] (usize const idx) { return _entries[idx]; }
            constexpr const PagetableEntry& operator [] (usize const idx) const { return _entries[idx]; }

        private :
            static usize constexpr const NUM_ENTRIES = 1024;
            Array<PagetableEntry, NUM_ENTRIES> _entries;
    }__attribute__((aligned(PAGESIZE)));

    class PageDirectoryEntry {
        public :
            PageDirectoryEntry(PageDirectoryEntry const& _) = delete;
            PageDirectoryEntry() : _data(0) {}

            // [P] return whether this PDE is present
            [[nodiscard]] auto constexpr present()       const -> bool { return _data & 0b1; }

            // [R/W] return whether this PDE is writable
            [[nodiscard]] auto constexpr writable()      const -> bool { return _data & 0b10; }

            // [U/S] return whether this PDE is user-accessible
            [[nodiscard]] auto constexpr user()          const -> bool { return _data & 0b100; }
            
            // [PWT] return whether write-through caching or write-back is enabled
            [[nodiscard]] auto constexpr write_through() const -> bool { return _data & 0b1000; }

            // [PCD] return whether this pagetable will be cached
            [[nodiscard]] auto constexpr cache_disable() const -> bool { return _data & 0b10000; }
            
            // [A] return whether this PDE was read during virtual address translation
            [[nodiscard]] auto constexpr accessed()      const -> bool { return _data & 0b100000; }

            // return the address of the pagetable pointed to by this PDE (upper 24 bits)
            [[nodiscard]] auto constexpr get_entry_address()   const -> uptr { return _data & 0x0FFFFFF00; }

            // return reference to the pagetable pointed to by this PDE
            [[nodiscard]] auto constexpr get_entry_pagetable() const -> Option<Pagetable&> {
                auto const addr = get_entry_address();
                if (!addr) {
                    return Option<Pagetable&>();
                }
                auto const pt_addr = reinterpret_cast<Pagetable*>(addr);
                return Option<Pagetable&>(*pt_addr);
            }

            // map virtual address `va' to physical address `pa' with permissions `perm'
            auto map(u32 const va, u32 const pa, u8 const perm) -> signed {
                auto& pt = get_entry_pagetable().unwrap();
                auto& entry_page = pt[pt.get_pagetable_index(va)];
                return entry_page.map(pa, perm);
            }
            
            // try to map virtual address `va' to physical address `pa' with permissions `perm'
            [[nodiscard]] auto try_map(u32 const va, u32 const pa, u8 const perm) -> signed {
                auto possible_pt = get_entry_pagetable();
                if (possible_pt.none()) {
                    return -1;
                }
                auto& pt = possible_pt.unwrap();
                auto& entry_page = pt[pt.get_pagetable_index(va)];
                return entry_page.try_map(pa, perm);
            }

            auto add_pagetable(uptr const pt_addr, u8 const perm) -> signed;

        private :
            u32 _data;
    }__attribute__((packed));

    class PageDirectory {
        public :
            PageDirectory(PageDirectory const& _) = delete;
            PageDirectory() {}

            auto constexpr get_entry_directory(usize const idx) -> PageDirectoryEntry& { return _entries[idx]; }
            auto constexpr get_entry_directory(usize const idx) const -> PageDirectoryEntry const& { return _entries[idx]; }

            // map virtual address `va' to physical address `pa' with permissions `perm'
            auto map(u32 const va, u32 const pa, u8 const perm) -> signed;

            // try to map virtual address `va' to physical address `pa' with permissions `perm'
            [[nodiscard]] auto try_map(u32 const va, u32 const pa, u8 const perm) -> signed;

            // get index of the PDE with virtual address `addr' (bits 21-12)
            [[nodiscard]] auto constexpr va_to_index(uptr const addr) const -> usize { return (addr & 0xFFC00000) >> 22; }

            // get physical address of the PDE with virtual address `addr'
            auto va_to_pa(uptr const addr) const -> uptr;

            PageDirectoryEntry& operator [] (usize const idx) { return _entries[idx]; }
            PageDirectoryEntry const& operator [] (usize const idx) const { return _entries[idx]; }

            // set `%cr3' to address of page directory
            void set_page_directory() const;
        
        private :
            auto static constexpr const NUM_ENTRIES = 1024_usize;
            Array<PageDirectoryEntry, NUM_ENTRIES> _entries;
    }__attribute__((aligned(PAGESIZE)));

    // enable paging and protection bits in `%cr0'
    void enable_paging();
}