#include "../libk/allocator.hh"
#include "../libk/int.hh"

using namespace allocator;

Array<Block, 0x20> BuddyAllocator::m_free_partitions;
Array<BlockMetadata, 0x20> BuddyAllocator::m_metadata;

auto BuddyAllocator::kalloc(usize size) -> uptr {
    const usize aligned_size = size;
    for (usize i = aligned_size / PAGE_SIZE; i < m_free_partitions.len(); ++i) {
        // search for smallest possible block
        if (m_free_partitions[i].m_allocatable) {
            const auto current_block = &m_free_partitions[i];
            const auto new_block_addr = current_block->m_address;

            usize current_block_size = 1 << i;
            while (current_block_size > aligned_size) {
                const auto new_block_size = current_block_size >> 1;

                // add free buddy to `m_free_partitions[i - 1]'
                m_free_partitions[i - 1].m_address = new_block_addr + new_block_size;
                m_free_partitions[i - 1].m_buddy_address = new_block_addr;
                m_free_partitions[i - 1].m_allocatable = true;

                // update free buddy metadata in `m_metadata[m_i]'
                auto m_i = pa_to_index(m_free_partitions[i - 1].m_address);
                m_metadata[m_i].m_size = new_block_size;
                m_metadata[m_i].m_block = &m_free_partitions[i - 1];
                m_metadata[m_i].m_allocatable = true;

                current_block_size >>= 1;
            }
            return uptr(new_block_addr);
        }
    }
    return 0;
}

auto BuddyAllocator::kfree(const uptr addr) -> signed {
    return 0;
}

void BuddyAllocator::coalesce(Block* block) {
}