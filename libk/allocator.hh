#pragma once
#include "array.hh"
#include "int.hh"

namespace allocator {
    class Block {
        public :
            Block(const Block& _) = delete;
            Block() : prev(nullptr), next(nullptr) {}

            BlockMetadata* meta;
            Block* prev;
            Block* next;
    }__attribute((packed))__;

    class BlockMetadata {
        public :
            BlockMetadata(const BlockMetadata& _) = delete;
            BlockMetadata() : size(0), is_allocatable(false), parent_block(nullptr), buddy(nullptr) {}

            usize size;
            bool is_allocatable;
            Block* parent_block;
            Block* buddy;
    }__attribute((packed))__;

    class BuddyAllocator {
        public :
            BuddyAllocator(const BuddyAllocator& _) = delete;
            BuddyAllocator() {

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
                return addr >= MIN_ADDRESS && addr <= MAX_ADDRESS && (addr & (PAGE_SIZE - 1) == 0);
            }

            // get the last block in the linked list at `_free_partitions[i]'
            [[nodiscard]] auto get_last_block(const u8 i) -> Block*;

        private :
            static unsigned long long constexpr const MIN_ADDRESS = 0x200000;
            static unsigned long long constexpr const MAX_ADDRESS = 0xFFFFFFFF;
            static unsigned long long constexpr const PAGE_SIZE = 0x1000;
            Array<Block, 0x20> _free_partitions;
            Array<BlockMetadata, (MAX_ADDRESS - MIN_ADDRESS) / 4096> _metadata;
    };
}