#include <iostream>
#include <cstring>
#include <sstream>
#include <cstdlib>
#define dbgStream std::cout
/**
Kind of cpu.cpp/cpu.py, but by Adam Balawender (github.com/phaezah7)
*/

class CPU {

    // [TODO] : timer, interrupts, debug, memory protection
    enum registerNames { PC, SP, DP, CR, A, B, C, D, L, P };

    unsigned int clock;

#ifndef dbgStream
    std::stringstream dbgStream;
#endif

    enum instructions { 
        NOP = 0x00,     // 
        INT = 0x01,     // store PC, don't increment SP? , PC = to 0+CR[7:8] ? 
        SWP = 0x02,     // swap L <-> A
        JRE = 0x04,     // x y :: branch always / if SH set :: relative to P/PC
        JMP = 0x08,     // unimp
        BRC = 0x0C,     // unimp
        ALU = 0x10,     // 
        LFA = 0x20,     // xx yy :: src  C/D/SP/DP :: dest A/B/C/L
        STA = 0x30,     // xx yy :: dest C/D/SP/DP :: src  A/B/C/L
        TST = 0x40,     // x yy zzz :: use AB/LC :: chain with SH - NONE/AND/OR/XOR :: MASK
        MOV = 0x80,     // xxx yyy  :: src A/C/D/L/P/SP/DP/PC :: dest B/C/D/L/P/SP/DP/CR
        SET = 0xC0,     // xx y zzz :: reset NONE/UP/DOWN/ALL :: set UP/DOWN :: A/B/C/D/L/P/DP/CR

        _NOP = MOV | 0x1<<3 | 0x1
    };

    enum controlRegisterBits {
        control_SYS = 0x01,     // if set, CPU is in privileged mode (by default)
        control_DBG = 0x02,     // if set, CPU in dbg mode (unimpl)
        control_BRP = 0x04,     // enable breakpoints      (unimpl)
        control_SHB = 0x08,     // branching flag
        control_INT = 0x30,     // interrupt source
        control_TIM = 0x10,     //      timer
        control_ILL = 0x20,     //      illegal instr
        control_ILM = 0x30,     //      illegal memory access (unimpl)

        control_RESERVED = 0xC0,

        control_TIC = 0xFF00,   // timer interruption control
        control_TBC = 0x0F00,   // timer base
        control_TSH = 0xF000    // timer shift
    };

    short int registers[10];
    unsigned char memory[2<<16] = { 0 }; // 64KB of memory

    public:

    CPU( void ) {

        registers[CR] = 1;
        registers[PC] = 0x40;
        registers[SP] = 1028;

        char heap[] = "Hello, world!";

        unsigned char prog1[] = {
            // INIT
            SET | 0x30 | 0x3,   // D = 1024
                1024>>8,        //
            MOV | 0x15,         // SP = D
            SET | 0x30 | 0x6,   // DP = 2048
                2048>>8,        //
            //SET | 0x7, 0,       // ZERO CR, permission test, should fail here.

            // LOOP
            LFA | 0x8 | 0x0,    //  A = *SP
            STA | 0xC | 0x0,    //  *DP = A
            TST | 0x7,          //  if(A) -> set(SH)
            JRE | 0x3,          //  if(SH) -> jump -5  // loop until non-zero data is read
            (unsigned char)-5
        };

        unsigned char prog2[] = {
            // interrupt handlers
                NOP,           //0x00 - user interrupt handler
                _NOP,
                _NOP,
                _NOP,
                _NOP,
                _NOP,
                _NOP,
                _NOP,
                _NOP,
                _NOP,
                _NOP,
                _NOP,
                _NOP,
                _NOP,
                _NOP,
                _NOP,
                SWP,            //0x10  -   timer interrupt handler
                ALU,
                SWP,
                JRE,
                0,
                _NOP,
                _NOP,
                _NOP,
                _NOP,
                _NOP,
                _NOP,
                _NOP,
                _NOP,
                _NOP,
                _NOP,
                _NOP,
                _NOP,
                _NOP,
                _NOP,
                _NOP,
                _NOP,
                _NOP,
                _NOP,
                _NOP,
                _NOP,
                _NOP,
                _NOP,
                _NOP,
                _NOP,
                _NOP,
                _NOP,
                _NOP,
                _NOP,
                _NOP,
                _NOP,
                _NOP,
                _NOP,
                _NOP,
                _NOP,
                _NOP,
                _NOP,
                _NOP,
                _NOP,
                _NOP,
                _NOP,
                _NOP,
                _NOP,
                _NOP,
            // INIT
            SET | 0x30 | 0x7,   // CR = 0
                0xF1,
            JRE | 0x1,          // wait for interrupt
                (unsigned char)-2,
        };

        std::memcpy( memory+1024, &heap, sizeof(heap) );
        std::memcpy( memory, &prog2, sizeof(prog2) );
    }

