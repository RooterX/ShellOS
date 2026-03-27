void print(const char *str, unsigned char color);
void clear_screen();
extern int cursor_x;
extern int cursor_y;
extern int prompt_x;

int str_cmp(const char *a, const char *b) {
    while (*a && *b && *a == *b) { a++; b++; }
    return *a - *b;
}

void cmd_help() {
    print("comandos disponiveis:\n", 0x0B);
    print("  help   - lista comandos\n", 0x0F);
    print("  clear  - limpa a tela\n", 0x0F);
    print("  echo   - imprime texto\n", 0x0F);
    print("  halt   - desliga o sistema\n", 0x0F);
    print("  mem    - info de memoria\n", 0x0F);
    print("  ver    - versao do shellOS\n", 0x0F);
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
    outw(0x604, 0x2000);   // QEMU/Bochs
    outw(0x4004, 0x3400);  // VirtualBox
    outw(0xB004, 0x2000);  // VMware/outros
    __asm__ volatile ("cli; hlt");
}

void cmd_mem() {
    print("memoria VGA:  0xB8000 - 0xB8FA0\n", 0x0F);
    print("kernel:       0x7E00\n", 0x0F);
    print("stack:        0x90000\n", 0x0F);
}

void cmd_ver() {
    print("shellOS v0.1 - feito do zero\n", 0x0A);
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
    else {
        print(cmd, 0x0C);
        print(": comando nao encontrado\n", 0x0C);
    }
}
