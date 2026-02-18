global switch_to_task

; void switch_to_task(uint32_t *old_esp_ptr, uint32_t new_esp);
switch_to_task:
    push ebx
    push esi
    push edi
    push ebp
    
    mov edi, [esp + 20]
    mov [edi], esp

    mov esi, [esp + 24]
    mov esp, esi 

    pop ebp
    pop edi
    pop esi
    pop ebx

    ret