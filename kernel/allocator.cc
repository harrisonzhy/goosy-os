#include "../libk/allocator.hh"
#include "../libk/int.hh"

using namespace console;
using namespace allocator;

auto BuddyAllocator::kmalloc(const usize size) -> uptr {
    const auto aligned_size = PAGE_SIZE;
    const auto aligned_size_log = 0;

    for (usize i = aligned_size / PAGE_SIZE - 1; i < _free_blocks.len(); ++i) {
        if (_free_blocks[i].m_allocatable) {
            const auto s_i = i;
            const auto alloc_addr = _free_blocks[s_i].m_address;

            usize curr_block_size = 1 << s_i;
            for (; curr_block_size > aligned_size; --i) {
                _free_blocks[i].m_address = alloc_addr + curr_block_size;
                _free_blocks[i].m_allocatable = true;

                // allocate new block and augment `current_block'
                auto new_block = current_block;
                current_block = get_next_block();
                new_block->set_size(aligned_size_log);
                new_block->set_allocatable(false);

                curr_block_size /= 2;
            }

            _free_blocks[s_i].m_allocatable = false;
            return uptr(alloc_addr);
        }
    }
    return uptr(0);
}

auto BuddyAllocator::kfree(const uptr addr) -> signed {
    return 0;
}

void BuddyAllocator::coalesce(Block* block) {
}

auto BuddyAllocator::get_next_block() -> Block* {
    if (!current_block->m_next) {
        // allocate `PAGE_SIZE' / sizeof(`Block') more blocks
        const auto num_blocks = PAGE_SIZE / sizeof(Block);
        const auto k = kmalloc(num_blocks);
        if (!k) [[unlikely]] {
            return nullptr;
        }

        // set all new_blocks created as allocatable
        const auto new_blocks = reinterpret_cast<Block*>(k);
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
    }
    return current_block->m_next;
}

auto BuddyAllocator::simple_kmalloc(const usize size) -> uptr {
    const auto new_partition_size = size / PAGE_SIZE;
    const auto new_partition_objects = reinterpret_cast<Block*>(kmalloc(new_partition_size * sizeof(Block)));
    new_partition_objects[1].m_data = 0b11100000;
    return (uptr)new_partition_objects;
}

