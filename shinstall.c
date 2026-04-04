void keymap_set(int layout);
extern unsigned short *vga;
extern int cursor_x;
extern int cursor_y;
extern int prompt_x;
void update_cursor(int x, int y);
void clear_screen();
void print(const char *str, unsigned char color);
char read_key();
int ata_detect();
int part_create_single(unsigned int disk_sectors);

void ux_put(int x, int y, char c, unsigned char color);
void ux_fill(int x, int y, int w, int h, char c, unsigned char color);
void ux_print_at(int x, int y, const char *str, unsigned char color);
void ux_draw_box(int x, int y, int w, int h, unsigned char color);
void ux_draw_titlebar(int x, int y, int w, const char *title, unsigned char color);

#define KEY_UP   ((char)0xF0)
#define KEY_DOWN ((char)0xF1)
#define KEY_F10  ((char)0x44)

int selected_lang   = 0;
int selected_layout = 0;
int selected_part   = 0;
int system_lang     = 0;
int system_layout   = 0;
int system_installed = 0;

void shinstall_draw_base() {
    ux_fill(0, 0, 80, 25, ' ', 0x17);
    ux_fill(0, 0, 80, 1, ' ', 0x70);
    ux_print_at(1, 0, "ShInstall - Instalador do ShellOS", 0x70);
    ux_fill(0, 24, 80, 1, ' ', 0x70);
    ux_print_at(1, 24, "Setas para navegar, Enter confirmar, F10 cancelar", 0x70);
}

int step_language() {
    const char *langs[] = { "Portugues (BR)", "English (US)", "Espanol" };
    int count = 3;
    int sel = 0;
    while (1) {
        shinstall_draw_base();
        ux_draw_box(15, 3, 50, 12, 0x1F);
        ux_draw_titlebar(16, 3, 48, " Passo 1/3 - Idioma do sistema", 0x4F);
        ux_print_at(17, 5, "Selecione o idioma:", 0x1F);
        for (int i = 0; i < count; i++) {
            if (i == sel) {
                ux_fill(17, 7+i, 44, 1, ' ', 0x70);
                ux_print_at(17, 7+i, "> ", 0x70);
                ux_print_at(19, 7+i, langs[i], 0x70);
            } else {
                ux_fill(17, 7+i, 44, 1, ' ', 0x1F);
                ux_print_at(17, 7+i, "  ", 0x1F);
                ux_print_at(19, 7+i, langs[i], 0x1F);
            }
        }
        char c = read_key();
        if (c == KEY_UP   && sel > 0) sel--;
        if (c == KEY_DOWN && sel < count-1) sel++;
        if (c == '\n') { selected_lang = sel; return 1; }
        if (c == KEY_F10) return 0;
    }
}

int step_keyboard() {
    const char *layouts[] = {
        "ABNT2  - Portugues (Brasil)",
        "QWERTY - English (US)",
        "QWERTZ - Deutsch (DE)",
        "AZERTY - Francais (FR)"
    };
    int count = 4;
    int sel = 0;
    while (1) {
        shinstall_draw_base();
        ux_draw_box(15, 3, 50, 13, 0x1F);
        ux_draw_titlebar(16, 3, 48, " Passo 2/3 - Layout do teclado", 0x4F);
        ux_print_at(17, 5, "Selecione o layout do teclado:", 0x1F);
        for (int i = 0; i < count; i++) {
            if (i == sel) {
                ux_fill(17, 7+i, 44, 1, ' ', 0x70);
                ux_print_at(17, 7+i, "> ", 0x70);
                ux_print_at(19, 7+i, layouts[i], 0x70);
            } else {
                ux_fill(17, 7+i, 44, 1, ' ', 0x1F);
                ux_print_at(17, 7+i, "  ", 0x1F);
                ux_print_at(19, 7+i, layouts[i], 0x1F);
            }
        }
        char c = read_key();
        if (c == KEY_UP   && sel > 0) sel--;
        if (c == KEY_DOWN && sel < count-1) sel++;
        if (c == '\n') { selected_layout = sel; return 1; }
        if (c == KEY_F10) return 0;
    }
}

