void cmd_shinstall_run();
extern int system_installed;
char read_key();
extern int system_installed;
void cmd_makeux();
void cmd_write(char *args);
void print(const char *str, unsigned char color);
void clear_screen();
typedef struct {
    char name[32];
    char data[4096];
    int size;
    int used;
} File;

File* fs_find(const char *dirname, const char *filename);
extern int cursor_x;
extern int cursor_y;
extern int prompt_x;

// declaracoes do fs.c
int fs_mkdir(const char *name);
void fs_ls(void (*print_fn)(const char*, unsigned char));
int fs_rm(const char *dirname, const char *filename);

int str_cmp(const char *a, const char *b) {
    while (*a && *b && *a == *b) { a++; b++; }
    return *a - *b;
}

void cmd_help() {
    print("comandos disponiveis:\n", 0x0B);
    print("  help          - lista comandos\n", 0x0F);
    print("  clear         - limpa a tela\n", 0x0F);
    print("  echo          - imprime texto\n", 0x0F);
    print("  halt          - desliga o sistema\n", 0x0F);
    print("  mem           - info de memoria\n", 0x0F);
    print("  ver           - versao do shellOS\n", 0x0F);
    print("  mkdir <nome>  - cria diretorio\n", 0x0F);
    print("  ls            - lista arquivos\n", 0x0F);
    print("  rm <dir/arq>  - remove arquivo\n", 0x0F);
    print("  write <arq>   - editor de texto\n", 0x0F);
    print("  makeux        - interface grafica\n", 0x0F);
    print("  shinstall     - instalar o shellOS\n", 0x0F);
}

void cmd_echo(char *args) {
    if (args) {
        print(args, 0x0F);
        print("\n", 0x0F);
    }
}

static inline void outw(unsigned short port, unsigned short val) {
    __asm__ volatile ("outw %0, %1" : : "a"(val), "Nd"(port));
}

void cmd_halt() {
    print("\ndesligando...\n", 0x0C);
    for (int i = 0; i < 50000000; i++)
        __asm__ volatile ("nop");
    outw(0x604, 0x2000);
    outw(0x4004, 0x3400);
    outw(0xB004, 0x2000);
    __asm__ volatile ("cli; hlt");
}

extern unsigned int mem_used();
extern unsigned int mem_free_space();

void cmd_mem() {
    print("--- memoria (Basix) ---\n", 0x0B);
    print("heap:   0x100000 - 0x400000\n", 0x0F);
    print("kernel: 0x7E00\n", 0x0F);
    print("stack:  0x90000\n", 0x0F);
    print("modulo: memman.rs (Rust)\n", 0x0A);
}

void cmd_shinstall() {
    if (system_installed) {
        print("erro: ShellOS ja esta instalado!\n", 0x0C);
        print("reinicie para reinstalar.\n", 0x0C);
        return;
    }
    cmd_shinstall_run();
}

void cmd_ver() {
    print("shellOS v0.1\n", 0x0F);
    print("kernel: Basix (Assembly + C + Rust)\n", 0x0F);
}

void cmd_mkdir(char *args) {
    if (!args) { print("uso: mkdir <nome>\n", 0x0C); return; }
    int r = fs_mkdir(args);
    if (r == -2) print("erro: diretorio ja existe\n", 0x0C);
    else if (r == -1) print("erro: limite atingido\n", 0x0C);
    else { print("criado: ", 0x0A); print(args, 0x0A); print("\n", 0x0F); }
}

void cmd_ls() {
    fs_ls(print);
}

void cmd_rm(char *args) {
    if (!args) { print("uso: rm <dir>/<arquivo>\n", 0x0C); return; }
    char *slash = 0;
    for (int i = 0; args[i]; i++) {
        if (args[i] == '/') { args[i] = '\0'; slash = &args[i+1]; break; }
    }
    if (!slash) { print("uso: rm <dir>/<arquivo>\n", 0x0C); return; }
    int r = fs_rm(args, slash);
    if (r == -1) print("arquivo nao encontrado\n", 0x0C);
    else print("removido\n", 0x0A);
}

void execute_command(char *cmd) {
    char *args = 0;
    for (int i = 0; cmd[i]; i++) {
        if (cmd[i] == ' ') {
            cmd[i] = '\0';
            args = &cmd[i + 1];
            break;
        }
    }

    if      (str_cmp(cmd, "help")  == 0) cmd_help();
    else if (str_cmp(cmd, "clear") == 0) {
        clear_screen();
        cursor_x = 0;
        cursor_y = 0;
        return;
    }
    else if (str_cmp(cmd, "echo")  == 0) cmd_echo(args);
    else if (str_cmp(cmd, "halt")  == 0) cmd_halt();
    else if (str_cmp(cmd, "mem")   == 0) cmd_mem();
    else if (str_cmp(cmd, "ver")   == 0) cmd_ver();
    else if (str_cmp(cmd, "mkdir") == 0) cmd_mkdir(args);
    else if (str_cmp(cmd, "ls")    == 0) cmd_ls();
    else if (str_cmp(cmd, "rm")    == 0) cmd_rm(args);
    else if (str_cmp(cmd, "makeux") == 0) cmd_makeux();
    else if (str_cmp(cmd, "shinstall") == 0) cmd_shinstall();
else if (str_cmp(cmd, "write") == 0) cmd_write(args);
    else {
        // tenta executar como arquivo em /file
        File *f = fs_find("file", cmd);
        if (f && f->size > 0) {
            char line[80];
            int lp = 0;
            for (int i = 0; i <= f->size; i++) {
                char ch = (i < f->size) ? f->data[i] : '\n';
                if (ch == '\n' || ch == '\0') {
                    line[lp] = '\0';
                    if (lp > 0) execute_command(line);
                    lp = 0;
                } else if (lp < 79) {
                    line[lp++] = ch;
                }
            }
        } else {
            print(cmd, 0x0C);
            print(": comando nao encontrado\n", 0x0C);
        }
    }
}

void cmd_keytest() {
    print("pressione uma tecla (F10 pra sair):\n", 0x0F);
    while (1) {
        char c = read_key();
        if (c == (char)0x44) return; // F10 sai
        // imprime o valor hex da tecla
        unsigned char v = (unsigned char)c;
        char hex[5];
        hex[0] = '0';
        hex[1] = 'x';
        hex[2] = "0123456789ABCDEF"[v >> 4];
        hex[3] = "0123456789ABCDEF"[v & 0xF];
        hex[4] = 0;
        print(hex, 0x0E);
        print(" ", 0x0F);
    }
}

int ata_detect();
int ata_write(unsigned int lba, unsigned char *buf);
int ata_read(unsigned int lba, unsigned char *buf);

void cmd_atatest() {
    print("detectando disco ATA...\n", 0x0F);
    if (!ata_detect()) {
        print("erro: disco nao encontrado!\n", 0x0C);
        return;
    }
    print("disco encontrado!\n", 0x0A);

    // testa escrita/leitura no setor 100 (longe do kernel)
    unsigned char buf[512];
    for (int i = 0; i < 512; i++) buf[i] = 0xAB;

    if (ata_write(100, buf) < 0) {
        print("erro: falha na escrita!\n", 0x0C);
        return;
    }
    print("escrita ok!\n", 0x0A);

    // le de volta e verifica
    unsigned char buf2[512];
    if (ata_read(100, buf2) < 0) {
        print("erro: falha na leitura!\n", 0x0C);
        return;
    }

    if (buf2[0] == 0xAB && buf2[511] == 0xAB)
        print("leitura ok! ATA funcionando!\n", 0x0A);
    else
        print("erro: dados corrompidos!\n", 0x0C);
}