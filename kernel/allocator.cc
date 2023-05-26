#include "../libk/allocator.hh"
#include "../libk/int.hh"

using namespace console;
using namespace allocator;

auto BuddyAllocator::kmalloc(usize const size) -> uptr {
    if (size >= (1 << 27) || size == 0) [[unlikely]] {
        return uptr(-1);
    }

    usize const aligned_size = (size + PAGE_SIZE - 1) & -PAGE_SIZE;
    usize const aligned_size_log = log_two_ceil(aligned_size) - log_two_ceil(PAGE_SIZE);

    for (usize i = aligned_size_log; i < _free_blocks.len(); ++i) {
        if (_free_blocks[i].m_allocatable) {
            auto const s_i = i;
            auto const alloc_addr = _free_blocks[s_i].m_address;

            usize curr_block_size = (1 << s_i) * PAGE_SIZE;
            for (; curr_block_size >= aligned_size; --i) {
                _free_blocks[i].m_address = alloc_addr + curr_block_size;
                _free_blocks[i].m_allocatable = true;
                curr_block_size >>= 1;
            }

            // allocate new block and augment `current_block'
            current_block->set_address(alloc_addr);
            current_block->set_size(aligned_size_log);
            current_block->set_allocatable(false);
            current_block = current_block->m_next;

            // dynamically allocate blocks if needed
            if (!current_block->m_next && current_block->m_prev) {
                current_block->m_next = kmalloc_next_block();
                if (!current_block->m_next) [[unlikely]] {
                    return uptr(0);
                }
                current_block = current_block->m_next;
            }

            _free_blocks[s_i].m_allocatable = false;
            return uptr(alloc_addr);
        }
    }
    return uptr(0);
}

void BuddyAllocator::kfree(uptr const addr) {
    if (!check_address(addr)) {
        return;
    }

    auto iter_block = &_memory_blocks[0];
    while (iter_block) {
        auto const block_addr = iter_block->get_address();
        if (block_addr == addr) {
            auto const buddy_addr = ((block_addr - MIN_ADDRESS) ^ iter_block->get_size()) + MIN_ADDRESS;
            auto iter_buddy = &_memory_blocks[0];

            // try to find and coalesce buddy block
            while (iter_buddy) {
                // TODO: fix possibly wrong condition
                if (iter_buddy->get_address() == buddy_addr) {
                    // coalesce down
                    auto const s_i = iter_block->get_size() - log_two_ceil(PAGE_SIZE) + 0;
                    _free_blocks[s_i].m_address = (block_addr < buddy_addr) ? block_addr : buddy_addr;                    
                    kcoalesce(s_i);

                    // free buddy
                    iter_buddy->set_address(0);
                    iter_buddy->set_size(0);
                    iter_buddy->set_allocatable(true);
                    
                    // reinsert block into `_memory_blocks'
                    auto const next_block_buddy = current_block->m_next;
                    iter_buddy->m_prev = current_block;
                    current_block->m_next = iter_buddy;
                    next_block_buddy->m_prev = iter_buddy;
                    iter_buddy->m_next = next_block_buddy;
                }
                iter_buddy = iter_buddy->m_next;
            }
            // free block
            iter_block->set_address(0);
            iter_block->set_size(0);
            iter_block->set_allocatable(true);
            
            // reinsert block into `_memory_blocks'
            auto const next_block = current_block->m_next;
            iter_block->m_prev = current_block;
            current_block->m_next = iter_block;
            next_block->m_prev = iter_block;
            iter_block->m_next = next_block;
        }
        iter_block = iter_block->m_next;
    }
}

void BuddyAllocator::kcoalesce(u8 const s_i) {
    for (usize i = s_i + 1; i < _free_blocks.len(); ++i) {
        if (!_free_blocks[i].m_allocatable) {
            _free_blocks[i].m_allocatable = true;
            --i;
            for (; i >= s_i; --i) {
                _free_blocks[i].m_allocatable = false;
            }
            break;
        }
    }
}

auto BuddyAllocator::kmalloc_next_block() -> Block* {
    // allocate `PAGE_SIZE' / sizeof(`Block') more blocks
    auto const k = kmalloc(PAGE_SIZE);
    if (k > MAX_ADDRESS) [[unlikely]] {
        return nullptr;
    }

    // set all new_blocks created as allocatable
    auto const new_blocks = reinterpret_cast<Block*>(k);
    auto const num_blocks = PAGE_SIZE / sizeof(Block);
    for (usize i = 1; i < num_blocks; ++i) {
        new_blocks[i].set_allocatable(true);
        new_blocks[i].set_size(0);
        new_blocks[i].m_prev = &new_blocks[i - 1];
        new_blocks[i - 1].m_next = &new_blocks[i];
    }
    new_blocks[0].set_allocatable(true);
    new_blocks[0].set_size(0);
    new_blocks[0].m_prev = current_block;
    current_block->m_next = &new_blocks[0];
    return current_block->m_next;
}