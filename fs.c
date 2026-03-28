#define MAX_FILES 32
#define MAX_DIRS 16
#define MAX_FILENAME 32
#define MAX_FILESIZE 4096

typedef struct {
    char name[MAX_FILENAME];
    char data[MAX_FILESIZE];
    int size;
    int used;
} File;

typedef struct {
    char name[MAX_FILENAME];
    File files[MAX_FILES];
    int file_count;
    int used;
} Dir;

static Dir dirs[MAX_DIRS];
static int dir_count = 0;

// utilitario de string
int fs_strcmp(const char *a, const char *b) {
    while (*a && *b && *a == *b) { a++; b++; }
    return *a - *b;
}

void fs_strcpy(char *dst, const char *src) {
    while ((*dst++ = *src++));
}

int fs_strlen(const char *s) {
    int i = 0;
    while (s[i]) i++;
    return i;
}

// inicializa o FS com diretorios padrão
void fs_init() {
    // criar /sys e /file automaticamente
    fs_strcpy(dirs[0].name, "sys");
    dirs[0].used = 1;
    dirs[0].file_count = 0;

    fs_strcpy(dirs[1].name, "file");
    dirs[1].used = 1;
    dirs[1].file_count = 0;

    dir_count = 2;
}

// criar diretorio
int fs_mkdir(const char *name) {
    if (dir_count >= MAX_DIRS) return -1;
    for (int i = 0; i < dir_count; i++)
        if (dirs[i].used && fs_strcmp(dirs[i].name, name) == 0)
            return -2; // ja existe
    fs_strcpy(dirs[dir_count].name, name);
    dirs[dir_count].used = 1;
    dirs[dir_count].file_count = 0;
    dir_count++;
    return 0;
}

// listar diretorios e arquivos
void fs_ls(void (*print_fn)(const char*, unsigned char)) {
    for (int i = 0; i < dir_count; i++) {
        if (!dirs[i].used) continue;
        print_fn("[dir] ", 0x0B);
        print_fn(dirs[i].name, 0x0B);
        print_fn("\n", 0x0F);
        for (int j = 0; j < dirs[i].file_count; j++) {
            if (!dirs[i].files[j].used) continue;
            print_fn("  [file] ", 0x0E);
            print_fn(dirs[i].files[j].name, 0x0E);
            print_fn("\n", 0x0F);
        }
    }
}

// criar/abrir arquivo em um diretorio
File* fs_open(const char *dirname, const char *filename) {
    for (int i = 0; i < dir_count; i++) {
        if (!dirs[i].used) continue;
        if (fs_strcmp(dirs[i].name, dirname) != 0) continue;
        // procura arquivo existente
        for (int j = 0; j < dirs[i].file_count; j++) {
            if (dirs[i].files[j].used &&
                fs_strcmp(dirs[i].files[j].name, filename) == 0)
                return &dirs[i].files[j];
        }
        // cria novo
        if (dirs[i].file_count >= MAX_FILES) return 0;
        File *f = &dirs[i].files[dirs[i].file_count++];
        fs_strcpy(f->name, filename);
        f->size = 0;
        f->used = 1;
        return f;
    }
    return 0;
}

// escrever no arquivo
int fs_write(File *f, const char *data, int len) {
    if (!f || f->size + len >= MAX_FILESIZE) return -1;
    for (int i = 0; i < len; i++)
        f->data[f->size++] = data[i];
    f->data[f->size] = '\0';
    return 0;
}

// ler arquivo
const char* fs_read(File *f) {
    if (!f) return 0;
    return f->data;
}

// deletar arquivo
int fs_rm(const char *dirname, const char *filename) {
    for (int i = 0; i < dir_count; i++) {
        if (!dirs[i].used) continue;
        if (fs_strcmp(dirs[i].name, dirname) != 0) continue;
        for (int j = 0; j < dirs[i].file_count; j++) {
            if (dirs[i].files[j].used &&
                fs_strcmp(dirs[i].files[j].name, filename) == 0) {
                dirs[i].files[j].used = 0;
                return 0;
            }
        }
    }
    return -1;
}
