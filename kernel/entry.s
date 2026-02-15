global enter_user_mode

; -----------------------------------------------------------------------------
; void enter_user_mode(uint32_t entry_point, uint32_t user_stack);
; -----------------------------------------------------------------------------
enter_user_mode:
    mov ecx, [esp + 4]   ; EIP (Where we want to jump in User Mode)
    mov edx, [esp + 8]   ; ESP (The stack that we will use)

    cli

    ; SS (User Data Segment)
    mov ax, 0x23        ; 0x20 (GDT index 4) | 0x3 (RPL 3)
    push eax

    ; ESP (User Stack Pointer)
    push edx

    ; EFLAGS
    pushf
    pop eax
    or eax, 0x200
    push eax

    ; CS (User Code Segment)
    mov ax, 0x1b        ; 0x18 (GDT index 3) | 0x3 (RPL 3)
    push eax

    ; EIP (Instruction Pointer)
    push ecx

    mov ax, 0x23
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    iret