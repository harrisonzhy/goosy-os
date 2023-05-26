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
            Block() : m_data(1 << 4), m_next(nullptr), m_prev(nullptr) {}

            inline __attribute__((always_inline)) auto get_size() const -> const u8 { return m_data & 0xF; }

            inline __attribute__((always_inline)) auto is_allocatable() const -> const bool { return m_data & 0x10; }

            // set size of corresponding allocation (bits 3-0)
            void set_size(const u8 size) { m_data &= 0xF0; m_data |= size; }

            // set whether this block is allocatable (bit 4)
            void set_allocatable(bool allocatable) {
                auto const upper = m_data & ~(1 << 4);
                m_data = upper | (allocatable << 4);
            }

            u8 m_data;
            Block* m_next;
            Block* m_prev;
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

            auto kfree(uptr const addr) -> signed;

            void coalesce(Block* block);

            // get index into `m_metadata' array given virtual address `addr'
            [[nodiscard]] auto va_to_index(const u32 addr) -> signed {
                auto const valid = check_address(addr);
                if (!valid) {
                    return -1;
                }
                return (addr - MIN_ADDRESS) / PAGE_SIZE - 1;
            }

            [[nodiscard]] auto check_address(const u32 addr) -> bool {
                auto const bounded = addr >= MIN_ADDRESS && addr <= MAX_ADDRESS;
                auto const aligned = (addr & (PAGE_SIZE - 1)) == 0;
                return bounded && aligned;
            }

            // dynamically allocate more `Block()' objects if needed
            [[nodiscard]] auto kmalloc_next_block() -> Block*;

            [[nodiscard]] inline __attribute__((always_inline)) auto log_two_ceil(const u32 num) -> u8 {
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
                while (iter_block && i < 0x10) {
                    u8 const allocatable = iter_block->is_allocatable();
                    if (!allocatable) {
                        k_console.print("[", iter_block->get_size(), ",", allocatable, "]");
                    }
                    iter_block = iter_block->m_next;
                    ++i;
                }
                k_console.print("\n\n");
            }

        private :
            Array<Block, 0x32> _memory_blocks;
            Array<Partition, 0x14> _free_blocks;
            Block* current_block;

            static u32 constexpr const MIN_ADDRESS = 0x200000;
            static u32 constexpr const MAX_ADDRESS = 0xFFFFFFFF;
            static u32 constexpr const PAGE_SIZE = 0x1000;
    };
}