    short int fetch_instruction( void ) {

        ++clock;

        if( registers[CR] & control_TIC and clock == (unsigned int)( (registers[CR]>>8 & 0xF ) << (registers[CR]>>12 & 0xF  ) << 4 ) ) {
            clock = 0;
            registers[CR] |= control_TIM;
        }

        if( registers[CR] & control_INT ) { std::cout << "interrupt fired!" << std::endl; return 1; }


        short int instr = memory[registers[PC]];
        if( instr == 0 ) std::cout << "fetched 0 from " << registers[PC] << "\n";
        ++registers[PC];


        return instr;

    }

    short int execute_instruction( short int instr ) {

        static int c = 0;


        if ( INT != instr  ) c = 0;


        if( NOP == instr )  // unreachable, instr fetch does not pass 0 to here
        {
            dbgStream << "NOP" << std::endl;
        }
        else if( INT == instr )
        {
            dbgStream << "INT" << std::endl;
            //std::cout << "reg: " << ( registers[CR] & 0x6 ) << " memory at " << (int)memory[registers[CR] & 0x6 ] << std::endl;
            registers[PC] = registers[CR] & control_INT;
            //registers[PC] = *(short int*)&memory[registers[CR] & control_INT ];
            registers[CR] &= ~control_INT;

            std::cout << "jump to : " << registers[PC] << std::dec << " ( " << registers[PC] << " ) " << std::hex << std::endl;
        }
        else if( SWP == (instr & 0xFE) )
        {
            dbgStream << "SWP" << std::endl;

            std::swap( registers[L], registers[A] );
        }
        else if( JRE == (instr & 0xFC) )
        {
            short int tmp = (char)memory[registers[PC]++] + ( instr & 1 ? registers[PC] : registers[P] ); // assumption: (incrementation + read) execution from right to left

            dbgStream << "JRE 0x" << std::hex << (instr & 0x03) << " " << tmp << std::endl;
            if( instr & 0x02 ) {
                if( registers[CR] & 0x08 ) {
                    registers[PC] = tmp;
                }
            }
            else registers[PC] = tmp;
        }
        else if( ALU == (instr & 0xF0) )
        {
            dbgStream << "ALU 0x" << std::hex << (instr & 0x0F) << std::endl;
            switch( instr & 0x0F ) {
                case 0x0: ++registers[A]; break;
                case 0x1: --registers[A]; break;
                case 0x2: registers[A] *= -1; break;
                case 0x3: registers[A] = registers[C]; break;
                case 0x4: registers[A] = 0; break;
                case 0x5: registers[A] += registers[C]; break;
                case 0x6: registers[A] -= registers[C]; break;
                case 0x7: registers[A] <<= registers[C]; break;
                case 0x8: registers[A] >>= registers[C]; break;
                case 0x9: registers[A] <<= 1; break;
                case 0xA: registers[A] >>= 1; break;
                case 0xB: registers[A] = 1<<registers[C]; break;
                case 0xC: registers[A] &= registers[C]; break;
                case 0xD: registers[A] |= registers[C]; break;
                case 0xE: registers[A] ^= registers[C]; break;
                case 0xF: registers[A] = ~registers[A];
            }
        }
        else if( LFA == (instr & 0xF0) )
        {
            const char *src[] = { "C", "D", "SP", "DP" };
            const char *dst[] = { "A", "B", "C", "L" };
            dbgStream << "LFA " << src[(instr & 0xC)>>2] << ", " << dst[instr & 0x3] << std::endl;
            //dbgStream << "LFA 0x" << std::hex << (instr & 0x0F) << std::endl;
            short int tmp;
            switch( instr & 0x0C ) {
                case 0x0: tmp = *(short int*)&memory[registers[C] ]; break;
                case 0x4: tmp = *(short int*)&memory[registers[D] ]; break;
                case 0x8: tmp = *(short int*)&memory[registers[SP]]; registers[SP]+=2; break;
                case 0xC: tmp = *(short int*)&memory[registers[DP]]; registers[DP]-=2; break;
            }
            switch( instr & 0x03 ) {
                case 0x0: registers[A] = tmp; break;
                case 0x1: registers[B] = tmp; break;
                case 0x2: registers[C] = tmp; break;
                case 0x3: registers[L] = tmp; break;
            }
        }
        else if( STA == (instr & 0xF0) )
        {
            const char *dst[] = { "C", "D", "SP", "DP" };
            const char *src[] = { "A", "B", "C", "L" };
            dbgStream << "STA " << src[instr & 0x3] << ", " << dst[(instr & 0xC)>>2] << std::endl;
            //dbgStream << "STA 0x" << std::hex << (instr & 0x0F) << std::endl;
            short int tmp;
            switch( instr & 0x03 ) {
                case 0x0: tmp = registers[A]; break;
                case 0x1: tmp = registers[B]; break;
                case 0x2: tmp = registers[C]; break;
                case 0x3: tmp = registers[L]; break;
            }
            switch( instr & 0x0F ) {
                case 0x0: *(short int*)&memory[registers[C]]  = tmp; break;
                case 0x4: *(short int*)&memory[registers[D]]  = tmp; break;
                case 0x8: *(short int*)&memory[registers[SP]] = tmp; registers[SP]-=2; break;
                case 0xC: *(short int*)&memory[registers[DP]] = tmp; registers[DP]+=2; break;
            }
        }
        else if( TST == (instr & 0xC0) )
        {
            dbgStream << "TST 0x" << std::hex << (instr & 0x3F) << std::endl;
            bool tmp;
            short int *reg1 = ( instr & 0x20 ) ? &registers[L] : &registers[A];
            short int *reg2 = ( instr & 0x20 ) ? &registers[C] : &registers[B];
            switch ( instr & 0x07 ) {
                case 0x0: tmp = *reg1 == 0;     break;
                case 0x1: tmp = *reg1 <  *reg2; if( tmp and instr & 0x20 ) ++registers[L]; break;
                case 0x2: tmp = *reg1 == *reg2; break;
                case 0x3: tmp = *reg1 <= *reg2; break;
                case 0x4: tmp = *reg1 >  *reg2; if( tmp and instr & 0x20 ) --registers[L]; break;
                case 0x5: tmp = *reg1 >= *reg2; break;
                case 0x6: tmp = *reg1 != *reg2; break;
                case 0x7: tmp = *reg1 != 0;     break;
            }
            switch ( instr & 0x18 ) {
                case 0x0:  registers[CR] = ( registers[CR] & ~0x08 ) | (0x08*tmp); break;
                case 0x8:  registers[CR] =   registers[CR]           & (0x08*tmp); break;
                case 0x10: registers[CR] =   registers[CR]           | (0x08*tmp); break;
                case 0x18: registers[CR] =   registers[CR]           ^ (0x08*tmp); break;
            }
        }
        else if( MOV == (instr & 0xC0) )
        {
            const char *src[] = { "A", "C", "D", "L", "P", "SP", "DP", "PC" };
            const char *dst[] = { "B", "C", "D", "L", "P", "SP", "DP", "CR" };
            dbgStream << "MOV " << src[(instr & 0x38)>>3] << ", " << dst[instr & 0x7] << std::endl;
            //dbgStream << "MOV 0x" << std::hex << (instr & 0x3F) << std::endl;
                    //MOV = 0x80,     // xxx yyy  :: src A/C/D/L/P/SP/DP/PC :: dest B/C/D/L/P/SP/DP/CR
            
            short int *srcP, *dstP;
            switch( instr>>3 & 0x07 ) {
                case 0x0: srcP = &registers[A ]; break;
                case 0x1: srcP = &registers[C ]; break;
                case 0x2: srcP = &registers[D ]; break;
                case 0x3: srcP = &registers[L ]; break;
                case 0x4: srcP = &registers[P ]; break;
                case 0x5: srcP = &registers[SP]; break;
                case 0x6: srcP = &registers[DP]; break;
                case 0x7: srcP = &registers[PC]; break;
            }
            switch( instr & 0x07 ) {
                case 0x0: dstP = &registers[B ]; break;
                case 0x1: dstP = &registers[C ]; break;
                case 0x2: dstP = &registers[D ]; break;
                case 0x3: dstP = &registers[L ]; break;
                case 0x4: dstP = &registers[P ]; break;
                case 0x5: dstP = &registers[SP]; break;
                case 0x6: dstP = &registers[DP]; break;
                case 0x7: {
                            if( registers[CR] & 0x1 ) dstP = &registers[CR]; // if in system mode ( 0x4 ), ok
                            else { registers[CR] |= 0x20; dstP = srcP; }     // else set ILL instr
                            break;
                          }
            }

            *dstP = *srcP;
        }
        else if( SET == (instr & 0xC0) )
        {
            short int tmp = memory[registers[PC]++];
            const char *rst[] = { "NONE", "UP", "DOWN", "ALL" };
            const char *set[] = { "UP", "DOWN" };
            const char *dst[] = { "A", "B", "C", "D", "L", "P", "DP", "CR" };

            dbgStream << "SET (clean " << rst[(instr & 0x30)>>4] << ") " << set[(instr & 8)>>3] << ", " << dst[instr & 0x7] << std::endl;
            //SET = 0xC0      // xx y zzz :: reset NONE/UP/DOWN/ALL :: set UP/DOWN :: A/B/C/D/L/P/DP/CR

            short int mask = 0xFF00 * (~instr & 0x10) | 0x00FF * (~instr & 0x20);
            //dbgStream << "SET 0x" << std::hex << (instr & 0x3F) << "\timmediate value: " << (int)tmp << "\tmask: " << mask << std::endl;

            if( ~instr & 0x08 ) tmp <<= 8; // set upper part 

            switch( instr & 0x07 ) {
                case 0x0: registers[A]  = ( registers[A]  & mask ) | tmp; break;
                case 0x1: registers[B]  = ( registers[B]  & mask ) | tmp; break;
                case 0x2: registers[C]  = ( registers[C]  & mask ) | tmp; break;
                case 0x3: registers[D]  = ( registers[D]  & mask ) | tmp; break;
                case 0x4: registers[L]  = ( registers[L]  & mask ) | tmp; break;
                case 0x5: registers[P]  = ( registers[P]  & mask ) | tmp; break;
                case 0x6: registers[DP] = ( registers[DP] & mask ) | tmp; break;
                case 0x7: {
                            if( registers[CR] & 0x1 ) registers[CR] = ( registers[CR] & mask ) | tmp; // if in system mode ( 0x1 ), ok
                            else registers[CR] |= 0x20;                                                // else set ILL instr
                            break;
                          }
            }
        }
        else std::cout << "illegal instruction " << instr << " PC now " << registers[PC] << std::endl;
        return 0;
    }

    void print_registers( void ) {
        std::cout   //<< "clock: " << clock
                    << "\tPC: " << registers[PC]
                    << "\tA: "  << registers[A] << '(' << (char)(registers[A] & 0xFF) << (char)(registers[A]>>8) << ')'
                    << "\tC: "  << registers[C]
                    << "\tD: "  << registers[D]
                    << "\tL: "  << registers[L]
                    << "\tDP: " << registers[DP]
                    << "\tSP: " << registers[SP]
                    << "\tCR: " << registers[CR]
                    << " MEM @ 2048 '" << (char*)(memory+2048) << "'" << std::endl;
    }

};

int main() {
    CPU cpu;
    cpu.print_registers();
    short instr;
    while( (instr = cpu.fetch_instruction()) ) {
        cpu.print_registers();
        cpu.execute_instruction( instr );
    }
    std::cout << "got zero it seems.\n";
    return 0;
}
