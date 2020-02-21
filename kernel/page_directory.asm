[global load_page_directory]
load_page_directory:
    push ebp
    mov ebp, esp

    ; Mov
    mov eax, [ebp+8]
    mov cr3, eax

    ; Return 1 for a success
    mov eax, 1

    mov esp, ebp
    pop ebp
    ret

[global enable_paging]
enable_paging:
    push ebp
    mov ebp, esp

    ; Enable the 32 bit for paging within CR0
    mov eax, cr0
    or eax, 0x80000000
    mov cr0, eax

    ; Return 1 for a success
    mov eax, 1

    mov esp, ebp
    pop ebp
    ret
