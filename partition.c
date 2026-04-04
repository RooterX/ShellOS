int ata_read(unsigned int lba, unsigned char *buf);
int ata_write(unsigned int lba, unsigned char *buf);

// entrada da tabela de particoes MBR
typedef struct {
    unsigned char  status;        // 0x80 = bootavel
    unsigned char  chs_first[3];  // CHS inicio (ignorado em LBA)
    unsigned char  type;          // tipo da particao
    unsigned char  chs_last[3];   // CHS fim (ignorado em LBA)
    unsigned int   lba_start;     // inicio em LBA
    unsigned int   lba_size;      // tamanho em setores
} __attribute__((packed)) PartEntry;

// MBR completo
typedef struct {
    unsigned char code[446];      // bootloader code
    PartEntry     parts[4];       // 4 particoes primarias
    unsigned short signature;     // 0xAA55
} __attribute__((packed)) MBR;

static unsigned char mbr_buf[512];

// le a MBR do disco
int part_read_mbr() {
    return ata_read(0, mbr_buf);
}

// escreve a MBR no disco
int part_write_mbr() {
    return ata_write(0, mbr_buf);
}

// particiona o disco inteiro com uma unica particao
int part_create_single(unsigned int disk_sectors) {
    if (part_read_mbr() < 0) return -1;

    MBR *mbr = (MBR*)mbr_buf;

    // limpa tabela de particoes
    for (int i = 0; i < 4; i++) {
        mbr->parts[i].status    = 0;
        mbr->parts[i].type      = 0;
        mbr->parts[i].lba_start = 0;
        mbr->parts[i].lba_size  = 0;
    }

    // cria particao unica bootavel
    mbr->parts[0].status    = 0x80;       // bootavel
    mbr->parts[0].type      = 0x83;       // Linux/custom
    mbr->parts[0].lba_start = 1;          // comeca no setor 1
    mbr->parts[0].lba_size  = disk_sectors - 1;

    // assinatura MBR
    mbr->signature = 0xAA55;

    return part_write_mbr();
}

// grava o kernel no disco a partir do setor 1
int part_install_kernel(unsigned char *kernel_data, unsigned int size) {
    unsigned int sectors = (size + 511) / 512;
    for (unsigned int i = 0; i < sectors; i++) {
        if (ata_write(i + 1, kernel_data + i * 512) < 0)
            return -1;
    }
    return 0;
}
