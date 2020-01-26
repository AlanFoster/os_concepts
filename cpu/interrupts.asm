; Full details on interrupt service routines:
; http://www.osdever.net/bkerndev/Docs/isrs.htm
; https://wiki.osdev.org/Exceptions
;
; Interrupt stack frame
; https://os.phil-opp.com/cpu-exceptions/#the-interrupt-stack-frame
;
; Note that some interrupts do not contain error codes, and some do.
; This file produces a fake error code when none is present, in order to
; have a consistent stack.

extern handle_isr
ALIGN 4


; Whether the handler has a dummy error code or not, this is the shared logic required
shared_handler_logic:
    ; 1 - Pushing the state on to the stack in preparation for calling the handler
	mov ax, ds ; Lower 16-bits of eax = ds.
	push eax ; save the data segment descriptor
	mov ax, 0x10  ; Load the kernel data segment descriptor
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax

	; push esp ; TODO: registers_t *r
    cld ; sysV ABI require clear DF

    ; 2. Invoking the fault handler, written in C
    call handle_isr

    ; 3. Cleanup + Returning
	pop eax
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	add esp, 8 ; Remove error code + interrupt number from the isr assembly handlers
	sti
    iret ; Interrupt return

; Macro syntax: name, followed by number of arguments. Access arguments via %1, %2, ..., %n
%macro real_error 3
[global isr%1]
isr%1:
    cli
    push 0xff    ; Push the current interupt handler, note that no dummy code was pushed
    jmp shared_handler_logic
%endmacro

%macro mock_error 3
[global isr%1]
isr%1:
    cli
    push byte 123    ; Push dummy error code
    push byte %1     ; Push the current interupt number

    jmp shared_handler_logic
%endmacro

; With the above macros defined, let's create our handlers.
; Note that the textual description isn't used by the macro, it's documentationo purposes,.
mock_error 0, "Division By Zero Exception", "No error code"
mock_error 1, "Debug Exception", "No error code"
mock_error 2, "Non Maskable Interrupt Exception", "No error code"
mock_error 3, "Breakpoint Exception", "No error code"
mock_error 4, "Into Detected Overflow Exception", "No error code"
mock_error 5, "Out of Bounds Exception", "No error code"
mock_error 6, "Invalid Opcode Exception", "No error code"
mock_error 7, "No Coprocessor Exception", "No error code"
real_error 8, "Double Fault Exception", "Yes error code"
mock_error 9, "Coprocessor Segment Overrun Exception", "No error code"
real_error 10, "Bad TSS Exception", "Yes error code"
real_error 11, "Segment Not Present Exception", "Yes error code"
real_error 12, "Stack Fault Exception", "Yes error code"
real_error 13, "General Protection Fault Exception", "Yes error code"
real_error 14, "Page Fault Exception", "Yes error code"
mock_error 15, "Unknown Interrupt Exception", "No error code"
mock_error 16, "Coprocessor Fault Exception", "No error code"
real_error 17, "Alignment Check Exception  486+ ", "Yes error code"
mock_error 18, "Machine Check Exception  Pentium/586+ ", "No error code"
mock_error 19, "19-31 Reserved Exceptions", "No error code"
mock_error 20, "19-31 Reserved Exceptions", "No error code"
mock_error 21, "19-31 Reserved Exceptions", "No error code"
mock_error 22, "19-31 Reserved Exceptions", "No error code"
mock_error 23, "19-31 Reserved Exceptions", "No error code"
mock_error 24, "19-31 Reserved Exceptions", "No error code"
mock_error 25, "19-31 Reserved Exceptions", "No error code"
mock_error 26, "19-31 Reserved Exceptions", "No error code"
mock_error 27, "19-31 Reserved Exceptions", "No error code"
mock_error 28, "19-31 Reserved Exceptions", "No error code"
mock_error 29, "19-31 Reserved Exceptions", "No error code"
real_error 30, "Security Exception", "Yes error code"
mock_error 31, "19-31 Reserved Exceptions", "No error code"
