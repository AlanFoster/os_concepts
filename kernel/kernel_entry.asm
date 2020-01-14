[bits 32]
[extern main] ; Additional meta-data for the linker to subtitute the final address for our kernel's entry point
call main     ; Call the main function defined within our C kernel
jmp $
