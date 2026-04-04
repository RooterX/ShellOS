void fs_init();
void shell_run();
void clear_screen();
void print(const char *str, unsigned char color);
void update_cursor(int x, int y);

unsigned short *vga = (unsigned short*)0xB8000;
int cursor_x = 0;
int cursor_y = 0;
int prompt_x = 0;

static inline unsigned char inb(unsigned short port) {
    unsigned char val;
    __asm__ volatile ("inb %1, %0" : "=a"(val) : "Nd"(port));
    return val;
}

static inline void outb(unsigned short port, unsigned char val) {
    __asm__ volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

void update_cursor(int x, int y) {
    unsigned short pos = y * 80 + x;
    outb(0x3D4, 0x0F);
    outb(0x3D5, (unsigned char)(pos & 0xFF));
    outb(0x3D4, 0x0E);
    outb(0x3D5, (unsigned char)((pos >> 8) & 0xFF));
}

void scroll() {
    for (int i = 0; i < 24 * 80; i++)
        vga[i] = vga[i + 80];
    for (int i = 24 * 80; i < 25 * 80; i++)
        vga[i] = 0x0720;
    cursor_y = 24;
}

void put_char(char c, unsigned char color) {
    if (c == '\n') {
        cursor_x = 0;
        cursor_y++;
    } else {
        vga[cursor_y * 80 + cursor_x] = (color << 8) | (unsigned char)c;
        cursor_x++;
    }
    if (cursor_x >= 80) { cursor_x = 0; cursor_y++; }
    if (cursor_y >= 25) scroll();
}

void print(const char *str, unsigned char color) {
    for (int i = 0; str[i]; i++)
        put_char(str[i], color);
}

void clear_screen() {
    for (int j = 0; j < 80 * 25; j++)
        vga[j] = 0x0720;
    cursor_x = 0;
    cursor_y = 0;
}

// no topo do kernel.c adiciona
char keymap_get(unsigned char sc, int shift);

char read_key() {
    unsigned char sc;
    static int shift = 0;
    static int ctrl  = 0;  // adiciona ctrl
    while (1) {
        while (!(inb(0x64) & 0x01));
        sc = inb(0x60);

        if (sc == 0x2A || sc == 0x36) { shift = 1; continue; }
        if (sc == 0xAA || sc == 0xB6) { shift = 0; continue; }

        // ctrl pressionado/solto
        if (sc == 0x1D) { ctrl = 1; continue; }
        if (sc == 0x9D) { ctrl = 0; continue; }

        if (sc == 0xE0) {
            while (!(inb(0x64) & 0x01));
            unsigned char sc2 = inb(0x60);
            if (sc2 == 0x48) return (char)0xF0;
            if (sc2 == 0x50) return (char)0xF1;
            if (sc2 == 0x4B) return (char)0xF2;
            if (sc2 == 0x4D) return (char)0xF3;
            continue;
        }

        if (sc & 0x80) continue;

        if (sc >= 0x3B && sc <= 0x44) return (char)sc;
        if (sc == 0x57) return (char)0x57;
        if (sc == 0x58) return (char)0x58;

        // ctrl + tecla — retorna codigo de controle
        if (ctrl) {
            char k = keymap_get(sc, 0);
            if (k >= 'a' && k <= 'z') return k - 'a' + 1; // Ctrl+A=1, Ctrl+C=3...
            if (k >= 'A' && k <= 'Z') return k - 'A' + 1;
            continue;
        }

        return keymap_get(sc, shift);
    }
}

__attribute__((section(".text")))
void _start() {
    clear_screen();
    fs_init();
    print("shellOS> ", 0x0A);
    prompt_x = 9;
    update_cursor(cursor_x, cursor_y);
    shell_run();
}
