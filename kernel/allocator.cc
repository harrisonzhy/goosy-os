#include "../libk/allocator.hh"
#include "../libk/int.hh"

using namespace allocator;

auto BuddyAllocator::kalloc(const usize size) -> uptr {
    const usize aligned_size = size;
    signed i = m_free_partitions.len() - 1;
    auto current_block = m_free_partitions[i]->m_next;

    while (i > 0 && (usize)(1 << (i - 1)) > aligned_size && current_block) {
        if (current_block && !m_free_partitions[i - 1]->m_next) {
            // create free block in `m_free_partitions[i - 1]' only if there are no free blocks there
            auto current_block_size = current_block->m_data->m_size;            
            m_free_partitions[i - 1]->m_next = create_new_block(i - 1, current_block_size, true, nullptr, 0);
        }

        --i;
        current_block = m_free_partitions[i]->m_next;
    }

    // fill corresponding entry in `m_metadata' to indicate allocation
    usize m_i = va_to_index()
}

auto BuddyAllocator::kfree(const uptr addr) -> signed {

    return 0;
}

void BuddyAllocator::coalesce(Block* block) {
}

auto BuddyAllocator::create_new_block(const usize i, const usize size,
    const bool is_allocatable, Block* parent, u32 buddy) -> Block* {
    auto block_data = BlockMetadata(size, is_allocatable, parent, buddy);
    auto block = Block(&block_data, m_free_partitions[i], nullptr);
    return &block;
}