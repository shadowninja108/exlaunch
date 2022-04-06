/*
 *  @date   : 2018/04/18
 *  @author : Rprop (r_prop@outlook.com)
 *  https://github.com/Rprop/And64InlineHook
 */
/*
 MIT License

 Copyright (c) 2018 Rprop (r_prop@outlook.com)

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.
 */
#define __STDC_FORMAT_MACROS
#include <cstring>
#include <stdlib.h>

#include "nx_hook.hpp"
#include "rw_pages.hpp"
#include "mem_layout.hpp"


#define __attribute __attribute__
#define aligned(x) __aligned__(x)
#define __intval(p) reinterpret_cast<intptr_t>(p)
#define __uintval(p) reinterpret_cast<uintptr_t>(p)
#define __ptr(p) reinterpret_cast<void*>(p)
#define __page_size PAGE_SIZE
#define __page_align(n) __align_up(static_cast<uintptr_t>(n), __page_size)
#define __ptr_align(x) __ptr(__align_down(reinterpret_cast<uintptr_t>(x), __page_size))
#define __align_up(x, n) (((x) + ((n)-1)) & ~((n)-1))
#define __align_down(x, n) ((x) & -(n))
#define __countof(x) static_cast<intptr_t>(sizeof(x) / sizeof((x)[0]))  // must be signed
#define __atomic_increase(p) __sync_add_and_fetch(p, 1)
#define __sync_cmpswap(p, v, n) __sync_bool_compare_and_swap(p, v, n)

namespace exl::util {

    namespace {

        // Hooking constants
        constexpr s64 MaxInstructions = 5;
        constexpr u64 HookMax = 10;
        constexpr size_t TrampolineSize = MaxInstructions * 10;
        constexpr u64 MaxReferences = MaxInstructions * 2;
        constexpr u32 Aarch64Nop = 0xd503201f;
        typedef uint32_t HookPool[HookMax][TrampolineSize];
        constexpr size_t HookPoolSize = ALIGN_UP(sizeof(HookPool), PAGE_SIZE);

        // Inline hooking constants
        constexpr size_t InlineHookHandlerSize = 0xC; // correct if handler size changes
        struct PACKED InlineHookEntry {
            std::array<u8, InlineHookHandlerSize> m_Handler;
            void* m_CurrentHandler;
            void* m_Callback;
            void* m_Trampoline;
        };
        constexpr size_t InlineHookSize = sizeof(InlineHookEntry);
        constexpr size_t InlineHookMax = 1;
        constexpr size_t InlineHookPoolSize = InlineHookSize * InlineHookMax;

        extern "C" {
            extern const u8  InlineHandler[InlineHookSize];
            extern const u64 InlineHandlerImpl;
            extern const u64 ExInlineHandlerImpl;
        }

        typedef uint32_t* __restrict* __restrict instruction;
        typedef struct {
            struct fix_info {
                uint32_t* bprx;
                uint32_t* bprw;
                uint32_t ls;  // left-shift counts
                uint32_t ad;  // & operand
            };
            struct insns_info {
                union {
                    uint64_t insu;
                    int64_t ins;
                    void* insp;
                };
                fix_info fmap[MaxReferences];
            };
            int64_t basep;
            int64_t endp;
            insns_info dat[MaxInstructions];

        public:
            inline bool is_in_fixing_range(const int64_t absolute_addr) {
                return absolute_addr >= this->basep && absolute_addr < this->endp;
            }
            inline intptr_t get_ref_ins_index(const int64_t absolute_addr) {
                return static_cast<intptr_t>((absolute_addr - this->basep) / sizeof(uint32_t));
            }
            inline intptr_t get_and_set_current_index(uint32_t* __restrict inp, uint32_t* __restrict outp) {
                intptr_t current_idx = this->get_ref_ins_index(reinterpret_cast<int64_t>(inp));
                this->dat[current_idx].insp = outp;
                return current_idx;
            }
            inline void reset_current_ins(const intptr_t idx, uint32_t* __restrict outp) { this->dat[idx].insp = outp; }
            void insert_fix_map(const intptr_t idx, uint32_t* bprw, uint32_t* bprx, uint32_t ls = 0u, uint32_t ad = 0xffffffffu) {
                for (auto& f : this->dat[idx].fmap) {
                    if (f.bprw == NULL) {
                        f.bprw = bprw;
                        f.bprx = bprx;
                        f.ls = ls;
                        f.ad = ad;
                        return;
                    }  // if
                }
                // What? GGing..
            }
            void process_fix_map(const intptr_t idx) {
                for (auto& f : this->dat[idx].fmap) {
                    if (f.bprw == NULL) break;
                    *(f.bprw) =
                        *(f.bprx) | (((int32_t(this->dat[idx].ins - reinterpret_cast<int64_t>(f.bprx)) >> 2) << f.ls) & f.ad);
                    f.bprw = NULL;
                    f.bprx = NULL;
                }
            }
        } context;

