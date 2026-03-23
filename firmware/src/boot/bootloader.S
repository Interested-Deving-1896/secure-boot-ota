/*
 * Secure Bootloader Entry Point
 * ARM Cortex-M Assembly
 */

.syntax unified
.cpu cortex-m4
.fpu softvfp
.thumb

.global _start
.type _start, %function

_start:
    /* Disable interrupts during boot */
    cpsid i
   
    /* Initialize stack pointer from vector table */
    ldr sp, =_estack
   
    /* Copy .data section from flash to RAM */
    ldr r0, =_sdata
    ldr r1, =_edata
    ldr r2, =_sidata
    b .copy_data_loop
   
 .copy_data_loop:
    cmp r0, r1
    bge .zero_bss
    ldr r3, [r2], #4
    str r3, [r0], #4
    b .copy_data_loop
   
 .zero_bss:
    /* Zero out .bss section */
    ldr r0, =_sbss
    ldr r1, =_ebss
    movs r2, #0
    b .zero_bss_loop
   
 .zero_bss_loop:
    cmp r0, r1
    bge .init_vectors
    str r2, [r0], #4
    b .zero_bss_loop
   
 .init_vectors:
    /* Initialize vector table offset register */
    ldr r0, =0x08000000
    msr VTOR, r0
   
    /* Jump to application if signature verified */
    bl secure_boot_verify_application
    cmp r0, #0
    beq .boot_app
   
    /* Bootloader stays active if verification fails */
    b .bootloader_main
   
 .boot_app:
    /* Set application stack pointer */
    ldr sp, =_estack_app
   
    /* Jump to application reset handler */
    ldr r0, =_start_app
    bx r0
   
 .bootloader_main:
    /* Wait loop with LED indication */
    b .bootloader_main

/* Secure boot verification function */
.global secure_boot_verify_application
.type secure_boot_verify_application, %function
secure_boot_verify_application:
    /* Call C function for signature verification */
    bl secure_boot_verify_firmware
    bx lr

.end

