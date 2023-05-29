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

    class Partition {
        public :
            Partition(Partition const& _) = delete;
            Partition() : m_address(0), m_allocatable(false) {}

            u32 m_address;
            bool m_allocatable;
    };

    class BuddyAllocator {
        public :
            BuddyAllocator(BuddyAllocator const& _) = delete;
            BuddyAllocator() : current_block(&_memory_blocks[0]) {
                for (usize i = 1; i < _memory_blocks.len(); ++i) {
                    _memory_blocks[i].m_prev = &_memory_blocks[i - 1];
                    _memory_blocks[i - 1].m_next = &_memory_blocks[i];
                }
                _free_blocks[_free_blocks.len() - 1].m_address = MIN_ADDRESS;
                _free_blocks[_free_blocks.len() - 1].m_allocatable = true;
            }

            auto kmalloc(usize const size) -> uptr;

            void kfree(uptr const addr);

            [[nodiscard]] auto check_address(uptr const addr) -> bool {
                auto const bounded = addr >= MIN_ADDRESS && addr <= MAX_ADDRESS;
                auto const aligned = (addr & (PAGE_SIZE - 1)) == 0;
                return bounded && aligned;
            }

            // dynamically allocate more `Block()' objects if needed
            [[nodiscard]] auto kmalloc_next_block() -> Block*;

            // coalesce contiguous free blocks at and below `_free_blocks[`s_i']'
            void kcoalesce(Block* block);

            [[nodiscard]] inline __attribute__((always_inline)) auto log_two_ceil(u32 const num) -> u8 {
                return sizeof(num) * 8 - __builtin_clz(num) - 1;
            }

            void print_memory_map() {
                k_console.print("FREE MEMORY\n");
                for (usize i = 0; i < _free_blocks.len(); ++i) {
                    u8 const allocatable = _free_blocks[i].m_allocatable;
                    k_console.print("[", _free_blocks[i].m_address, ",", allocatable, "]");
                }
                k_console.print("\n\n");

                k_console.print("ALLOCATED MEMORY\n");
                auto i = 0;
                auto iter_block = &_memory_blocks[0];
                while (iter_block && i < 0x14) {
                    u8 const allocatable = iter_block->allocatable();
                    // if (!allocatable) {
                        k_console.print("[", iter_block->get_size(), ",", allocatable, "]");
                    // }
                    iter_block = iter_block->m_next;
                    ++i;
                }
                k_console.print("\n\n");
            }

        private :
            Array<Block, 0x32> _memory_blocks;
            Array<Partition, 0x14> _free_blocks;
            Block* current_block;

            u32 static constexpr const MIN_ADDRESS = 0x200000;
            u32 static constexpr const MAX_ADDRESS = 0xFFFFFFFF;
            u32 static constexpr const PAGE_SIZE   = 0x1000;
    };
}