        //-------------------------------------------------------------------------

        bool __fix_branch_imm(instruction inprwp, instruction inprxp, instruction outprw, instruction outprx,
                                    context* ctxp) {
            constexpr uint32_t mbits = 6u;
            constexpr uint32_t mask = 0xfc000000u;   // 0b11111100000000000000000000000000
            constexpr uint32_t rmask = 0x03ffffffu;  // 0b00000011111111111111111111111111
            constexpr uint32_t op_b = 0x14000000u;   // "b"  ADDR_PCREL26
            constexpr uint32_t op_bl = 0x94000000u;  // "bl" ADDR_PCREL26

            const uint32_t ins = *(*inprwp);
            const uint32_t opc = ins & mask;
            switch (opc) {
                case op_b:
                case op_bl: {
                    intptr_t current_idx = ctxp->get_and_set_current_index(*inprxp, *outprx);
                    int64_t absolute_addr = reinterpret_cast<int64_t>(*inprxp) +
                                            (static_cast<int32_t>(ins << mbits) >> (mbits - 2u));  // sign-extended
                    int64_t new_pc_offset =
                        static_cast<int64_t>(absolute_addr - reinterpret_cast<int64_t>(*outprx)) >> 2;  // shifted
                    bool special_fix_type = ctxp->is_in_fixing_range(absolute_addr);
                    // whether the branch should be converted to absolute jump
                    if (!special_fix_type && llabs(new_pc_offset) >= (rmask >> 1)) {
                        bool b_aligned = (reinterpret_cast<uint64_t>(*outprx + 2) & 7u) == 0u;
                        if (opc == op_b) {
                            if (b_aligned != true) {
                                (*outprw)[0] = Aarch64Nop;
                                ctxp->reset_current_ins(current_idx, ++(*outprx));
                                ++(*outprw);
                            }                            // if
                            (*outprw)[0] = 0x58000051u;  // LDR X17, #0x8
                            (*outprw)[1] = 0xd61f0220u;  // BR X17
                            memcpy(*outprw + 2, &absolute_addr, sizeof(absolute_addr));
                            *outprx += 4;
                            *outprw += 4;
                        } else {
                            if (b_aligned == true) {
                                (*outprw)[0] = Aarch64Nop;
                                ctxp->reset_current_ins(current_idx, ++(*outprx));
                                (*outprw)++;
                            }                            // if
                            (*outprw)[0] = 0x58000071u;  // LDR X17, #12
                            (*outprw)[1] = 0x1000009eu;  // ADR X30, #16
                            (*outprw)[2] = 0xd61f0220u;  // BR X17
                            memcpy(*outprw + 3, &absolute_addr, sizeof(absolute_addr));
                            *outprw += 5;
                            *outprx += 5;
                        }  // if
                    } else {
                        if (special_fix_type) {
                            intptr_t ref_idx = ctxp->get_ref_ins_index(absolute_addr);
                            if (ref_idx <= current_idx) {
                                new_pc_offset =
                                    static_cast<int64_t>(ctxp->dat[ref_idx].ins - reinterpret_cast<int64_t>(*outprx)) >> 2;
                            } else {
                                ctxp->insert_fix_map(ref_idx, *outprw, *outprx, 0u, rmask);
                                new_pc_offset = 0;
                            }  // if
                        }      // if

                        (*outprw)[0] = opc | (new_pc_offset & ~mask);
                        ++(*outprw);
                        ++(*outprx);
                    }  // if

                    ++(*inprxp);
                    ++(*inprwp);
                    return ctxp->process_fix_map(current_idx), true;
                }
            }
            return false;
        }

