bits 16                        ; We're dealing with 16 bit code currently
org 0x7c00                     ; Inform the assembler of the starting location for this code

boot:
    mov ax, 0x3
    int 0x10                   ; set vga text mode 3, provides text interface 80 characters wide, 25 chars per line per screen

    mov [BOOT_DRIVE], dl       ; On boot, the dl register should be set to the current boot drive

    mov bx, DISK_READ_MSG
    call println_string

    ; Preparing to read more than the 512 bytes that was loaded by BIOS.
    mov bp, 0x8000             ; Move our base stack somewhere safe
    mov sp, bp                 ; Move our stack pointer somewhere safe

    mov bx, 0x9000             ; Load the sectors to 0x0000 (ES):0x9000 (BX)
    mov dh, 5                 ; Specify how many sectors to read
    mov dl, [BOOT_DRIVE]       ; Specify which drive
    call disk_load

    call enable_protected_mode
    jmp halt ; This should never occur

halt:
    hlt                        ; Stop

; ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Disk Helpers
; ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; Load DH sectors to ES:BX from drive DL, with DH sectors
disk_load:
  push dx

  ; Set up bios read disk call
  ; https://en.wikipedia.org/wiki/INT_13H#INT_13h_AH=02h:_Read_Sectors_From_Drive

  mov ah, 0x02                ; bios read sector
  mov dl, dl                  ; which drive to read, disk 0 for floppy
  mov al, dh                  ; How many sectors to read
  mov ch, 0x00                ; Which cylinder to read
  mov dh, 0x00                ; Which head to use, 0 for read, 1 for write, 0 based
  mov cl, 0x02                ; Which sector to read from the track, 1 based
  mov bx, bx                  ; copy target

  int 0x13                    ; bios call to read sectors from drive

  jc disk_error               ; CF should be clear if no error

  pop dx                      ; Restore pushed dx register
  cmp dh, al                  ; al is the actual sectors read, ensure that read = expected
  jne disk_error

  ret

disk_error:
  mov bx, DISK_ERROR_MSG
  call println_string
  jmp $

; ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Global descriptor table - used to describe the segments within
; memory. This contains the null segment, code segment, and data
; segment. In this case the approach taken is a basic flat model,
; where both the code and data segments overlap for 4GB, without
; any attempt to separate them. This is not good for security.
; There is no use of paging features for virtual memory either.
;
; Layout: https://wiki.osdev.org/Global_Descriptor_Table
; ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

gdt_start:
gdt_null_descriptor:
  dq 0 ; 8-byte, 64-bit, null descriptor

gdt_code_descriptor:
  ; base=0x0, limit=0xfffff
  ; 1st flags: (present, always 1) 1 (privilege level) 00 (descrptor type, 1 for code or data) 1
  ; type flags: (executable) 1 (conforming) 1 (readable) 1 (accesssesd) 0
  ; 2nd flags: (granularity, 0=1byte,1=4kbyte) 1 (operand size, 0=16bit, 1=32bit) (always 0) 0 (AVL - Available for system use) 0
  dw 0xffff      ; Segment limit, bits 0-15
  dw 0           ; Segment base, bits 0-15
  db 0           ; Segment base, bits 16-23
  db 10011010b   ; Segment access byte, who has access to this memory. Marked as code.
  db 11001111b   ; High 4 bits = flags influencing segment size, lower 4 bit segment limit, bits 16-19
  db 0           ; Segment base, 24-31 bits

gdt_data_descriptor:
  ; Same as code section, but:
  ; type flags: (executable) 0 (expand down) 0 (writable) 1 (accesssesd) 0
  dw 0xffff      ; Segment limit, bits 0-15
  dw 0           ; Segment base, bits 0-15
  db 0           ; Segment base, bits 16-23
  db 10010010b   ; Segment access byte, who has access to this memory. Marked as data.
  db 11001111b   ; High 4 bits = flags influencing segment size, lower 4 bit segment limit, bits 16-19
  db 0           ; Segment base, 24-31 bits

gdt_end:           ; Empty label for simple gdt size calculations belows
gdt_pointer:
  dw gdt_end - gdt_start -1 ; gdt size
  dd gdt_start              ; gdt offset

; Constants for GDT segment descriptor offsets
CODE_SEG equ gdt_code_descriptor - gdt_start
DATA_SEG equ gdt_data_descriptor - gdt_start

; ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Protected mode
; ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

enable_protected_mode:
.enable_a20_address_line:
    mov ax, 0x2401
    int 0x15                   ; Enable the A20 line, or addressing line 20, so that the CPU can access beyond 1mb of data
