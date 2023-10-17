/* Copyright (C) 2019  Adam Green (https://github.com/adamgreen)

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

#include "CrashCatcherPriv.h"

/* Implementation of ARMv7-M assembly language code to trap exceptions and call CrashCatcher_Entry(). */
    .text
    .syntax unified
    .arch armv7-m

    /*
     * Called on Hard Fault exception.
     * Stacks important registers and calls CrashCatcher_Entry().
     */

    /* extern "C" void HardFault_Handler(void); */
    .global HardFault_Handler
    .type HardFault_Handler, %function
    .thumb_func
HardFault_Handler:
    /* Push the following onto the stack (see CrashCatcherExceptionRegisters structure).
     * The g_crashCatcherStack buffer is reserved for use as the stack while CrashCatcher is running.
     * msp
     * psp
     * exceptionPSR
     * r4
     * r5
     * r6
     * r7
     * r8
     * r9
     * r10
     * r11
     * exceptionLR
     */

    // Prevent double fault
    ldr r0, =crash_orig_stack
    ldr r0, [r0]
    cmp r0, #0
    bne .

    // Create stack for CrashCatcher
    mrs     r3, xpsr
    mrs     r2, psp
    mrs     r1, msp
    ldr     sp, =(g_crashCatcherStack + 4 * CRASH_CATCHER_STACK_WORD_COUNT)
    push.w  {r1-r11,lr}

    // Preserve current CrashCatcher's stack pointer value
    ldr r0, =crash_orig_stack
    str sp, [r0]

    // Call CrashCatcher_Entry with first argument pointing to registers that were just stacked.
    mov r0, sp                      // Set CrashCatcher's stack as first argument to CrashCatcher_Entry
    ldr r2, =CrashCatcher_Entry     // Stack frame exception return function
    mrs r3, psp
    str r2, [r3, #0x18]             // Overwrite existing return address with CrashCatcher_Entry address
    str r0, [r3]                    // Switch PSP to CrashCatcher's stack
    orr lr, lr, #4                  // Use PSP after return
    orr lr, lr, #8                  // Switch to thread mode upon returning
    bx lr                           // Return from exception
    b .                             // Just in case

    // Let assembler know that we have hit the end of the HardFault_Handler function.
    .pool
    .size HardFault_Handler, .-HardFault_Handler

    /*
     * Called from CrashCatcher core to copy all floating point registers to supplied buffer.
     * The supplied buffer must be large enough to contain 33 32-bit values (S0-S31 & FPSCR).
     */

    /* void CrashCatcher_CopyAllFloatingPointRegisters(uint32_t* pBuffer); */
    .global CrashCatcher_CopyAllFloatingPointRegisters
    .type CrashCatcher_CopyAllFloatingPointRegisters, %function
    .thumb_func
CrashCatcher_CopyAllFloatingPointRegisters:
#if CRASH_CATCHER_WITH_FPU

    vmrs        r1, fpscr       // Grab a copy of FPSCR before issuing any other FP instructions.
    vstmia.32   r0!, {s0 - s31} // Move s0 - s31 to pBuffer.
    str         r1, [r0]        // Move fpscr to pBuffer.
#endif
    bx          lr              // Return to caller.
    .pool
    .size       CrashCatcher_CopyAllFloatingPointRegisters, .-CrashCatcher_CopyAllFloatingPointRegisters


    /*
     * Called from CrashCatcher core to copy upper 16 floating point registers to supplied buffer.
     * The supplied buffer must be large enough to contain 16 32-bit values (S16-S31).
     */

    /* void CrashCatcher_CopyUpperFloatingPointRegisters(uint32_t* pBuffer); */
    .global CrashCatcher_CopyUpperFloatingPointRegisters
    .type CrashCatcher_CopyUpperFloatingPointRegisters, %function
    .thumb_func
CrashCatcher_CopyUpperFloatingPointRegisters:
#if CRASH_CATCHER_WITH_FPU
    vstmia.32   r0!, {s16 - s31} // Move s16 - s31 to pBuffer.
#endif
    bx          lr     // Return to caller.
    .pool
    .size CrashCatcher_CopyUpperFloatingPointRegisters, .-CrashCatcher_CopyUpperFloatingPointRegisters


	/*
	 * This function is called by the SVC #1 syscall
	 * and then collect stack data for the crash dump
	 * which will be run in the second stage.
	 * It also changes stack to the static internal
	 * due to possibility of corruption of the original
	 * stack frame.
	 */

	/*	extern "C" void _StackTrace_Dump_svc_1(void); */
    .global _StackTrace_Dump_svc_1
    .type _StackTrace_Dump_svc_1 , %function
    .thumb_func
_StackTrace_Dump_svc_1:
    // Preserve exception stack frame
    mrs     r3, xpsr
    mrs     r2, psp
    mrs     r1, msp
    ldr     sp, =(g_crashCatcherStack + 4 * CRASH_CATCHER_STACK_WORD_COUNT)
    push.w  {r1-r11,lr}

    // Preserve current CrashCatcher's stack pointer value for use in second stage below
	ldr     r0, =crash_orig_stack
    str    	sp, [r0]

    mov     sp, r1 // Switch back to original stack
    bx      lr     // Return from exception
    .pool
    .size _StackTrace_Dump_svc_1, .-_StackTrace_Dump_svc_1


	/*
	 * This function is the second stage of the crash dump
	 * 'crash_orig_stack' should contain valid stack from
	 * the exception, so that CrashCatcher is able to read
	 * stack frame pointer and dump data in the user mode
	 * from the second part of the exit function.
	 * In our model CrashCatcher never returns to the
	 * caller, so simple branch is enough, without caring
	 * about proper unstacking.
	 */

    /* extern "C" void _StackTrace_Dump_stage_2(void); */
    .global _StackTrace_Dump_stage_2
    .type _StackTrace_Dump_stage_2 , %function
    .thumb_func
_StackTrace_Dump_stage_2:
	 ldr r0,    =crash_orig_stack
	 ldr r0,    [r0]                // Pass CrashCatcher's stack as a first argument to CrashCatcher_Entry
     b          CrashCatcher_Entry  // Jump to CrashCatcher_Entry
    .pool
    .size _StackTrace_Dump_stage_2, .-_StackTrace_Dump_stage_2

    .data
    crash_orig_stack: .long 0 // Stores CrashCatcher stack pointer position