        //-------------------------------------------------------------------------

        bool __fix_cond_comp_test_branch(instruction inprwp, instruction inprxp, instruction outprw, instruction outprx,
                                                context* ctxp) {
            constexpr uint32_t lsb = 5u;
            constexpr uint32_t lmask01 = 0xff00001fu;  // 0b11111111000000000000000000011111
            constexpr uint32_t mask0 = 0xff000010u;    // 0b11111111000000000000000000010000
            constexpr uint32_t op_bc = 0x54000000u;    // "b.c"  ADDR_PCREL19
            constexpr uint32_t mask1 = 0x7f000000u;    // 0b01111111000000000000000000000000
            constexpr uint32_t op_cbz = 0x34000000u;   // "cbz"  Rt, ADDR_PCREL19
            constexpr uint32_t op_cbnz = 0x35000000u;  // "cbnz" Rt, ADDR_PCREL19
            constexpr uint32_t lmask2 = 0xfff8001fu;   // 0b11111111111110000000000000011111
            constexpr uint32_t mask2 = 0x7f000000u;    // 0b01111111000000000000000000000000
            constexpr uint32_t op_tbz =
                0x36000000u;  // 0b00110110000000000000000000000000 "tbz"  Rt, BIT_NUM, ADDR_PCREL14
            constexpr uint32_t op_tbnz =
                0x37000000u;  // 0b00110111000000000000000000000000 "tbnz" Rt, BIT_NUM, ADDR_PCREL14

            const uint32_t ins = *(*inprwp);
            uint32_t lmask = lmask01;
            if ((ins & mask0) != op_bc) {
                uint32_t opc = ins & mask1;
                if (opc != op_cbz && opc != op_cbnz) {
                    opc = ins & mask2;
                    if (opc != op_tbz && opc != op_tbnz) {
                        return false;
                    }  // if
                    lmask = lmask2;
                }  // if
            }      // if

            intptr_t current_idx = ctxp->get_and_set_current_index(*inprxp, *outprx);
            int64_t absolute_addr = reinterpret_cast<int64_t>(*inprxp) + ((ins & ~lmask) >> (lsb - 2u));
            int64_t new_pc_offset = static_cast<int64_t>(absolute_addr - reinterpret_cast<int64_t>(*outprx)) >> 2;  // shifted
            bool special_fix_type = ctxp->is_in_fixing_range(absolute_addr);
            if (!special_fix_type && llabs(new_pc_offset) >= (~lmask >> (lsb + 1))) {
                if ((reinterpret_cast<uint64_t>(*outprx + 4) & 7u) != 0u) {
                    (*outprw)[0] = Aarch64Nop;
                    ctxp->reset_current_ins(current_idx, *outprx);

                    (*outprx)++;
                    (*outprw)++;
                }                                                               // if
                (*outprw)[0] = (((8u >> 2u) << lsb) & ~lmask) | (ins & lmask);  // B.C #0x8
                (*outprw)[1] = 0x14000005u;                                     // B #0x14
                (*outprw)[2] = 0x58000051u;                                     // LDR X17, #0x8
                (*outprw)[3] = 0xd61f0220u;                                     // BR X17
                memcpy(*outprw + 4, &absolute_addr, sizeof(absolute_addr));
                *outprw += 6;
                *outprx += 6;
            } else {
                if (special_fix_type) {
                    intptr_t ref_idx = ctxp->get_ref_ins_index(absolute_addr);
                    if (ref_idx <= current_idx) {
                        new_pc_offset = static_cast<int64_t>(ctxp->dat[ref_idx].ins - reinterpret_cast<int64_t>(*outprx)) >> 2;
                    } else {
                        ctxp->insert_fix_map(ref_idx, *outprw, *outprx, lsb, ~lmask);
                        new_pc_offset = 0;
                    }  // if
                }      // if

                (*outprw)[0] = (static_cast<uint32_t>(new_pc_offset << lsb) & ~lmask) | (ins & lmask);
                ++(*outprw);
                ++(*outprx);
            }  // if

            ++(*inprxp);
            ++(*inprwp);
            return ctxp->process_fix_map(current_idx), true;
        }

