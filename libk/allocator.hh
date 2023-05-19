#pragma once
#include "array.hh"
#include "int.hh"

namespace allocator {
    class Block;

    class BlockMetadata {
        public :
            BlockMetadata(const BlockMetadata& _) = delete;
            BlockMetadata(const usize size, const bool is_allocatable, Block* parent, u32 buddy) : 
                m_size(size), m_is_allocatable(is_allocatable), m_parent(parent), m_buddy(buddy) {}
            BlockMetadata() : m_size(0), m_is_allocatable(false), m_parent(nullptr), m_buddy(0) {}

            usize m_size;
            bool m_is_allocatable;
            Block* m_parent;
            u32 m_buddy;
    }__attribute((packed))__;

    class Block {
        public :
            Block(const Block& _) = delete;
            Block(BlockMetadata* data, Block* prev, Block* next) : m_data(data), m_prev(prev), m_next(next) {}
            Block() : m_data(nullptr), m_prev(nullptr), m_next(nullptr) {}

            BlockMetadata* m_data;
            Block* m_prev;
            Block* m_next;
    };

    class BuddyAllocator {
        public :
            BuddyAllocator(const BuddyAllocator& _) = delete;
            BuddyAllocator() {
                usize l = m_free_partitions.len() - 1;
                auto dummy_block = m_free_partitions[l];
                dummy_block->m_next = create_new_block(l, MAX_ADDRESS - MIN_ADDRESS, true, nullptr, 0);
            }

            [[nodiscard]] auto kalloc(const usize size) -> uptr;

            [[nodiscard]] auto kfree(const uptr addr) -> signed;

            void coalesce(Block* block);

            // get index into `_metadata' array given virtual address `addr'
            [[nodiscard]] auto va_to_index(u32 addr) -> signed {
                if (!check_address(addr)) {
                    return -1;
                }
                return (addr - MIN_ADDRESS) / PAGE_SIZE;
            }

            [[nodiscard]] auto check_address(u32 addr) -> bool {
                return addr >= MIN_ADDRESS && addr <= MAX_ADDRESS && (addr & (PAGE_SIZE - 1)) == 0;
            }

            [[nodiscard]] auto BuddyAllocator::create_new_block(const usize i, const usize size, 
                const bool is_allocatable, Block* parent, u32 buddy) -> Block*;

        private :
            static unsigned long long constexpr const MIN_ADDRESS = 0x200000;
            static unsigned long long constexpr const MAX_ADDRESS = 0xFFFFFFFF;
            static unsigned long long constexpr const PAGE_SIZE = 0x1000;
            Array<Block*, 0x20> m_free_partitions;
            Array<BlockMetadata*, (MAX_ADDRESS - MIN_ADDRESS) / 4096> m_metadata;
    };
}