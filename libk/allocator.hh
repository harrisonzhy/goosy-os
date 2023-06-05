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

            inline __attribute__((always_inline)) auto get_address() const -> uptr const { return _address; }

            inline __attribute__((always_inline)) auto get_size() const -> u8 const { return _data & 0xF; }

            inline __attribute__((always_inline)) auto allocatable() const -> bool const { return _data & 0x10; }

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
                for (usize i = 0; i < _free_blocks.len(); ++i) {
                    _free_blocks[i].set_allocatable(false);
                }
                
                // create starting block of max size
                usize const l = _free_blocks.len() - 1;
                _memory_blocks[0].m_prev = &_free_blocks[l];
                _free_blocks[l].m_next = &_memory_blocks[0];
                _free_blocks[l].m_next->set_allocatable(true);

                current_block = &_memory_blocks[1];
            }

            auto kmalloc(usize const size) -> uptr;

            void kfree(uptr const addr);

            auto kmalloc_next_block() -> Block*;

            auto find_min_free(u8 const i) -> Block*;

            void append_block(Block* block);

            void print_memory_map() {
                k_console.print("FREE MEMORY\n");
                for (usize i = 0; i < _free_blocks.len(); ++i) {
                    k_console.print("i=", i, " ");
                    auto iter_block = _free_blocks[i].m_next;
                    while (iter_block) {
                        k_console.print("[", iter_block->get_size(), ",", iter_block->allocatable(), "]");
                        iter_block = iter_block->m_next;
                    }
                    k_console.print("\n");
                }

                k_console.print("USED MEMORY\n");
                auto iter_block = current_block;
                while (iter_block) {
                    k_console.print("[", iter_block->get_size(), ",", iter_block->allocatable(), "]");
                    iter_block = iter_block->m_next;
                }
            }

            [[nodiscard]] auto msb(u32 const num) -> u8 {
                return sizeof(num) * 8 - __builtin_clz(num) - 1;
            }

        private :
            Array<Block, 0x32> _memory_blocks;
            Array<Block, 0x15> _free_blocks;
            Block* current_block;

            u32 static constexpr const MIN_ADDRESS = 0x200000;
            u32 static constexpr const MAX_ADDRESS = 0xFFFFFFFF;
            u32 static constexpr const PAGE_SIZE   = 0x1000;
    };
}