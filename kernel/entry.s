global enter_user_mode

; -----------------------------------------------------------------------------
; void enter_user_mode(uint32_t entry_point, uint32_t user_stack);
; -----------------------------------------------------------------------------
enter_user_mode:
    mov ecx, [esp + 4]   ; EIP (Where we want to jump in User Mode)
    mov edx, [esp + 8]   ; ESP (The stack that we will use)

    ; 0x20 | 0x3 = 0x23
    mov ax, 0x23
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    ; IRET swtack frame:
    ; SS, ESP, EFLAGS, CS, EIP
    
    ; Push SS (Stack Segment)
    push 0x23

    ; ESP (Stack Pointer)
    push edx            

    ; EFLAGS
    pushf               
    pop eax             
    or eax, 0x200
    push eax

    ; CS (Code Segment)
    ; 0x18 | 0x3 = 0x1B
    push 0x1B           

    ; EIP (Instruction Pointer)
    push ecx

    iret