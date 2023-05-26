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
            current_block->set_size(aligned_size_log);
            current_block->set_allocatable(false);
            current_block = current_block->m_next;

            // dynamically allocate blocks if needed
            if (!current_block->m_next && current_block->m_prev) {
                current_block->m_next = kmalloc_next_block();
                if (!current_block->m_next) {
                    return uptr(-1);
                }
                current_block = current_block->m_next;
            }
            
            _free_blocks[s_i].m_allocatable = false;
            return uptr(alloc_addr);
        }
    }
    return uptr(-1);
}

auto BuddyAllocator::kfree(const uptr addr) -> signed {

    return 0;
}

void BuddyAllocator::coalesce(Block* block) {
}

auto BuddyAllocator::kmalloc_next_block() -> Block* {
    // allocate `PAGE_SIZE' / sizeof(`Block') more blocks
    const auto k = kmalloc(PAGE_SIZE);
    if (k > MAX_ADDRESS) [[unlikely]] {
        return nullptr;
    }

    // set all new_blocks created as allocatable
    const auto new_blocks = reinterpret_cast<Block*>(k);
    const auto num_blocks = PAGE_SIZE / sizeof(Block);
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