#pragma once
#include "int.hh"
#include "array.hh"
#include "option.hh"

static auto constexpr const PAGESIZE = 0x1000;
static auto constexpr const PTE_P    = 0b001;
static auto constexpr const PTE_W    = 0b010;
static auto constexpr const PTE_U    = 0b100;
static auto constexpr const PTE_PWU  = PTE_P | PTE_W | PTE_U;
static auto constexpr const PTE_PW   = PTE_P | PTE_W;
static auto constexpr const PTE_PU   = PTE_P | PTE_U;

namespace pagetables {
    class PagetableEntry {
        public :
            PagetableEntry(const PagetableEntry& _) = delete;
            PagetableEntry() : _data(0) {}

            // [P] returns whether this PTE is present
            [[nodiscard]] auto constexpr present()       const -> bool { return _data & 0b000000001; }
            // [R/W] returns whether this PTE is writable
            [[nodiscard]] auto constexpr writable()      const -> bool { return _data & 0b000000010; }
            // [U/S] returns whether this PTE is user-accessible
            [[nodiscard]] auto constexpr user()          const -> bool { return _data & 0b000000100; }
            
            // [PWT] returns whether write-through caching or write-back is enabled
            [[nodiscard]] auto constexpr write_through() const -> bool { return _data & 0b000001000; }
            // [PCD] returns whether this page will be cached
            [[nodiscard]] auto constexpr cache_disable() const -> bool { return _data & 0b000010000; }
            
            // [A] returns whether this PTE was read during virtual address translation
            [[nodiscard]] auto constexpr accessed()      const -> bool { return _data & 0b000100000; }
            // [D] returns whether this page is dirty (has been written to)
            [[nodiscard]] auto constexpr dirty()         const -> bool { return _data & 0b001000000; }
            // [G] returns whether to invalidate the TLB entry for this page upon `mov %cr3'
            [[nodiscard]] auto constexpr global()        const -> bool { return _data & 0b010000000; }
            // [PAT] returns whether page attribute table is supported
            [[nodiscard]] auto constexpr using_pat()     const -> bool { return _data & 0b100000000; }

            // returns the address of the page pointed to by this PTE (upper 24 bits)
            [[nodiscard]] auto constexpr get_entry_address()  const -> uptr { return _data & 0x0FFFFFF00; }
        
            // creates physical address from virtual address `addr' and permissions `perm'
            auto map(uptr addr, u8 perm) -> signed {
                _data = addr;
                _data |= perm;
                return 0;
            }

            // tries to create physical address from virtual address `addr' and permissions `perm'
            [[nodiscard]] auto try_map(uptr addr, u8 perm) -> signed {
                return map(addr, perm);
            }

        private :
            u32 _data = 0;
    }__attribute__((packed));

    class Pagetable {
        public :
            Pagetable(const Pagetable& _) = delete;
            Pagetable() {}

            // get index of the PTE with virtual address `addr' (bits 21-12)
            auto constexpr get_pagetable_index(u32 addr) const -> usize { return (addr & 0x3FF000) >> 12; }

            // get physical address of the PTE with virtual address `addr'
            auto constexpr va_to_pa(u32 addr) const -> uptr { return _entries[get_pagetable_index(addr)].get_entry_address(); }

            constexpr PagetableEntry& operator [] (const usize i) { return _entries[i]; }
            constexpr const PagetableEntry& operator [] (const usize i) const { return _entries[i]; }

        private :
            static usize constexpr const NUM_ENTRIES = 1024;
            Array<PagetableEntry, NUM_ENTRIES> _entries;
    }__attribute__((aligned(PAGESIZE)));

    class PageDirectoryEntry {
        public :
            PageDirectoryEntry(const PageDirectoryEntry& _) = delete;
            PageDirectoryEntry() : _data(0) {}

            // [P] returns whether this PDE is present
            [[nodiscard]] auto constexpr present()       const -> bool { return _data & 0b000000001; }
            // [R/W] returns whether this PDE is writable
            [[nodiscard]] auto constexpr writable()      const -> bool { return _data & 0b000000010; }
            // [U/S] returns whether this PDE is user-accessible
            [[nodiscard]] auto constexpr user()          const -> bool { return _data & 0b000000100; }
            
            // [PWT] returns whether write-through caching or write-back is enabled
            [[nodiscard]] auto constexpr write_through() const -> bool { return _data & 0b000001000; }
            // [PCD] returns whether this pagetable will be cached
            [[nodiscard]] auto constexpr cache_disable() const -> bool { return _data & 0b000010000; }
            
            // [A] returns whether this PDE was read during virtual address translation
            [[nodiscard]] auto constexpr accessed()      const -> bool { return _data & 0b000100000; }

            // returns the address of the pagetable pointed to by this PDE (upper 24 bits)
            [[nodiscard]] auto constexpr get_entry_address()   const -> uptr { return _data & 0x0FFFFFF00; }

            // returns reference to the pagetable pointed to by this PDE
            [[nodiscard]] auto constexpr get_entry_pagetable() const -> Option<Pagetable&> {
                const auto pt_addr = get_entry_address();
                if (!pt_addr) {
                    return Option<Pagetable&>();
                }
                return Option<Pagetable&>(*(Pagetable*)pt_addr);
            }

            // maps virtual address `va' to physical address `pa'
            auto map(const u32 va, const u32 pa, const u8 perm) -> signed {
                auto& pt = get_entry_pagetable().unwrap();
                auto& entry_page = pt[pt.get_pagetable_index(va)];
                return entry_page.map(pa, perm);
            }
            
            // tries to map virtual address `va' to physical address `pa'. returns -1 on failure and 0 otherwise
            [[nodiscard]] auto try_map(const u32 va, const u32 pa, const u8 perm) -> signed {
                auto possible_pt = get_entry_pagetable();
                if (possible_pt.none()) {
                    return -1;
                }
                auto& pt = possible_pt.unwrap();
                auto& entry_page = pt[pt.get_pagetable_index(va)];
                return entry_page.try_map(pa, perm);
            }

            auto add_pagetable(const uptr pt_addr, const u8 perm) -> signed;

        private :
            u32 _data = 0;
    }__attribute__((packed));

    class PageDirectory {
        public :
            PageDirectory(const PageDirectory& _) = delete;
            PageDirectory() {}

            auto constexpr get_entry_directory(const usize i) -> PageDirectoryEntry& { return _entries[i]; }
            auto constexpr get_entry_directory(const usize i) const -> const PageDirectoryEntry& { return _entries[i]; }

            // maps virtual address `va' to physical address `pa'
            auto map(const u32 va, const u32 pa, const u8 perm) -> signed;

            // tries to map virtual address `va' to physical address `pa'. returns -1 on failure and 0 otherwise
            [[nodiscard]] auto try_map(const u32 va, const u32 pa, const u8 perm) -> signed;

            // get index of the PDE with virtual address `addr' (bits 21-12)
            [[nodiscard]] auto constexpr va_to_index(const uptr addr) const -> usize { return (addr & 0xFFC00000) >> 22; }

            // get physical address of the PDE with virtual address `addr'
            auto va_to_pa(const uptr addr) const -> uptr;

            void set_page_directory() const;

            PageDirectoryEntry& operator [] (const usize i) { return _entries[i]; }
            const PageDirectoryEntry& operator [] (const usize i) const { return _entries[i]; }
        
        private :
            static auto constexpr const NUM_ENTRIES = 1024_usize;
            Array<PageDirectoryEntry, NUM_ENTRIES> _entries;
    }__attribute__((aligned(PAGESIZE)));

    void enable_paging();
}