extern unsigned short *vga;
extern int cursor_x;
extern int cursor_y;
extern int prompt_x;
void update_cursor(int x, int y);
void clear_screen();
void print(const char *str, unsigned char color);
char read_key();

// escreve um caractere em posicao absoluta na VGA
void ux_put(int x, int y, char c, unsigned char color) {
    if (x < 0 || x >= 80 || y < 0 || y >= 25) return;
    vga[y * 80 + x] = (color << 8) | (unsigned char)c;
}

// preenche area com um caractere
void ux_fill(int x, int y, int w, int h, char c, unsigned char color) {
    for (int j = y; j < y + h; j++)
        for (int i = x; i < x + w; i++)
            ux_put(i, j, c, color);
}

// escreve string em posicao absoluta
void ux_print_at(int x, int y, const char *str, unsigned char color) {
    for (int i = 0; str[i]; i++)
        ux_put(x + i, y, str[i], color);
}

// desenha borda de janela
void ux_draw_box(int x, int y, int w, int h, unsigned char color) {
    // cantos
    ux_put(x,         y,         0xC9, color); // ╔
    ux_put(x + w - 1, y,         0xBB, color); // ╗
    ux_put(x,         y + h - 1, 0xC8, color); // ╚
    ux_put(x + w - 1, y + h - 1, 0xBC, color); // ╝

    // bordas horizontais
    for (int i = x + 1; i < x + w - 1; i++) {
        ux_put(i, y,         0xCD, color); // ═
        ux_put(i, y + h - 1, 0xCD, color); // ═
    }

    // bordas verticais
    for (int j = y + 1; j < y + h - 1; j++) {
        ux_put(x,         j, 0xBA, color); // ║
        ux_put(x + w - 1, j, 0xBA, color); // ║
    }

    // interior
    ux_fill(x + 1, y + 1, w - 2, h - 2, ' ', color);
}

// barra de titulo da janela
void ux_draw_titlebar(int x, int y, int w, const char *title, unsigned char color) {
    ux_fill(x, y, w, 1, ' ', color);
    ux_print_at(x + 1, y, title, color);
    // botao fechar
    ux_put(x + w - 2, y, 'X', color);
}

// desenha o desktop completo
void ux_draw_desktop() {
    // fundo azul
    ux_fill(0, 0, 80, 25, ' ', 0x17);

    // barra de status no topo
    ux_fill(0, 0, 80, 1, ' ', 0x70);
    ux_print_at(1, 0, "ShellOS MakeUX", 0x70);
    ux_print_at(60, 0, "", 0x70);

    // barra de tarefas embaixo
    ux_fill(0, 24, 80, 1, ' ', 0x70);
    ux_print_at(1, 24, "[Shell]  [Arquivos]  [Memoria]", 0x70);

    // janela principal
    ux_draw_box(10, 3, 40, 14, 0x1F);
    ux_draw_titlebar(11, 3, 38, " Bem vindo ao ShellOS!", 0x4F);

    // conteudo da janela
    ux_print_at(12, 5,  "Sistema: ShellOS v0.1",        0x1F);
    ux_print_at(12, 6,  "Kernel:  BASIX",               0x1F);
    ux_print_at(12, 7,  "Shell:   shellOS terminal",       0x1F);
    ux_print_at(12, 8,  "Memoria: 0x100000-0x400000",   0x1F);
    ux_print_at(12, 9,  "VGA:     80x25 text mode",     0x1F);
    ux_print_at(12, 11, "Linguagens:",                  0x1B);
    ux_print_at(12, 12, "  Assembly + C + Rust",        0x1F);
    ux_print_at(12, 14, "",      0x18);

 // janela secundaria
    ux_draw_box(54, 5, 24, 12, 0x2F);
    ux_draw_titlebar(55, 5, 22, " Atalhos", 0x2F);
    ux_print_at(56, 7,  "F1  - abrir shell",  0x2F);
    ux_print_at(56, 8,  "F2  - arquivos",     0x2F);
    ux_print_at(56, 9,  "F3  - memoria",      0x2F);
    ux_print_at(56, 10, "F10 - sair",         0x2F);
    ux_print_at(56, 11, "F12 - desligar",     0x2F);

    update_cursor(0, 24);
}

void cmd_makeux() {
    ux_draw_desktop();

    while (1) {
        char c = read_key();

        // F1 = voltar ao shell
        if (c == (char)0x3B) {
            clear_screen();
            cursor_x = 0; cursor_y = 0; prompt_x = 0;
            update_cursor(0, 0);
            return;
        }

        // F2 = ls — mostra arquivos numa janela
        if (c == (char)0x3C) {
            ux_fill(10, 3, 40, 14, ' ', 0x1F);
            ux_draw_box(10, 3, 40, 14, 0x1F);
            ux_draw_titlebar(11, 3, 38, " Arquivos", 0x4F);
            // chama fs_ls mas precisa printar na janela
            ux_print_at(12, 5, "sys/", 0x1B);
            ux_print_at(12, 6, "file/", 0x1B);
            ux_print_at(12, 12, "", 0x18);
            continue;
        }

        // F3 = mem
        if (c == (char)0x3D) {
            ux_fill(10, 3, 40, 14, ' ', 0x1F);
            ux_draw_box(10, 3, 40, 14, 0x1F);
            ux_draw_titlebar(11, 3, 38, " Memoria", 0x4F);
            ux_print_at(12, 5, "heap:   0x100000-0x400000", 0x1F);
            ux_print_at(12, 6, "kernel: 0x7E00",            0x1F);
            ux_print_at(12, 7, "stack:  0x90000",           0x1F);
            ux_print_at(12, 8, "modulo: memman.rs (Rust)",  0x1A);
            ux_print_at(12, 12, "", 0x18);
            continue;
        }

        // F10 = sair
        if (c == (char)0x44) {
            clear_screen();
            cursor_x = 0; cursor_y = 0; prompt_x = 0;
            update_cursor(0, 0);
            return;
        }

        // F12 = halt
        if (c == (char)0x58) {
            print("\ndesligando...\n", 0x0C);
            for (int i = 0; i < 50000000; i++)
                __asm__ volatile ("nop");
            __asm__ volatile ("outw %0, %1" : : "a"((short)0x2000), "Nd"((short)0x604));
            __asm__ volatile ("outw %0, %1" : : "a"((short)0x3400), "Nd"((short)0x4004));
            __asm__ volatile ("cli; hlt");
        }
    }
}