        //-------------------------------------------------------------------------

        bool __fix_loadlit(instruction inprwp, instruction inprxp, instruction outprw, instruction outprx,
                                context* ctxp) {
            const uint32_t ins = *(*inprwp);

            // memory prefetch("prfm"), just skip it
            // http://infocenter.arm.com/help/topic/com.arm.doc.100069_0608_00_en/pge1427897420050.html
            if ((ins & 0xff000000u) == 0xd8000000u) {
                ctxp->process_fix_map(ctxp->get_and_set_current_index(*inprxp, *outprx));
                ++(*inprwp);
                ++(*inprxp);
                return true;
            }  // if

            constexpr uint32_t msb = 8u;
            constexpr uint32_t lsb = 5u;
            constexpr uint32_t mask_30 = 0x40000000u;   // 0b01000000000000000000000000000000
            constexpr uint32_t mask_31 = 0x80000000u;   // 0b10000000000000000000000000000000
            constexpr uint32_t lmask = 0xff00001fu;     // 0b11111111000000000000000000011111
            constexpr uint32_t mask_ldr = 0xbf000000u;  // 0b10111111000000000000000000000000
            constexpr uint32_t op_ldr =
                0x18000000u;  // 0b00011000000000000000000000000000 "LDR Wt/Xt, label" | ADDR_PCREL19
            constexpr uint32_t mask_ldrv = 0x3f000000u;  // 0b00111111000000000000000000000000
            constexpr uint32_t op_ldrv =
                0x1c000000u;  // 0b00011100000000000000000000000000 "LDR St/Dt/Qt, label" | ADDR_PCREL19
            constexpr uint32_t mask_ldrsw = 0xff000000u;  // 0b11111111000000000000000000000000
            constexpr uint32_t op_ldrsw = 0x98000000u;  // "LDRSW Xt, label" | ADDR_PCREL19 | load register signed word
            // LDR S0, #0 | 0b00011100000000000000000000000000 | 32-bit
            // LDR D0, #0 | 0b01011100000000000000000000000000 | 64-bit
            // LDR Q0, #0 | 0b10011100000000000000000000000000 | 128-bit
            // INVALID    | 0b11011100000000000000000000000000 | may be 256-bit

            uint32_t mask = mask_ldr;
            uintptr_t faligned = (ins & mask_30) ? 7u : 3u;
            if ((ins & mask_ldr) != op_ldr) {
                mask = mask_ldrv;
                if (faligned != 7u) faligned = (ins & mask_31) ? 15u : 3u;
                if ((ins & mask_ldrv) != op_ldrv) {
                    if ((ins & mask_ldrsw) != op_ldrsw) {
                        return false;
                    }  // if
                    mask = mask_ldrsw;
                    faligned = 7u;
                }  // if
            }      // if

            intptr_t current_idx = ctxp->get_and_set_current_index(*inprxp, *outprx);
            int64_t absolute_addr =
                reinterpret_cast<int64_t>(*inprxp) + ((static_cast<int32_t>(ins << msb) >> (msb + lsb - 2u)) & ~3u);
            int64_t new_pc_offset = static_cast<int64_t>(absolute_addr - reinterpret_cast<int64_t>(*outprx)) >> 2;  // shifted
            bool special_fix_type = ctxp->is_in_fixing_range(absolute_addr);
            // special_fix_type may encounter issue when there are mixed data and code
            if (special_fix_type ||
                (llabs(new_pc_offset) + (faligned + 1u - 4u) / 4u) >= (~lmask >> (lsb + 1))) {  // inaccurate, but it works
                while ((reinterpret_cast<uint64_t>(*outprx + 2) & faligned) != 0u) {
                    *(*outprw)++ = Aarch64Nop;
                    (*outprx)++;
                }
                ctxp->reset_current_ins(current_idx, *outprx);

                // Note that if memory at absolute_addr is writeable (non-const), we will fail to fetch it.
                // And what's worse, we may unexpectedly overwrite something if special_fix_type is true...
                uint32_t ns = static_cast<uint32_t>((faligned + 1) / sizeof(uint32_t));
                (*outprw)[0] = (((8u >> 2u) << lsb) & ~mask) | (ins & lmask);  // LDR #0x8
                (*outprw)[1] = 0x14000001u + ns;                               // B #0xc
                memcpy(*outprw + 2, reinterpret_cast<void*>(absolute_addr), faligned + 1);
                *outprw += 2 + ns;
                *outprx += 2 + ns;
            } else {
                faligned >>= 2;  // new_pc_offset is shifted and 4-byte aligned
                while ((new_pc_offset & faligned) != 0) {
                    *(*outprw)++ = Aarch64Nop;
                    (*outprx)++;
                    new_pc_offset = static_cast<int64_t>(absolute_addr - reinterpret_cast<int64_t>(*outprx)) >> 2;
                }
                ctxp->reset_current_ins(current_idx, *outprx);

                (*outprw)[0] = (static_cast<uint32_t>(new_pc_offset << lsb) & ~mask) | (ins & lmask);
                ++(*outprx);
                ++(*outprw);
            }  // if

            ++(*inprxp);
            ++(*inprwp);
            return ctxp->process_fix_map(current_idx), true;
        }

