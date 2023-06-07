#include "../libk/allocator.hh"
#include "../libk/int.hh"

using namespace console;
using namespace allocator;

auto BuddyAllocator::kmalloc(usize const size) -> uptr {
    usize const size_aligned = (size + PAGE_SIZE - 1) & -PAGE_SIZE;
    if (size_aligned > (1 << 27)) [[unlikely]] {
        return 0;
    }

    u8 const PAGE_SIZE_msb = msb(PAGE_SIZE);
    u8 const size_msb = msb(size_aligned) - PAGE_SIZE_msb;

    for (usize i = size_msb; i < _allocated_blocks.len(); ++i) {
        auto const block = find_min_free(i);
        if (block) {
            uptr const block_addr = block->get_address();
            for (usize j = size_msb; j < i; ++j) {
                // dynamically allocate memory blocks as needed
                auto new_block = current_block;
                auto next_block = current_block->m_next;
                if (!next_block) [[unlikely]] {
                    next_block = kmalloc_next_block();
                    if (!next_block) [[unlikely]] {
                        return 0;
                    }
                }

                // check for overflow, set address of `new_block',
                //      and append it to `allocated_blocks[j]'
                u32 tmp;
                u32 const size = 1 << (j + PAGE_SIZE_msb);
                if (__builtin_add_overflow(block_addr, size, &tmp)) {
                    return 0;
                }
                new_block->set_address(block_addr + size);
                append_block(new_block, &_allocated_blocks[j]);

                // augment `current_block'
                current_block = next_block;
            }

            // remove `block' from `_allocated_blocks[i]'
            //      and append it to `_allocated_blocks[size_msb]'
            block->m_prev->m_next = block->m_next;
            if (block->m_next) {
                block->m_next->m_prev = block->m_prev;
            }
            block->set_address(block_addr);
            block->set_allocatable(false);
            block->set_size(size_msb);
            append_block(block, &_allocated_blocks[size_msb]);

            return block_addr;
        }
    }
    return 0;
}

void BuddyAllocator::kfree(uptr const addr) {
    u8 const PAGE_SIZE_msb = msb(PAGE_SIZE);

    // search `_allocated_blocks' for block with address `addr'
    for (usize i = 0; i < _allocated_blocks.len(); ++i) {
        auto iter_bl = _allocated_blocks[i].m_next;
        while (iter_bl) {
            auto const block_addr = iter_bl->get_address();
            if (block_addr == addr && !iter_bl->allocatable()) {
                auto const size = 1 << (iter_bl->get_size() + PAGE_SIZE_msb);
                iter_bl->set_allocatable(true);

                // search for free buddy in `_allocated_blocks[i]' with address `buddy_addr'
                auto const buddy_addr = ((block_addr - MIN_ADDRESS) ^ size) + MIN_ADDRESS;
                auto iter_bd = _allocated_blocks[i].m_next;
                while (iter_bd) {
                    if (iter_bd->get_address() == buddy_addr && iter_bd->allocatable()) {
                        // remove block from `_allocated_blocks[i]' 
                        //      and append to list rooted at `current_block for reuse
                        iter_bl->m_prev->m_next = iter_bl->m_next;
                        if (iter_bl->m_next) {
                            iter_bl->m_next->m_prev = iter_bl->m_prev;
                        }
                        append_block(iter_bl, current_block);

                        // coalesce up and append `iter_bd' at `_allocated_blocks[res_idx]'
                        auto const res_idx = coalesce(block_addr, i);
                        k_console.print(res_idx);
                        
                        auto const new_size = 1 << (res_idx + PAGE_SIZE_msb);
                        auto min_addr = (block_addr < buddy_addr) ? block_addr : buddy_addr;
                        auto const min_buddy_addr = ((min_addr - MIN_ADDRESS) ^ new_size) + MIN_ADDRESS;
                        min_addr = (min_addr < min_buddy_addr) ? min_addr : min_buddy_addr;

                        iter_bd->set_address(min_addr);
                        iter_bd->set_allocatable(true);
                        append_block(iter_bd, &_allocated_blocks[res_idx]);

                        return;
                    }
                    iter_bd = iter_bd->m_next;
                }
            }
            iter_bl = iter_bl->m_next;
        }
    }
}

auto BuddyAllocator::coalesce(uptr const addr, u8 const idx) -> u8 {
    auto const PAGE_SIZE_msb = msb(PAGE_SIZE);

    // coalesce up
    for (usize i = idx; i < _allocated_blocks.len(); ++i) {
        usize const size = 1 << (i + PAGE_SIZE_msb);
        auto const buddy_addr = ((addr - MIN_ADDRESS) ^ size) + MIN_ADDRESS;

        // find buddy to coalesce in `_allocated_blocks[i]'
        auto iter = &_allocated_blocks[i];
        while (iter) {
            if (iter->get_address() == buddy_addr && iter->allocatable()) {
                // remove buddy from `_allocated_blocks[i]' 
                //      and append to list rooted at `current_block' for reuse
                iter->m_prev->m_next = iter->m_next;
                if (iter->m_next) {
                    iter->m_next->m_prev = iter->m_prev;
                }
                iter->set_address(0);
                iter->set_allocatable(true);
                iter->set_size(0);
                append_block(iter, current_block);
                break;
            }
            iter = iter->m_next;
        }
        // cannot find (free) buddy to coalesce
        if (!iter) {
            return i;
        }
    }
    return _allocated_blocks.len() - 1;
}

auto BuddyAllocator::kmalloc_next_block() -> Block* {
    // allocate `PAGE_SIZE' / sizeof(`Block') more blocks
    auto const k = kmalloc(PAGE_SIZE);
    if (!k) [[unlikely]] {
        return nullptr;
    }

    // set all new blocks created as allocatable
    auto const new_blocks = reinterpret_cast<Block*>(k);
    auto const num_blocks = PAGE_SIZE / sizeof(Block);
    for (usize i = 1; i < num_blocks; ++i) {
        new_blocks[i].set_allocatable(true);
        new_blocks[i].m_prev = &new_blocks[i - 1];
        new_blocks[i - 1].m_next = &new_blocks[i];
    }
    new_blocks[0].set_allocatable(true);
    new_blocks[0].m_prev = current_block;
    current_block->m_next = &new_blocks[0];
    return current_block->m_next;
}

auto BuddyAllocator::find_min_free(u8 const idx) -> Block* {
    uptr min_addr = uptr(-1);
    Block* min_block = nullptr;

    auto iter = _allocated_blocks[idx].m_next;
    while (iter) {
        auto const addr = iter->get_address();
        if (addr < min_addr && iter->allocatable()) {
            min_addr = addr;
            min_block = iter;
        }
        iter = iter->m_next;
    }
    return min_block;
}

void BuddyAllocator::append_block(Block* block, Block* root) {
    auto iter = root;
    while (iter->m_next) {
        iter = iter->m_next;
    }
    block->m_prev = iter;
    block->m_next = nullptr;
    iter->m_next = block;
}