.clear_interupt_table:
    cli
.enable_protected_mode_within_control_register:
    ; Reading the current value of the control register, and setting the protected mode bit
    mov eax, cr0
    or eax, 0x1  ; Setting the protected bit on the CPU reg cr0
    mov cr0, eax ; Updating the cr0 reg
.load_global_descriptor_table:
    lgdt [gdt_pointer]
    jmp CODE_SEG:initialize_protected_mode ; long jump into the protected mode segment
    ; This additionally forces the cpu to drop any pre-fetched 16 bit instructinos, in preparation for 32 bit mode

; ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; String Helpers
; ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

HEX_OUT:
    db "0x0000", 0 ; Placeholder in memory to store the computation of converting a value in memory to hex, used by print_hex

; Prints the hex value currently stored within the dx register
print_hex:
  pusha                        ; Preserve the original registers
  mov cx, 5                    ; Temporary pointer index to HEX_OUT, decremented on each loop, used to slot in the hex values within the array of chars

.loop:
  cmp cx, 1
  je .end                      ; We've calcuated all 4 hex chars

  mov ax, dx                   ; dx will act as the next 4 bits that we're converting to hex
  and ax, 0xf                  ; mask the right mosts 4 bits for conversion into hex
  add ax, 48                   ; Add ascii '0', i.e. 48 decimal, to the current number
  cmp ax, 58                   ; If ax is greater than 58 decimal, it's beyond '9' in ascii, so it must be ascii a to f instead
  jl .store_hex_and_decrement_pointer

.if_a_to_f:
  add ax, 7                    ; An extra 7 to move from ascii '0' to '9', to 'A' - 'F'

.store_hex_and_decrement_pointer:
  mov bx, HEX_OUT              ; Point ax to the base address of HEX_OUT
  add bx, cx                   ; Increment by our current hex pointer, stored in cx
  mov [bx], al                 ; Update hex_dump[cx] = hex ascii value

  shr dx, 4                    ; shift the number we're printing by 4 bits, so we can prepare to print the next hex value

                               ; Decrement cx pointer
  sub cx, 1
  jmp .loop

.end:
  mov bx, HEX_OUT              ; After mutating HEX_OUT directly, print it as a normal string
  call println_string

  popa                         ; Normal function clean up
  ret

ZERO_ASCII equ 00110000b

print_binary:
  pusha

  mov ah, 0x0e
  mov al, '0'
  int 0x10

  mov al, 'b'
  int 0x10

.loop:
  cmp bx, 0
  je .end

  mov cx, bx
  and cx, 1                    ; cx = bx & 1

  mov ah, 0x0e
  mov al, ZERO_ASCII
  add al, cl                   ; output(ascii_zero + last_bit)
  int 0x10                     ; print

  shr bx, 4                    ; bx = bx >> 4
  jmp .loop

.end:
  popa
  ret

; Prints the 0 terminated string which is indexed by the bx register, with a new line
println_string:
  pusha

  mov bx, bx
  call print_string

  mov bx, NEW_LINE
  call print_string

  popa
  ret

; Prints the given 0 terminated string indexed by the bx register, without a new line
print_string:
  pusha
  mov ah, 0x0e
  mov si, bx

.loop:
  lodsb
  OR al, al
  jz .end
  int 0x10
  jmp .loop

.end:
  popa
  ret

; ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Constants
; ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

DISK_READ_MSG:
    db "Reading disk...", 0

DISK_ERROR_MSG:
    db "Disk read error!", 0

NEW_LINE:
  db 0x0d, 0x0a, 0 ; 0 terminated string withnewline and carriage return

BOOT_DRIVE: db 0

; ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;  Magic boot marker
; ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; Mark the device as bootable
times 510-($-$$) db 0          ; Add any additional zeroes to make 510 bytes in total
dw 0xAA55                      ; Write the final 2 bytes as the magic number 0x55aa, remembering x86 little endian

; ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;  32-bit protected mode begins
; ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

[bits 32]

initialize_protected_mode:
  mov ax, DATA_SEG          ; Replacing the old segment registers with the new data segment, defined within the GDT
  mov ds, ax
  mov es, ax
  mov fs, ax
  mov gs, ax
  mov ss, ax

  mov ebp, 0x90000          ; Initializing the stack position in free space
  mov esp, ebp

  call write_smiley_face

  jmp $

write_smiley_face:
  mov ah, 0x3f

  mov al, ':'
  mov [0xb8000 + 160], ax

  mov al, ')'
  mov [0xb8000 + 162], ax

  jmp $
