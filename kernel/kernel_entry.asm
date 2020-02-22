global _start
[bits 32]

_start:
    extern kernel_main   ; Additional meta-data for the linker to subtitute the final address for our kernel's entry point
    call kernel_main     ; Call the main function defined within our C kernel
    jmp $
