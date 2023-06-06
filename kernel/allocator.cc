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
    // auto const buddy_addr = ((block_addr - MIN_ADDRESS) ^ iter_block->get_size()) + MIN_ADDRESS;
    auto iter = &_memory_blocks[0];
    while (iter) {
        auto const iter_addr = iter->get_address();
        if (iter_addr == addr) {
            auto const size_msb = iter->get_size();

            // remove block from `_allocated_memory'
            iter->m_prev->m_next = iter->m_next;
            iter->m_next->m_prev = iter->m_prev;

            auto const buddy_addr = ((addr - MIN_ADDRESS) ^ iter->get_size()) + MIN_ADDRESS;
            auto iter_buddy = &_allocated_blocks[size_msb];
            while (iter_buddy) {
                if (iter_buddy->get_address() == addr) {
                    auto const min_addr = (iter_addr < buddy_addr) ? iter_addr : buddy_addr;

                }
            }
            return;
        }
        iter = iter->m_next;
    }
}

void BuddyAllocator::coalesce(uptr const addr, u8 const idx) {

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

void BuddyAllocator::append_block(Block* block, Block* list) {
    auto iter = list;
    while (iter->m_next) {
        iter = iter->m_next;
    }
    block->m_prev = iter;
    block->m_next = nullptr;
    iter->m_next = block;
}