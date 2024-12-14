void kmain(void) {
    char *video_memory = (char *) 0xB8000;  // Indirizzo della memoria video
    video_memory[0] = 'G';                  // Scrivi 'H' come primo carattere
    video_memory[1] = 0x07;                 // Colore del testo (bianco su nero)

    // Altre operazioni del kernel
    while (1) {
        // Loop infinito per evitare il ritorno a loader
    }
}
