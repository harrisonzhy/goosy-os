#pragma once
#include "array.hh"
#include "int.hh"
#include "tty.hh"
#include "option.hh"

using namespace console;

extern Console k_console;

namespace allocator {
    class Block {
        public :
            constexpr Block(Block const& _) = delete;
            constexpr Block() : m_next(nullptr), m_prev(nullptr), _address(0), _data(0b1) {}

            auto constexpr address() const -> uptr const { return _address; }

            auto constexpr allocatable() const -> bool const { return _data & 0b1; }

            // set address of this block
            void set_address(uptr const addr) { _address = addr; }

            // set whether this block is allocatable (bit 0)
            void set_allocatable(bool allocatable) {
                _data ^= -allocatable ^ _data;
            }

            Block* m_next;
            Block* m_prev;

        private :
            u32 _address;
            u8 _data;
    };

    class BuddyAllocator {
        public :
            BuddyAllocator(BuddyAllocator const& _) = delete;
            BuddyAllocator() {
                for (u8 i = 1; i < NUM_ENTRIES_MEM; ++i) {
                    _memory_blocks[i].m_prev = &_memory_blocks[i - 1];
                    _memory_blocks[i - 1].m_next = &_memory_blocks[i];
                }
                for (u8 i = 0; i < NUM_ENTRIES_ALLOC; ++i) {
                    _allocated_blocks[i].set_allocatable(false);
                }
                _allocated_blocks.last().m_next = &_memory_blocks[0];

                // create starting block of maximum size
                _memory_blocks.begin()->m_prev = &_allocated_blocks.last();
                _memory_blocks.begin()->m_next = nullptr;
                current_block = &_memory_blocks[1];
            }

            auto kmalloc(usize const size) -> uptr;

            void kfree(uptr const addr);

            auto kmalloc_next_block() -> Block*;

            auto coalesce(uptr const addr, u8 const idx) -> u8;

            auto find_min_free(u8 const idx) -> Block*;

            void append_block(Block* block, Block* list);

            void print_memory_map();

            [[nodiscard]] auto constexpr msb(u32 const num) -> u8 {
                return sizeof(num) * 8 - __builtin_clz(num) - 1;
            }

        private :
            usize static constexpr const NUM_ENTRIES_MEM = 0x32;
            usize static constexpr const NUM_ENTRIES_ALLOC = 0x15;
            usize static constexpr const MAX_ALLOC_SIZE = 0x8000000;

            u32 static constexpr const MIN_ADDRESS = 0x200000;
            u32 static constexpr const MAX_ADDRESS = 0xFFFFFFFF;
            u32 static constexpr const PAGE_SIZE   = 0x1000;

            Array<Block, NUM_ENTRIES_MEM> _memory_blocks;
            Array<Block, NUM_ENTRIES_ALLOC> _allocated_blocks;
            Block* current_block;
    };
}