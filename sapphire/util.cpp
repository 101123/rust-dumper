#include "util.hpp"
#include "hde/hde64.h"

size_t get_fn_length( void* address, size_t limit ) {
    uint64_t len = 0;

    while ( len < limit ) {
        uint8_t* inst = ( uint8_t* )address + len;

        // Break on padding or int 3
        if ( *inst == 0 || *inst == 0xCC ) {
            break;
        }

        // Break on return
        if ( *inst == 0xC3 ) {
            len++;
            break;
        }

        hde64s hs;
        uint64_t instr_len = hde64_disasm( inst, &hs );

        if ( hs.flags & F_ERROR ) {
            break;
        }

        len += instr_len;
    }

    return len;
}