        //-------------------------------------------------------------------------

        bool __fix_pcreladdr(instruction inprwp, instruction inprxp, instruction outprw, instruction outprx,
                                    context* ctxp) {
            // Load a PC-relative address into a register
            // http://infocenter.arm.com/help/topic/com.arm.doc.100069_0608_00_en/pge1427897645644.html
            constexpr uint32_t msb = 8u;
            constexpr uint32_t lsb = 5u;
            constexpr uint32_t mask = 0x9f000000u;     // 0b10011111000000000000000000000000
            constexpr uint32_t rmask = 0x0000001fu;    // 0b00000000000000000000000000011111
            constexpr uint32_t lmask = 0xff00001fu;    // 0b11111111000000000000000000011111
            constexpr uint32_t fmask = 0x00ffffffu;    // 0b00000000111111111111111111111111
            constexpr uint32_t max_val = 0x001fffffu;  // 0b00000000000111111111111111111111
            constexpr uint32_t op_adr = 0x10000000u;   // "adr"  Rd, ADDR_PCREL21
            constexpr uint32_t op_adrp = 0x90000000u;  // "adrp" Rd, ADDR_ADRP

            const uint32_t ins = *(*inprwp);
            intptr_t current_idx;
            switch (ins & mask) {
                case op_adr: {
                    current_idx = ctxp->get_and_set_current_index(*inprxp, *outprx);
                    int64_t lsb_bytes = static_cast<uint32_t>(ins << 1u) >> 30u;
                    int64_t absolute_addr = reinterpret_cast<int64_t>(*inprxp) +
                                            (((static_cast<int32_t>(ins << msb) >> (msb + lsb - 2u)) & ~3u) | lsb_bytes);
                    int64_t new_pc_offset = static_cast<int64_t>(absolute_addr - reinterpret_cast<int64_t>(*outprx));
                    bool special_fix_type = ctxp->is_in_fixing_range(absolute_addr);
                    if (!special_fix_type && llabs(new_pc_offset) >= (max_val >> 1)) {
                        if ((reinterpret_cast<uint64_t>(*outprx + 2) & 7u) != 0u) {
                            (*outprw)[0] = Aarch64Nop;
                            ctxp->reset_current_ins(current_idx, ++(*outprx));
                            ++*(outprw);
                        }  // if

                        (*outprw)[0] = 0x58000000u | (((8u >> 2u) << lsb) & ~mask) | (ins & rmask);  // LDR #0x8
                        (*outprw)[1] = 0x14000003u;                                                  // B #0xc
                        memcpy(*outprw + 2, &absolute_addr, sizeof(absolute_addr));
                        *outprw += 4;
                        *outprx += 4;
                    } else {
                        if (special_fix_type) {
                            intptr_t ref_idx = ctxp->get_ref_ins_index(absolute_addr & ~3ull);
                            if (ref_idx <= current_idx) {
                                new_pc_offset =
                                    static_cast<int64_t>(ctxp->dat[ref_idx].ins - reinterpret_cast<int64_t>(*outprx));
                            } else {
                                ctxp->insert_fix_map(ref_idx, *outprw, *outprx, lsb, fmask);
                                new_pc_offset = 0;
                            }  // if
                        }      // if

                        // the lsb_bytes will never be changed, so we can use lmask to keep it
                        (*outprw)[0] = (static_cast<uint32_t>(new_pc_offset << (lsb - 2u)) & fmask) | (ins & lmask);
                        ++(*outprw);
                        ++(*outprx);
                    }  // if
                } break;
                case op_adrp: {
                    current_idx = ctxp->get_and_set_current_index(*inprxp, *outprw);
                    int32_t lsb_bytes = static_cast<uint32_t>(ins << 1u) >> 30u;
                    int64_t absolute_addr =
                        (reinterpret_cast<int64_t>(*inprxp) & ~0xfffll) +
                        ((((static_cast<int32_t>(ins << msb) >> (msb + lsb - 2u)) & ~3u) | lsb_bytes) << 12);
                    if (ctxp->is_in_fixing_range(absolute_addr)) {
                        intptr_t ref_idx = ctxp->get_ref_ins_index(absolute_addr /* & ~3ull*/);
                        if (ref_idx > current_idx) {
                            // the bottom 12 bits of absolute_addr are masked out,
                            // so ref_idx must be less than or equal to current_idx!
                            /*skyline::logger::s_Instance->Log(
                                "[And64InlineHook] ref_idx must be less than or equal to current_idx!\n");*/
                        }  // if

                        // *absolute_addr may be changed due to relocation fixing
                        *(*outprw)++ = ins;  // 0x90000000u;
                        (*outprx)++;
                    } else {
                        if ((reinterpret_cast<uint64_t>(*outprx + 2) & 7u) != 0u) {
                            (*outprw)[0] = Aarch64Nop;
                            ctxp->reset_current_ins(current_idx, ++(*outprx));
                            ++*(outprw);
                        }  // if

                        (*outprw)[0] = 0x58000000u | (((8u >> 2u) << lsb) & ~mask) | (ins & rmask);  // LDR #0x8
                        (*outprw)[1] = 0x14000003u;                                                  // B #0xc
                        memcpy(*outprw + 2, &absolute_addr, sizeof(absolute_addr));                  // potential overflow?
                        *outprw += 4;
                        *outprx += 4;
                    }  // if
                } break;
                default:
                    return false;
            }

            ctxp->process_fix_map(current_idx);
            ++(*inprxp);
            ++(*inprwp);
            return true;
        }

