#include "util.hpp"

namespace util {
    bool function_attributes_t::transfers_control_to( void* address ) {
        for ( uint64_t addr : calls ) {
            if ( addr == ( uint64_t )address ) {
                return true;
            }
        }

        for ( uint64_t addr : jmps ) {
            if ( addr == ( uint64_t )address ) {
                return true;
            }
        }

        return false;
    }

    function_attributes_t get_function_attributes( void* address, size_t limit ) {
        function_attributes_t attr;
        uint64_t len = 0;

        while ( len < limit ) {
            uint8_t* inst = ( uint8_t* )address + len;

            // Break on padding or int 3
            if ( *inst == 0 || *inst == 0xCC ) {
                break;
            }

            /*
            // Break on return
            if ( *inst == 0xC3 ) {
                len++;
                break;
            }
            */

            hde64s hs;
            uint64_t instr_len = hde64_disasm( inst, &hs );

            if ( hs.flags & F_ERROR ) {
                break;
            }

            else if ( hs.opcode == 0xE8 ) {
                uint64_t target = ( uint64_t )inst + instr_len + (int32_t)hs.imm.imm32;
                attr.calls.push_back( target );
            }

            else if ( hs.opcode == 0xE9 ) {
                uint64_t target = ( uint64_t )inst + instr_len + (int32_t)hs.imm.imm32;
                attr.jmps.push_back( target );
            }   

            len += instr_len;
        }

        attr.length = len;

        return attr;
    }
}
