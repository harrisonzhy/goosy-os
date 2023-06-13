#include "../libk/allocator.hh"
#include "../libk/int.hh"

using namespace console;
using namespace allocator;

auto BuddyAllocator::kmalloc(usize const size) -> uptr {
    usize const req_aligned = (size + PAGE_SIZE - 1) & -PAGE_SIZE;
    if (req_aligned > MAX_ALLOC_SIZE) [[unlikely]] {
        return 0;
    }

    u8 const PAGE_SIZE_msb = msb(PAGE_SIZE);
    u8 const size_msb = msb(req_aligned) - PAGE_SIZE_msb;
    for (u8 i = size_msb; i < NUM_ENTRIES_ALLOC; ++i) {
        auto const bl = find_min_free(i);
        if (bl) {
            uptr const bl_addr = bl->address();
            u32 tmp_res;
            if (__builtin_add_overflow(bl_addr, MIN_ADDRESS, &tmp_res)) {
                return 0;
            }
            
            for (u8 j = size_msb; j < i; ++j) {
                // dynamically allocate memory blocks
                auto new_bl = current_block;
                auto next_bl = current_block->m_next;
                if (!next_bl) [[unlikely]] {
                    next_bl = kmalloc_next_block();
                    if (!next_bl) [[unlikely]] {
                        return 0;
                    }
                }
                u32 tmp_addr;
                u32 const size = 1 << (j + PAGE_SIZE_msb);
                if (__builtin_add_overflow(bl_addr, size, &tmp_addr)) {
                    return 0;
                }

                // set buddy address and append to `allocated_blocks[j]'
                new_bl->set_address(bl_addr + size);
                append_block(new_bl, &_allocated_blocks[j]);
                current_block = next_bl;
            }

            // move `block' from `_allocated_blocks[i -> req_msb]'
            bl->m_prev->m_next = bl->m_next;
            if (bl->m_next) {
                bl->m_next->m_prev = bl->m_prev;
            }
            bl->set_address(bl_addr);
            bl->set_allocatable(false);
            append_block(bl, &_allocated_blocks[size_msb]);

            return bl_addr + MIN_ADDRESS;
        }
    }
    return 0;
}

void BuddyAllocator::kfree(uptr const addr) {
    u8 const PAGE_SIZE_msb = msb(PAGE_SIZE);
    uptr const req_addr = addr - MIN_ADDRESS;

    // search for block with address `addr'
    for (u8 i = 0; i < NUM_ENTRIES_ALLOC; ++i) {
        auto it_bl = _allocated_blocks[i].m_next;
        while (it_bl) {
            auto const bl_addr = it_bl->address();
            if (bl_addr == req_addr && !it_bl->allocatable()) {
                usize const size = 1 << (i + PAGE_SIZE_msb);
                it_bl->set_allocatable(true);

                // search for free buddy with address `buddy_addr'
                auto const bd_addr = bl_addr ^ size;
                auto it_bd = _allocated_blocks[i].m_next;
                while (it_bd) {
                    if (it_bd->address() == bd_addr && it_bd->allocatable()) {
                        // reuse freed block from `_allocated_blocks[i]'
                        it_bl->m_prev->m_next = it_bl->m_next;
                        if (it_bl->m_next) {
                            it_bl->m_next->m_prev = it_bl->m_prev;
                        }
                        append_block(it_bl, current_block);

                        // coalesce up and append `it_bd' at `_allocated_blocks[res_idx]'
                        auto const res_idx = coalesce(bl_addr, i);
                        auto min_addr = (bl_addr < bd_addr) ? bl_addr : bd_addr;
                        it_bd->set_address(min_addr);
                        it_bd->set_allocatable(true);
                        append_block(it_bd, &_allocated_blocks[res_idx]);
                        
                        return;
                    }
                    it_bd = it_bd->m_next;
                }
            }
            it_bl = it_bl->m_next;
        }
    }
}

auto BuddyAllocator::coalesce(uptr const addr, u8 const idx) -> u8 {
    auto const PAGE_SIZE_msb = msb(PAGE_SIZE);

    // coalesce up
    uptr next_addr = addr;
    for (u8 i = idx; i < NUM_ENTRIES_ALLOC; ++i) {
        usize const size = 1 << (i + PAGE_SIZE_msb);
        uptr const bd_addr = next_addr ^ size;
        next_addr = (next_addr < bd_addr) ? next_addr : bd_addr;

        // find buddy to coalesce
        auto it = &_allocated_blocks[i];
        while (it) {
            if (it->address() == bd_addr && it->allocatable()) {
                // reuse freed block from `_allocated_blocks[i]'
                it->m_prev->m_next = it->m_next;
                if (it->m_next) {
                    it->m_next->m_prev = it->m_prev;
                }
                it->set_address(0);
                it->set_allocatable(true);
                break;
            }
            it = it->m_next;
        }
        // cannot find (free) buddy to coalesce
        if (!it) {
            return i;
        }
    }
    return NUM_ENTRIES_ALLOC - 1;
}

auto BuddyAllocator::kmalloc_next_block() -> Block* {
    auto const k = kmalloc(PAGE_SIZE);
    if (!k) [[unlikely]] {
        return nullptr;
    }

    // link new blocks
    u16 constexpr const num_bl = PAGE_SIZE / sizeof(Block);
    auto const new_bl = reinterpret_cast<Block*>(k);
    for (u16 i = 1; i < num_bl; ++i) {
        new_bl[i].m_prev = &new_bl[i - 1];
        new_bl[i - 1].m_next = &new_bl[i];
    }
    new_bl[0].m_prev = current_block;
    current_block->m_next = &new_bl[0];
    return current_block->m_next;
}

auto BuddyAllocator::find_min_free(u8 const idx) -> Block* {
    uptr min_addr = UINTPTR_MAX;
    Block* min_bl = nullptr;
    auto it = _allocated_blocks[idx].m_next;
    while (it) {
        auto const addr = it->address();
        if (addr < min_addr && it->allocatable()) {
            min_addr = addr;
            min_bl = it;
        }
        it = it->m_next;
    }
    return min_bl;
}

void BuddyAllocator::append_block(Block* block, Block* root) {
    auto it = root;
    while (it->m_next) {
        it = it->m_next;
    }
    block->m_prev = it;
    block->m_next = nullptr;
    it->m_next = block;
}

void BuddyAllocator::print_memory_map() {
    k_console.switch_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
    k_console.print("MEMORY MAP\n");
    k_console.switch_color(Console::DEFAULT_FG, Console::DEFAULT_BG);
    for (u8 i = 0; i < 10; ++i) {
        auto it = _allocated_blocks[i].m_next;
        k_console.print(i, "  ");
        while (it) {
            k_console.print("[", it->allocatable(), "]");
            it = it->m_next;
        }
        k_console.print("\n");
    }
    for (u8 i = 10; i < NUM_ENTRIES_ALLOC; ++i) {
        auto it = _allocated_blocks[i].m_next;
        k_console.print(i, " ");
        while (it) {
            k_console.print("[", it->allocatable(), "]");
            it = it->m_next;
        }
        k_console.print("\n");
    }
}