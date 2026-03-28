void print(const char *str, unsigned char color);
void put_char(char c, unsigned char color);
void clear_screen();
void update_cursor(int x, int y);
char read_key();
void fs_strcpy(char *dst, const char *src);

extern unsigned short *vga;
extern int cursor_x;
extern int cursor_y;
extern int prompt_x;

typedef struct {
    char name[32];
    char data[4096];
    int size;
    int used;
} File;

File* fs_open(const char *dirname, const char *filename);
int fs_write_data(File *f, const char *data, int len);

void write_status_bar(const char *filename, int saved) {
    int ox = cursor_x, oy = cursor_y;
    cursor_x = 0; cursor_y = 0;
    for (int i = 0; i < 80; i++)
        vga[i] = (0x1F << 8) | ' ';
    cursor_x = 0; cursor_y = 0;
    print("  WRITE | ", 0x1F);
    print(filename, 0x1F);
    if (saved) print("  [salvo] ", 0x1F);
    else print("  [editando] ", 0x1F);
    print("  F2 salvar  F10 sair", 0x1F);
    cursor_x = ox;
    cursor_y = oy;
    update_cursor(cursor_x, cursor_y);
}

void cmd_write(char *args) {
    if (!args) { print("uso: write <dir>/<arquivo>\n", 0x0C); return; }

    char dirname[32] = {0};
    char filename[32] = {0};
    int slash = -1;

    for (int i = 0; args[i]; i++) {
        if (args[i] == '/') { slash = i; break; }
    }

    if (slash == -1) {
        fs_strcpy(dirname, "file");
        fs_strcpy(filename, args);
    } else {
        for (int i = 0; i < slash; i++) dirname[i] = args[i];
        for (int i = 0; args[slash+1+i]; i++) filename[i] = args[slash+1+i];
    }

    File *f = fs_open(dirname, filename);
    if (!f) { print("erro: diretorio nao encontrado\n", 0x0C); return; }

    char buffer[4096];
    int buf_pos = 0;

    // carregar conteudo existente
    for (int i = 0; i < f->size && i < 4095; i++)
        buffer[buf_pos++] = f->data[i];

    clear_screen();
    write_status_bar(filename, 0);

    cursor_x = 0;
    cursor_y = 1;
    update_cursor(cursor_x, cursor_y);

    // reimprimir conteudo existente
    for (int i = 0; i < buf_pos; i++)
        put_char(buffer[i], 0x0F);

    while (1) {
        char c = read_key();

// F2 = salvar (scancode 0x3C)
if (c == (char)0x3C) {
    f->size = 0;
    fs_write_data(f, buffer, buf_pos);
    write_status_bar(filename, 1);
    continue;
}

if (c == (char)0x44) {
    clear_screen();
    cursor_x = 0;
    cursor_y = 0;
    // NÃO imprime shellOS> aqui, o shell.c já faz isso, boa noticia ;)
    prompt_x = 0;
    update_cursor(cursor_x, cursor_y);
    return;
}

        if (c == '\b' && buf_pos > 0) {
            buf_pos--;
            if (cursor_x > 0) cursor_x--;
            else if (cursor_y > 1) { cursor_y--; cursor_x = 79; }
            vga[cursor_y * 80 + cursor_x] = (0x0F << 8) | ' ';
        } else if (c == '\n') {
            if (buf_pos < 4095) buffer[buf_pos++] = '\n';
            cursor_x = 0;
            cursor_y++;
        } else if (c != 0 && buf_pos < 4095) {
            buffer[buf_pos++] = c;
            put_char(c, 0x0F);
        }

        write_status_bar(filename, 0);
        update_cursor(cursor_x, cursor_y);
    }
}
