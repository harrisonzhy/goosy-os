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
                if (allocatable) {
                    m_data = (1 << 5) | m_data;
                }
                else {
                    m_data &= 0xEF;
                }
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
            BuddyAllocator() : m_allocated(PAGE_SIZE) {
                m_free_blocks[0x1F].m_allocatable = true;
            }

            [[nodiscard]] auto kmalloc(const usize size) -> uptr;

            [[nodiscard]] auto kfree(const uptr addr) -> signed;

            void coalesce(Block* block);

            // dynamically allocate more `Block()' objects
            [[nodiscard]] auto simple_kmalloc(const usize size) -> uptr;
            [[nodiscard]] auto simple_kmalloc2(const usize size) -> uptr;

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

        private :
            Array<Block, 0x20> m_memory_blocks;
            Array<Partition, 0x20> m_free_blocks;

            u32 m_allocated;
            static u32 constexpr const MIN_ADDRESS = 0x200000;
            static u32 constexpr const MAX_ADDRESS = 0xFFFFFFFF;
            static u32 constexpr const PAGE_SIZE = 0x1000;
    };
}