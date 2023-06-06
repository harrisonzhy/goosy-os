#include "../libk/allocator.hh"
#include "../libk/int.hh"

using namespace console;
using namespace allocator;

auto BuddyAllocator::kmalloc(usize const size) -> uptr {
    usize const size_aligned = (size + PAGE_SIZE - 1) & -PAGE_SIZE;
    usize const size_msb = msb(size_aligned) - msb(PAGE_SIZE);

    for (usize i = size_msb; i < _free_blocks.len(); ++i) {
        auto const block = _free_blocks[i].m_next;
        if (block) {
            uptr const block_addr = block->get_address() + MIN_ADDRESS;
            for (usize j = size_msb; j < i; ++j) {
                // insert `new_block' at beginning of list at `_free_blocks[j]'
                auto new_block = current_block;
                auto next_block = current_block->m_next;
                if (!next_block) [[unlikely]] {
                    next_block = kmalloc_next_block();
                    if (!next_block) {
                        return 0;
                    }
                }

                new_block->m_prev = &_free_blocks[j];
                new_block->m_next = _free_blocks[j].m_next;
                if (_free_blocks[j].m_next) {
                    _free_blocks[j].m_next->m_prev = new_block;
                }
                _free_blocks[j].m_next = new_block;
                new_block->set_address(block_addr + (1 << j) * PAGE_SIZE);

                // augment `current_block'
                current_block = next_block;
            }

            // remove `block' from `_free_blocks[i]' 
            //      and append `block' to `_allocated_blocks'
            _free_blocks[i].m_next = nullptr;
            block->set_address(block_addr);
            block->set_allocatable(false);
            block->set_size(size_msb);
            append_block(block);

            return block_addr;
        }
    }
    return 0;
}

void BuddyAllocator::kfree(uptr const addr) {
    auto const buddy_addr = 0;

    auto iter_block = &_memory_blocks[0];
    while (iter_block) {
        if (iter_block->get_address() == addr) {
            // delete `iter_block' from `_memory_blocks'
            
            // insert `new_block' at beginning of list at `_free_blocks[j]'
        }
    }
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
        new_blocks[i].m_prev = &new_blocks[i - 1];
        new_blocks[i - 1].m_next = &new_blocks[i];
    }
    new_blocks[0].set_allocatable(true);
    new_blocks[0].m_prev = current_block;
    current_block->m_next = &new_blocks[0];
    return current_block->m_next;
}

auto BuddyAllocator::find_min_free(u8 const i) -> Block* {
    uptr min_addr = UINTPTR_MAX;
    Block* min_block = nullptr;

    auto iter_block = _free_blocks[i].m_next;
    while (iter_block) {
        if (iter_block->get_address() < min_addr && iter_block->allocatable()) {
            min_addr = iter_block->get_address();
            min_block = iter_block;
        }
        iter_block = iter_block->m_next;
    }
    return min_block;
}

void BuddyAllocator::append_block(Block* block) {
    auto seek_iter = &_allocated_blocks;
    while (seek_iter->m_next) {
        seek_iter = seek_iter->m_next;
    }
    block->m_prev = seek_iter;
    block->m_next = nullptr;
    seek_iter->m_next = block;
}