        #define __flush_cache(c, n) __builtin___clear_cache(reinterpret_cast<char*>(c), reinterpret_cast<char*>(c) + n)

        //-------------------------------------------------------------------------

        void __fix_instructions(uint32_t* __restrict inprw, uint32_t* __restrict inprx, int32_t count,
                                    uint32_t* __restrict outrwp, uint32_t* __restrict outrxp) {
            context ctx;
            ctx.basep = reinterpret_cast<int64_t>(inprx);
            ctx.endp = reinterpret_cast<int64_t>(inprx + count);
            memset(ctx.dat, 0, sizeof(ctx.dat));
            static_assert(sizeof(ctx.dat) / sizeof(ctx.dat[0]) == MaxInstructions, "please use MaxInstructions!");
        #ifndef NDEBUG
            if (count > MaxInstructions) {
                EXL_ABORT(result::HookFixingTooManyInstructions);
            }   // if
        #endif  // NDEBUG

            uint32_t* const outprx_base = outrxp;
            uint32_t* const outprw_base = outrwp;

            while (--count >= 0) {
                if (__fix_branch_imm(&inprw, &inprx, &outrwp, &outrxp, &ctx)) continue;
                if (__fix_cond_comp_test_branch(&inprw, &inprx, &outrwp, &outrxp, &ctx)) continue;
                if (__fix_loadlit(&inprw, &inprx, &outrwp, &outrxp, &ctx)) continue;
                if (__fix_pcreladdr(&inprw, &inprx, &outrwp, &outrxp, &ctx)) continue;

                // without PC-relative offset
                ctx.process_fix_map(ctx.get_and_set_current_index(inprx, outrxp));
                *(outrwp++) = *(inprw++);
                outrxp++;
                inprx++;
            }

            constexpr uint_fast64_t mask = 0x03ffffffu;  // 0b00000011111111111111111111111111
            auto callback = reinterpret_cast<int64_t>(inprx);
            auto pc_offset = static_cast<int64_t>(callback - reinterpret_cast<int64_t>(outrxp)) >> 2;
            if (llabs(pc_offset) >= (mask >> 1)) {
                if ((reinterpret_cast<uint64_t>(outrxp + 2) & 7u) != 0u) {
                    outrwp[0] = Aarch64Nop;
                    ++outrxp;
                    ++outrwp;
                }                         // if
                outrwp[0] = 0x58000051u;  // LDR X17, #0x8
                outrwp[1] = 0xd61f0220u;  // BR X17
                *reinterpret_cast<int64_t*>(outrwp + 2) = callback;
                outrwp += 4;
                outrxp += 4;
            } else {
                outrwp[0] = 0x14000000u | (pc_offset & mask);  // "B" ADDR_PCREL26
                ++outrwp;
                ++outrxp;
            }  // if

            const uintptr_t total = (outrxp - outprx_base) * sizeof(uint32_t);
            // __flush_cache(outprx_base, total);  // necessary
            __flush_cache(outprw_base, total);
        }
    }

//-------------------------------------------------------------------------

Jit Hook::s_HookJit;
Jit Hook::s_InlineHookJit;
size_t Hook::s_UsedInlineHooks;
//static nn::os::MutexType hookMutex;

//-------------------------------------------------------------------------

void Hook::Initialize() {
    /* TODO: thread safety */

    alignas(PAGE_SIZE) static u8 hookJitRw[HookPoolSize] = {};

    R_ABORT_UNLESS(jitCreate(&s_HookJit, &hookJitRw, HookPoolSize));
    R_ABORT_UNLESS(jitTransitionToExecutable(&s_HookJit));

    auto current_search_addr = GetMainModuleInfo().m_Text.m_Start - InlineHookPoolSize;

    MemoryInfo mem;
    while (true) {
        u32 page_info;
        if (R_SUCCEEDED(svcQueryMemory(&mem, &page_info, current_search_addr)) && mem.type == MemType_Unmapped
            && mem.size >= ALIGN_UP(InlineHookPoolSize, PAGE_SIZE))
        {
            break;
        }

        current_search_addr -= PAGE_SIZE;
    }

    R_ABORT_UNLESS(jitCreate(&s_InlineHookJit, (void*)ALIGN_DOWN(mem.addr + mem.size - InlineHookPoolSize, PAGE_SIZE), InlineHookPoolSize));

    s_UsedInlineHooks = 0;
}

//-------------------------------------------------------------------------

Result Hook::AllocForTrampoline(uint32_t** rx, uint32_t** rw) {
    static_assert((TrampolineSize * sizeof(uint32_t)) % 8 == 0, "8-byte align");
    static volatile s32 index = -1;

    uint32_t i = __atomic_increase(&index);
    
    if(i > HookMax)
        return result::HookTrampolineAllocFail;

    HookPool* rwptr = (HookPool*)s_HookJit.rw_addr;
    HookPool* rxptr = (HookPool*)s_HookJit.rx_addr;
    *rw = (*rwptr)[i];
    *rx = (*rxptr)[i];

    return result::Success;
}

//-------------------------------------------------------------------------

static bool HookFuncImpl(void* const symbol, void* const replace, void* const rxtr, void* const rwtr) {
    static constexpr uint_fast64_t mask = 0x03ffffffu;  // 0b00000011111111111111111111111111

    uint32_t *rxtrampoline = static_cast<uint32_t*>(rxtr), *rwtrampoline = static_cast<uint32_t*>(rwtr),
             *original = static_cast<uint32_t*>(symbol);

    static_assert(MaxInstructions >= 5, "please fix MaxInstructions!");
    auto pc_offset = static_cast<int64_t>(__intval(replace) - __intval(symbol)) >> 2;
    if (llabs(pc_offset) >= (mask >> 1)) {
        const exl::util::RwPages ctrl((uintptr_t)original, 5 * sizeof(uint32_t));

        int32_t count = (reinterpret_cast<uint64_t>(original + 2) & 7u) != 0u ? 5 : 4;

        original = (u32*)ctrl.GetRw();

        if (rxtrampoline) {
            if (TrampolineSize < count * 10u) {
                return false;
            }  // if
            __fix_instructions(original, (u32*)ctrl.GetRo(), count, rwtrampoline, rxtrampoline);
        }  // if

        if (count == 5) {
            original[0] = Aarch64Nop;
            ++original;
        }                           // if
        original[0] = 0x58000051u;  // LDR X17, #0x8
        original[1] = 0xd61f0220u;  // BR X17
        *reinterpret_cast<int64_t*>(original + 2) = __intval(replace);
        __flush_cache(symbol, 5 * sizeof(uint32_t));
    } else {
        const exl::util::RwPages ctrl((uintptr_t)original, 1 * sizeof(uint32_t));

        original = (u32*)ctrl.GetRw();

        if (rwtrampoline) {
            if (TrampolineSize < 1u * 10u) {
                return false;
            }  // if
            __fix_instructions(original, (u32*)ctrl.GetRo(), 1, rwtrampoline, rxtrampoline);
        }  // if

        __sync_cmpswap(original, *original, 0x14000000u | (pc_offset & mask));  // "B" ADDR_PCREL26
        __flush_cache(symbol, 1 * sizeof(uint32_t));
    }  // if

    return true;
}

uintptr_t Hook::HookFuncCommon(uintptr_t hook, uintptr_t callback, bool do_trampoline) {
    /* TODO: thread safety */

    R_ABORT_UNLESS(jitTransitionToWritable(&s_HookJit));

    u32* rxtrampoline = NULL;
    u32* rwtrampoline = NULL;
    if (do_trampoline) 
        R_ABORT_UNLESS(AllocForTrampoline(&rxtrampoline, &rwtrampoline));

    if (!HookFuncImpl(reinterpret_cast<void*>(hook), reinterpret_cast<void*>(callback), rxtrampoline, rwtrampoline))
        EXL_ABORT(exl::result::HookFailed);

    R_ABORT_UNLESS(jitTransitionToExecutable(&s_HookJit));

    return (uintptr_t) rxtrampoline;
}

void Hook::InlineHook(uintptr_t hook, uintptr_t callback, bool is_extended) {
    R_ABORT_UNLESS(jitTransitionToExecutable(&s_HookJit));

    auto handler_start = (uintptr_t)&InlineHandler;
    auto handler_end = handler_start + InlineHookHandlerSize;

    EXL_ASSERT(*reinterpret_cast<u32*>(handler_end) == 0x5F4C5845, "Invalid handler size");

    EXL_ASSERT(InlineHookSize > s_UsedInlineHooks, "Number of inline hooks has exceeded the maximum");

    auto& rx_entries = *reinterpret_cast<std::array<InlineHookEntry, InlineHookMax>*>(s_InlineHookJit.rx_addr);
    auto& rx = rx_entries[s_UsedInlineHooks];

    uintptr_t trampoline = HookFuncCommon(hook, reinterpret_cast<uintptr_t>(rx.m_Handler.data()), true);

    jitTransitionToWritable(&s_InlineHookJit);

    auto& rw_entries = *reinterpret_cast<std::array<InlineHookEntry, InlineHookMax>*>(s_InlineHookJit.rw_addr);
    auto& rw = rw_entries[s_UsedInlineHooks];

    std::memcpy(rw.m_Handler.data(), reinterpret_cast<const void*>(InlineHandler), InlineHookHandlerSize);

    if (is_extended) {
        rw.m_CurrentHandler = const_cast<void*>(reinterpret_cast<const void*>(&ExInlineHandlerImpl));
    } else {
        rw.m_CurrentHandler = const_cast<void*>(reinterpret_cast<const void*>(&InlineHandlerImpl));
    }

    rw.m_Callback = reinterpret_cast<void*>(callback);
    rw.m_Trampoline = reinterpret_cast<void*>(trampoline);

    jitTransitionToExecutable(&s_InlineHookJit);

    s_UsedInlineHooks++;
}

};