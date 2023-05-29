#include "../libk/allocator.hh"
#include "../libk/int.hh"

using namespace console;
using namespace allocator;

auto BuddyAllocator::kmalloc(usize const size) -> uptr {
    if (size >= (1 << 27) || size == 0) [[unlikely]] {
        return uptr(0);
    }

    usize const aligned_size = (size + PAGE_SIZE - 1) & -PAGE_SIZE;
    usize const aligned_size_log = log_two_ceil(aligned_size) - log_two_ceil(PAGE_SIZE);
    k_console.print("sz: ", aligned_size_log, "\n");

    for (usize i = aligned_size_log; i < _free_blocks.len(); ++i) {
        if (_free_blocks[i].m_allocatable) {
                k_console.print(i, "\n");

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

auto BuddyAllocator::kmalloc_next_block() -> Block* {
    // allocate `PAGE_SIZE' / sizeof(`Block') more blocks
    auto const k = kmalloc(PAGE_SIZE);
    if (!k) [[unlikely]] {
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

void BuddyAllocator::kfree(uptr const addr) {
    // find block in `_memory_blocks' with address `addr' and coalesce
    auto iter_block = &_memory_blocks[0];
    while (iter_block) {
        if (iter_block->get_address() == addr) {
            kcoalesce(iter_block);
            break;
        }
        iter_block = iter_block->m_next;
    }
}

void BuddyAllocator::kcoalesce(Block* block) {
    auto const block_size = block->get_size();
    auto const block_addr = block->get_address();
    auto const buddy_addr = ((block_addr - MIN_ADDRESS) ^ block_size) + MIN_ADDRESS;

    auto in_memory_blocks = false;
    auto in_free_blocks = false;

    // determine if buddy is in `_free_blocks' (not yet allocated and free)
    //     or if it is in `_memory_blocks' (allocated but free)
    usize i = 0;
    for (; i < _free_blocks.len(); ++i) {
        if (_free_blocks[i].m_address == buddy_addr && _free_blocks[i].m_allocatable) {
            in_free_blocks = true;
            break;
        }
    }
    auto iter_buddy = &_memory_blocks[0];
    while (iter_buddy->m_next) {
        if (iter_buddy->get_address() == buddy_addr && iter_buddy->allocatable()) {
            in_memory_blocks = true;
            break;
        }
        iter_buddy = iter_buddy->m_next;
    }
    k_console.print("in free: ", (signed)in_free_blocks, "\n");
    k_console.print("in mems: ", (signed)in_memory_blocks, "\n");

    auto const s_i = i - 1;
    if (in_free_blocks || in_memory_blocks) {
        // coalesce block partitions in `_free_blocks'
        usize j = s_i;
        for (; _free_blocks[j].m_allocatable; ++j) {
            _free_blocks[j].m_allocatable = false;
        }
        _free_blocks[j].m_allocatable = true;
        auto const new_addr = (block_addr < buddy_addr) ? block_addr : buddy_addr;
        _free_blocks[j].m_address = (new_addr < _free_blocks[j].m_address) ? new_addr : _free_blocks[j].m_address;

        // insert `block' back into `_memory_blocks'
        block->set_address(0);
        block->set_size(0);
        block->set_allocatable(true);

        block->m_prev->m_next = block->m_next;
        block->m_next->m_prev = block->m_prev;
        current_block->m_next->m_prev = block;
        block->m_next = current_block->m_next;
        current_block->m_next = block;
        block->m_prev = current_block;

        // create new coalesced block and try to coalesce it
        auto const buddy = iter_buddy;
        if (in_memory_blocks) {
            buddy->set_address(new_addr);
            buddy->set_size(block_size << 1);
            buddy->set_allocatable(true);
            kcoalesce(buddy);
        }
    }
}