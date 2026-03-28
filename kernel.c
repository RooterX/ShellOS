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
        if (cursor_y >= 25) scroll();
        return;
    }
    if (cursor_y >= 25) scroll();
    vga[cursor_y * 80 + cursor_x] = (color << 8) | c;
    cursor_x++;
    if (cursor_x >= 80) {
        cursor_x = 0;
        cursor_y++;
        if (cursor_y >= 25) scroll();
    }
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

char read_key() {
    static char scancode_map[] = {
        0, 0, '1','2','3','4','5','6','7','8','9','0','-','=','\b',
        0, 'q','w','e','r','t','y','u','i','o','p','[',']','\n',
        0, 'a','s','d','f','g','h','j','k','l',';','\'','`',
        0,'\\','z','x','c','v','b','n','m',',','.','/',0,
        '*',0,' '
    };
    unsigned char sc;
    do {
        while (!(inb(0x64) & 0x01));
        sc = inb(0x60);
    } while (sc & 0x80);
    if (sc < sizeof(scancode_map))
        return scancode_map[sc];
    return 0;
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