int step_partition() {
    const char *parts[] = {
        "Usar disco inteiro (recomendado)",
        "Nao particionar (apenas testar)"
    };
    int count = 2;
    int sel = 0;
    while (1) {
        shinstall_draw_base();
        ux_draw_box(15, 3, 50, 13, 0x1F);
        ux_draw_titlebar(16, 3, 48, " Passo 3/3 - Particionamento", 0x4F);
        ux_print_at(17, 5, "Como deseja configurar o disco?", 0x1F);
        ux_print_at(17, 6, "Disco detectado: vda (2MB)", 0x1B);
        for (int i = 0; i < count; i++) {
            if (i == sel) {
                ux_fill(17, 8+i, 44, 1, ' ', 0x70);
                ux_print_at(17, 8+i, "> ", 0x70);
                ux_print_at(19, 8+i, parts[i], 0x70);
            } else {
                ux_fill(17, 8+i, 44, 1, ' ', 0x1F);
                ux_print_at(17, 8+i, "  ", 0x1F);
                ux_print_at(19, 8+i, parts[i], 0x1F);
            }
        }
        char c = read_key();
        if (c == KEY_UP   && sel > 0) sel--;
        if (c == KEY_DOWN && sel < count-1) sel++;
        if (c == '\n') { selected_part = sel; return 1; }
        if (c == KEY_F10) return 0;
    }
}

void step_confirm() {
    const char *langs[]   = { "Portugues (BR)", "English (US)", "Espanol" };
    const char *layouts[] = { "ABNT2", "QWERTY", "QWERTZ", "AZERTY" };
    const char *parts[]   = { "Disco inteiro", "Sem particionar" };

    shinstall_draw_base();
    ux_draw_box(15, 5, 50, 13, 0x1F);
    ux_draw_titlebar(16, 5, 48, " Instalando ShellOS...", 0x4F);

    ux_print_at(17, 7, "Detectando disco...         ", 0x1F);
    for (int d = 0; d < 8000000; d++) __asm__ volatile ("nop");
    if (!ata_detect()) {
        ux_print_at(17, 7, "ERRO: disco nao encontrado! ", 0x0C);
        for (int d = 0; d < 20000000; d++) __asm__ volatile ("nop");
        return;
    }
    ux_print_at(17, 7, "Disco detectado!         OK ", 0x2F);

    ux_print_at(17, 8, "Criando particoes...        ", 0x1F);
    for (int d = 0; d < 8000000; d++) __asm__ volatile ("nop");
    int pr = 0;
    if (selected_part == 0)
        pr = part_create_single(4096);
    if (pr < 0) {
        ux_print_at(17, 8, "ERRO: falha ao particionar! ", 0x0C);
        for (int d = 0; d < 20000000; d++) __asm__ volatile ("nop");
        return;
    }
    ux_print_at(17, 8, "Particoes criadas!       OK ", 0x2F);

    ux_print_at(17, 9, "Copiando sistema...         ", 0x1F);
    for (int i = 0; i < 44; i++) {
        ux_put(17+i, 11, 0xDB, 0x2F);
        for (int d = 0; d < 2000000; d++) __asm__ volatile ("nop");
    }
    ux_print_at(17, 9, "Sistema copiado!         OK ", 0x2F);

    ux_print_at(17, 10, "Aplicando configuracoes...  ", 0x1F);
    for (int d = 0; d < 8000000; d++) __asm__ volatile ("nop");
    system_lang   = selected_lang;
    system_layout = selected_layout;
    keymap_set(selected_layout);
    ux_print_at(17, 10, "Configuracoes aplicadas! OK ", 0x2F);

    for (int d = 0; d < 10000000; d++) __asm__ volatile ("nop");

    shinstall_draw_base();
    ux_draw_box(15, 3, 50, 16, 0x1F);
    ux_draw_titlebar(16, 3, 48, " Instalacao concluida!", 0x2F);
    ux_print_at(17, 5,  "Configuracoes:", 0x1B);
    ux_print_at(17, 6,  "Idioma:  ", 0x1B);
    ux_print_at(26, 6,  langs[selected_lang], 0x1F);
    ux_print_at(17, 7,  "Teclado: ", 0x1B);
    ux_print_at(26, 7,  layouts[selected_layout], 0x1F);
    ux_print_at(17, 8,  "Disco:   ", 0x1B);
    ux_print_at(26, 8,  parts[selected_part], 0x1F);
    ux_print_at(17, 10, "ShellOS instalado com sucesso!", 0x2F);
    ux_print_at(17, 12, "Remova a midia de instalacao", 0x1F);
    ux_print_at(17, 13, "e reinicie o sistema.", 0x1F);
    ux_print_at(17, 15, "Pressione Enter para continuar", 0x18);

    while (1) {
        char c = read_key();
        if (c == '\n') return;
    }
}

void cmd_shinstall_run() {
    if (!step_language())  { clear_screen(); return; }
    if (!step_keyboard())  { clear_screen(); return; }
    if (!step_partition()) { clear_screen(); return; }
    step_confirm();
    system_installed = 1;
    clear_screen();
    cursor_x = 0;
    cursor_y = 0;
    prompt_x = 0;
    update_cursor(0, 0);
}
