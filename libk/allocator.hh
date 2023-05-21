#pragma once
#include "array.hh"
#include "int.hh"
#include "tty.hh"

using namespace console;
namespace allocator {
    class Block;

    class BlockMetadata {
        public :
            BlockMetadata(const BlockMetadata& _) = delete;
            BlockMetadata(const usize size, Block* block, const bool allocatable, const u32 buddy_address,
                BlockMetadata* parent) : m_size(size), m_block(block), m_allocatable(allocatable), m_next(nullptr) {}
            BlockMetadata(BlockMetadata* parent) : m_size(0), m_block(nullptr), m_allocatable(false), m_next(nullptr) {}
            BlockMetadata() : m_size(0), m_block(nullptr), m_allocatable(false), m_next(nullptr) {}

            usize m_size;
            Block* m_block;
            bool m_allocatable;
            BlockMetadata* m_next;
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
                auto constexpr const l = 0x20 - 0x1;
                m_free_partitions[l].m_address = MIN_ADDRESS;
                m_free_partitions[l].m_buddy_address = 0;
                m_free_partitions[l].m_allocatable = true;

                m_metadata[0].m_size = MAX_ADDRESS - MIN_ADDRESS;
                m_metadata[0].m_block = &m_free_partitions[l];
                m_metadata[0].m_allocatable = true;

                for (auto i = 1; i < m_metadata.len(); ++i) {
                    m_metadata[i - 1].m_next = &m_metadata[i];
                }
            }

            [[nodiscard]] auto kalloc(usize size) -> uptr;

            [[nodiscard]] auto kfree(const uptr addr) -> signed;

            void coalesce(Block* block);

            // get index into `m_metadata' array given virtual address `addr'
            [[nodiscard]] auto va_to_index(u32 addr) -> signed {
                const auto valid = check_address(addr);
                if (!valid) {
                    return -1;
                }
                return (addr - MIN_ADDRESS) / PAGE_SIZE - 1;
            }

            [[nodiscard]] auto check_address(u32 addr) -> bool {
                return addr >= MIN_ADDRESS && addr <= MAX_ADDRESS && (addr & (PAGE_SIZE - 1)) == 0;
            }

        private :
            static u32 const MIN_ADDRESS = 0x200000;
            static u32 constexpr const MAX_ADDRESS = 0xFFFFFFFF;
            static u32 constexpr const PAGE_SIZE = 0x1000;
            Array<Block, 0x20> m_free_partitions;
            Array<BlockMetadata, 0x20> m_metadata;
    };
}