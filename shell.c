void print(const char *str, unsigned char color);
void put_char(char c, unsigned char color);
void clear_screen();
void update_cursor(int x, int y);
char read_key();
void execute_command(char *cmd);

extern unsigned short *vga;
extern int cursor_x;
extern int cursor_y;
extern int prompt_x;

#define MAX_CMD 80

void shell_run() {
    char buffer[MAX_CMD];
    int buf_pos = 0;

    while (1) {
        char c = read_key();

        if (c == '\n') {
    buffer[buf_pos] = '\0';
    cursor_x = 0;
    cursor_y++;
    if (buf_pos > 0)
        execute_command(buffer);
    // limpar linha atual antes do prompt
    for (int i = 0; i < 80; i++)
        vga[cursor_y * 80 + cursor_x + i] = (0x0F << 8) | ' ';
    // garantir que cursor esta no inicio da linha
    cursor_x = 0;
    print("shellOS> ", 0x0A);
    prompt_x = 9;
    buf_pos = 0;
} 
else if (c == '\b') {
            if (cursor_x > prompt_x && buf_pos > 0) {
                cursor_x--;
                buf_pos--;
                vga[cursor_y * 80 + cursor_x] = (0x0F << 8) | ' ';
            }

        } else if (c != 0 && buf_pos < MAX_CMD - 1) {
            buffer[buf_pos++] = c;
            put_char(c, 0x0F);
        }

        update_cursor(cursor_x, cursor_y);
    }
}
