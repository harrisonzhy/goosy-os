#include "../libk/allocator.hh"
#include "../libk/int.hh"

using namespace console;
using namespace allocator;

auto BuddyAllocator::kmalloc(const usize size) -> uptr {
    const auto aligned_size = size;
    for (usize i = aligned_size / PAGE_SIZE - 1; i < m_free_blocks.len(); ++i) {
        if (m_free_blocks[i].m_allocatable) {
            const auto s_i = i;
            const auto alloc_addr = m_free_blocks[s_i].m_address;
            
            usize curr_block_size = 1 << s_i;
            for (; curr_block_size > aligned_size; curr_block_size >>= 1, --i) {
                m_free_blocks[i].m_address = alloc_addr + curr_block_size;
                m_free_blocks[i].m_allocatable = true;

            }
            m_free_blocks[s_i].m_allocatable = false;
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

auto BuddyAllocator::simple_kmalloc(const usize size) -> uptr {
    const auto new_partition_size = size / PAGE_SIZE;
    const auto new_partition_objects = reinterpret_cast<Block*>(kmalloc(new_partition_size * sizeof(Block)));
    new_partition_objects[1].m_data = 0b11110000;
    Console::print(new_partition_objects[1].m_data);
    return (uptr)new_partition_objects;
}

