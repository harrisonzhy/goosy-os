#pragma once
#include "array.hh"
#include "int.hh"
#include "tty.hh"

using namespace console;

extern Console k_console;

namespace allocator {
    class Block {
        public :
            Block(Block const& _) = delete;
            Block() : m_next(nullptr), m_prev(nullptr), _address(0), _data(1 << 4) {}

            auto constexpr get_address() const -> uptr const { return _address; }

            auto constexpr get_size() const -> u8 const { return _data & 0xF; }

            auto constexpr allocatable() const -> bool const { return _data & 0x10; }

            // set address of this block
            void set_address(uptr const addr) { _address = addr; }

            // set size of corresponding allocation (bits 3-0)
            void set_size(u8 const size) { _data &= 0xF0; _data |= size; }

            // set whether this block is allocatable (bit 4)
            void set_allocatable(bool allocatable) {
                auto const upper = _data & ~(1 << 4);
                _data = upper | (allocatable << 4);
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
                for (usize i = 1; i < _memory_blocks.len(); ++i) {
                    _memory_blocks[i].m_prev = &_memory_blocks[i - 1];
                    _memory_blocks[i - 1].m_next = &_memory_blocks[i];
                }
                for (usize i = 0; i < _allocated_blocks.len(); ++i) {
                    _allocated_blocks[i].set_allocatable(false);
                }

                // create starting block of maximum size
                usize const l = _allocated_blocks.len() - 1;
                _memory_blocks[0].m_prev = &_allocated_blocks[l];
                _memory_blocks[0].m_next = nullptr;
                _allocated_blocks[l].m_next = &_memory_blocks[0];
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
            Array<Block, 0x32> _memory_blocks;
            Array<Block, 0x15> _allocated_blocks;
            Block* current_block;

            u32 static constexpr const MIN_ADDRESS = 0x200000;
            u32 static constexpr const MAX_ADDRESS = 0xFFFFFFFF;
            u32 static constexpr const PAGE_SIZE   = 0x1000;
    };
}