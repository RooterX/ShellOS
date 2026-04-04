// layouts disponiveis
#define KEYMAP_ABNT2  0
#define KEYMAP_US     1
#define KEYMAP_DE     2
#define KEYMAP_FR     3

// layout atual (alterado pelo shinstall)
int current_keymap = KEYMAP_US;

// ── QWERTY US ────────────────────────────────────────────────
static char map_us[] = {
    0, 0, '1','2','3','4','5','6','7','8','9','0','-','=','\b',
    0, 'q','w','e','r','t','y','u','i','o','p','[',']','\n',
    0, 'a','s','d','f','g','h','j','k','l',';','\'','`',
    0,'\\','z','x','c','v','b','n','m',',','.','/',0,
    '*',0,' '
};
static char map_us_shift[] = {
    0, 0, '!','@','#','$','%','^','&','*','(',')','_','+','\b',
    0, 'Q','W','E','R','T','Y','U','I','O','P','{','}','\n',
    0, 'A','S','D','F','G','H','J','K','L',':','"','~',
    0,'|','Z','X','C','V','B','N','M','<','>','?',0,
    '*',0,' '
};

// ── ABNT2 pt-BR ───────────────────────────────────────────────
static char map_abnt2[] = {
    0, 0, '1','2','3','4','5','6','7','8','9','0','-','=','\b',
    0, 'q','w','e','r','t','y','u','i','o','p','`','[','\n',
    0, 'a','s','d','f','g','h','j','k','l',';','~','\'',
    0,']','z','x','c','v','b','n','m',',','.','/',0,
    '*',0,' '
};
static char map_abnt2_shift[] = {
    0, 0, '!','@','#','$','%','^','&','*','(',')','_','+','\b',
    0, 'Q','W','E','R','T','Y','U','I','O','P','`','{','\n',
    0, 'A','S','D','F','G','H','J','K','L',':','^','"',
    0,'}','Z','X','C','V','B','N','M','<','>','?',0,
    '*',0,' '
};

// ── QWERTZ DE ────────────────────────────────────────────────
static char map_de[] = {
    0, 0, '1','2','3','4','5','6','7','8','9','0','-','=','\b',
    0, 'q','w','e','r','t','z','u','i','o','p','u','a','\n',
    0, 'a','s','d','f','g','h','j','k','l','o','a','#',
    0,'<','y','x','c','v','b','n','m',',','.','-',0,
    '*',0,' '
};
static char map_de_shift[] = {
    0, 0, '!','"','#','$','%','&','/','(',')','=','?','`','\b',
    0, 'Q','W','E','R','T','Z','U','I','O','P','U','A','\n',
    0, 'A','S','D','F','G','H','J','K','L','O','A','\'',
    0,'>','Y','X','C','V','B','N','M',';',':','_',0,
    '*',0,' '
};

// ── AZERTY FR ────────────────────────────────────────────────
static char map_fr[] = {
    0, 0, '&','e','\"','\'','(','u','e','!','c','a',')','=','\b',
    0, 'a','z','e','r','t','y','u','i','o','p','^','$','\n',
    0, 'q','s','d','f','g','h','j','k','l','m','u','`',
    0,'*','w','x','c','v','b','n',',',';',':','!',0,
    '*',0,' '
};
static char map_fr_shift[] = {
    0, 0, '1','2','3','4','5','6','7','8','9','0',')','+','\b',
    0, 'A','Z','E','R','T','Y','U','I','O','P','"','L','\n',
    0, 'Q','S','D','F','G','H','J','K','L','M','U','>',
    0,'<','W','X','C','V','B','N','?','.','/','-',0,
    '*',0,' '
};

// retorna o caractere correto baseado no layout atual e shift
char keymap_get(unsigned char sc, int shift) {
    char *map      = map_us;
    char *map_sh   = map_us_shift;
    int   map_size = sizeof(map_us);

    if (current_keymap == KEYMAP_ABNT2) {
        map = map_abnt2; map_sh = map_abnt2_shift; map_size = sizeof(map_abnt2);
    } else if (current_keymap == KEYMAP_DE) {
        map = map_de; map_sh = map_de_shift; map_size = sizeof(map_de);
    } else if (current_keymap == KEYMAP_FR) {
        map = map_fr; map_sh = map_fr_shift; map_size = sizeof(map_fr);
    }

    if (sc >= map_size) return 0;
    return shift ? map_sh[sc] : map[sc];
}

void keymap_set(int layout) {
    current_keymap = layout;
}
