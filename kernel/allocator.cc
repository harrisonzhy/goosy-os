#include "../libk/allocator.hh"
#include "../libk/int.hh"

using namespace allocator;

auto BuddyAllocator::kalloc(const usize size) -> uptr {
    usize i = _free_partitions.len() - 1;
    auto* current_block = _free_partitions[i].next;

    // track free blocks in `_free_partitions'
    while (i >= 0 && (usize)(1 << i) > size) {
        // set `next' of previous block to next of current block
        if (current_block->next) {
            current_block->prev->next = current_block->next;
            current_block->next->prev = current_block->prev;
        }
        // set `next' of previous block to `nullptr'
        else {
            current_block->prev->next = nullptr;
        }

        // append `block' to the linked list at `_free_partitions[i - 1]'
        if (i > 0) {
            auto* last_block = get_last_block(i - 1);
            if (last_block) {
                last_block->next = current_block;
                current_block->prev = last_block;
            }
        }
        --i;
    }
    ++i;
    // remove block from `_free_partitions'
    auto* alloc_block = get_last_block(i);
    alloc_block->prev->next = nullptr;
}

auto BuddyAllocator::kfree(const uptr addr) -> signed {

    return 0;
}

void BuddyAllocator::coalesce(Block* block) {
}

auto BuddyAllocator::get_last_block(const u8 i) -> Block* {
    if (i >= _free_partitions.len()) {
        return nullptr;
    }
    auto* next_block = _free_partitions[i].next;
    while (next_block->next) {
        next_block = next_block->next;
    }
    return next_block;
}