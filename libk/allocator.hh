#pragma once
#include "array.hh"
#include "int.hh"

namespace allocator {
    class Block;

    class BlockMetadata {
        public :
            BlockMetadata(const BlockMetadata& _) = delete;
            BlockMetadata(const usize size, Block* block, const bool allocatable, const u32 buddy_address,
                BlockMetadata* parent) : m_size(size), m_block(block), m_allocatable(allocatable) {}
            BlockMetadata(BlockMetadata* parent) : m_size(0), m_block(nullptr), m_allocatable(false) {}
            BlockMetadata() : m_size(0), m_block(nullptr), m_allocatable(false) {}

            usize m_size;
            Block* m_block;
            bool m_allocatable;
    };

    class Block {
        public :
            Block(const Block&  _) = delete;
            Block(const u32 address, const u32 buddy_address, bool allocatable) : m_address(address),
                m_buddy_address(buddy_address), m_allocatable(allocatable) {}
            Block() : m_address(0), m_buddy_address(0), m_allocatable(false) {}

            u32 m_address;
            u32 m_buddy_address;
            bool m_allocatable;
    };

    class BuddyAllocator {
        public :
            BuddyAllocator(const BuddyAllocator& _) = delete;
            BuddyAllocator() {
                auto constexpr const l = m_free_partitions.len() - 1;
                m_free_partitions[l].m_address = MIN_ADDRESS;
                m_free_partitions[l].m_buddy_address = 0;
                m_free_partitions[l].m_allocatable = false;

                m_metadata[0].m_size = MAX_ADDRESS - MIN_ADDRESS;
                m_metadata[0].m_block = &m_free_partitions[l];
                m_metadata[0].m_allocatable = true;
            }

            [[nodiscard]] auto kalloc(usize size) -> uptr;

            [[nodiscard]] auto kfree(const uptr addr) -> signed;

            void coalesce(Block* block);

            // get index into `_metadata' array given virtual address `addr'
            [[nodiscard]] auto pa_to_index(u32 addr) -> signed {
                if (!check_address(addr)) {
                    return -1;
                }
                return (addr - MIN_ADDRESS) / PAGE_SIZE;
            }

            [[nodiscard]] auto check_address(u32 addr) -> bool {
                return addr >= MIN_ADDRESS && addr <= MAX_ADDRESS && (addr & (PAGE_SIZE - 1)) == 0;
            }

        private :
            static u32 constexpr const MIN_ADDRESS = 0x200000;
            static u32 constexpr const MAX_ADDRESS = 0xFFFFFFFF;
            static u32 constexpr const PAGE_SIZE = 0x1000;
            static Array<Block, 0x20> m_free_partitions;
            static Array<BlockMetadata, 0x20> m_metadata;
    };
}