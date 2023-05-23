#pragma once
#include "array.hh"
#include "int.hh"
#include "tty.hh"

using namespace console;
namespace allocator {

    class Block {
        public :
            Block(const Block& _) = delete;
            Block() : m_data(0), m_next(nullptr), m_prev(nullptr) {}

            inline __attribute__((always_inline)) auto get_size() const -> const u8 { return m_data & 0xF; }

            inline __attribute__((always_inline)) auto is_allocatable() const -> const bool { return m_data & 0x10; }

            void set_size(const u8 size) { m_data &= 0xF0; m_data |= size; }

            void set_allocatable(bool allocatable) {
                m_data = (m_data & (~(1 << 4))) | (allocatable << 4);

                // if (allocatable) {
                //     m_data = (1 << 5) | m_data;
                // }
                // else {
                //     m_data &= 0xEF;
                // }
            }

            u8 m_data;
            Block* m_next;
            Block* m_prev;
    };

    class Partition {
        public :
            Partition(const Partition& _) = delete;
            Partition() : m_address(0), m_allocatable(false) {}

            u32 m_address;
            bool m_allocatable;
    };

    class BuddyAllocator {
        public :
            BuddyAllocator(const BuddyAllocator& _) = delete;
            BuddyAllocator() : current_block(&_memory_blocks[0]) {
                for (usize i = 1; i < _memory_blocks.len(); ++i) {
                    _memory_blocks[i].m_prev = &_memory_blocks[i - 1];
                    _memory_blocks[i - 1].m_next = &_memory_blocks[i];
                }
                _free_blocks[0x1F].m_allocatable = true;
            }

            [[nodiscard]] auto kmalloc(const usize size) -> uptr;

            [[nodiscard]] auto kfree(const uptr addr) -> signed;

            void coalesce(Block* block);

            // dynamically allocate more `Block()' objects
            [[nodiscard]] auto simple_kmalloc(const usize size) -> uptr;

            // get index into `m_metadata' array given virtual address `addr'
            [[nodiscard]] auto va_to_index(const u32 addr) -> signed {
                const auto valid = check_address(addr);
                if (!valid) {
                    return -1;
                }
                return (addr - MIN_ADDRESS) / PAGE_SIZE - 1;
            }

            [[nodiscard]] auto check_address(const u32 addr) -> bool {
                const auto bounded = addr >= MIN_ADDRESS && addr <= MAX_ADDRESS;
                const auto aligned = (addr & (PAGE_SIZE - 1)) == 0;
                return bounded && aligned;
            }

            [[nodiscard]] auto get_next_block() -> Block*;

            void print_memory_map() {
                for (auto i = 0; i < 0x20; ++i) {
                    const u32 allocatable = _free_blocks[i].m_allocatable;
                    Console::print("[", _free_blocks[i].m_address, " ", allocatable, "] ");
                }
            }

        private :
            Array<Block, 0x20> _memory_blocks;
            Array<Partition, 0x20> _free_blocks;
            Block* current_block;

            static u32 constexpr const MIN_ADDRESS = 0x200000;
            static u32 constexpr const MAX_ADDRESS = 0xFFFFFFFF;
            static u32 constexpr const PAGE_SIZE = 0x1000;
    };
}