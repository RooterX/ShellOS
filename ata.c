static inline unsigned char inb(unsigned short port) {
    unsigned char val;
    __asm__ volatile ("inb %1, %0" : "=a"(val) : "Nd"(port));
    return val;
}
static inline void outb(unsigned short port, unsigned char val) {
    __asm__ volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}
static inline unsigned short inw(unsigned short port) {
    unsigned short val;
    __asm__ volatile ("inw %1, %0" : "=a"(val) : "Nd"(port));
    return val;
}
static inline void outw(unsigned short port, unsigned short val) {
    __asm__ volatile ("outw %0, %1" : : "a"(val), "Nd"(port));
}

#define ATA_DATA       0x1F0
#define ATA_FEATURES   0x1F1
#define ATA_SECT_COUNT 0x1F2
#define ATA_LBA_LOW    0x1F3
#define ATA_LBA_MID    0x1F4
#define ATA_LBA_HIGH   0x1F5
#define ATA_DRIVE      0x1F6
#define ATA_CMD        0x1F7
#define ATA_ALT_STATUS 0x3F6

static void ata_delay() {
    // le o status alternativo 4 vezes para dar tempo ao disco
    inb(ATA_ALT_STATUS);
    inb(ATA_ALT_STATUS);
    inb(ATA_ALT_STATUS);
    inb(ATA_ALT_STATUS);
}

static int ata_wait_bsy() {
    int timeout = 1000000;
    while ((inb(ATA_CMD) & 0x80) && --timeout);
    return timeout ? 0 : -1;
}

static int ata_wait_drq() {
    int timeout = 1000000;
    unsigned char status;
    do {
        status = inb(ATA_CMD);
        if (status & 0x01) return -1; // erro
        if (--timeout == 0) return -2;
    } while (!(status & 0x08)); // espera DRQ
    return 0;
}

int ata_detect() {
    outb(ATA_DRIVE, 0xA0);
    ata_delay();
    outb(ATA_SECT_COUNT, 0);
    outb(ATA_LBA_LOW, 0);
    outb(ATA_LBA_MID, 0);
    outb(ATA_LBA_HIGH, 0);
    outb(ATA_CMD, 0xEC); // IDENTIFY
    ata_delay();
    unsigned char status = inb(ATA_CMD);
    if (status == 0) return 0;
    if (ata_wait_bsy() < 0) return 0;
    // verifica se é ATA (nao ATAPI)
    if (inb(ATA_LBA_MID) || inb(ATA_LBA_HIGH)) return 0;
    if (ata_wait_drq() < 0) return 0;
    // drena os 256 words do IDENTIFY
    for (int i = 0; i < 256; i++) inw(ATA_DATA);
    return 1;
}

int ata_read(unsigned int lba, unsigned char *buf) {
    if (ata_wait_bsy() < 0) return -1;
    outb(ATA_DRIVE,      0xE0 | ((lba >> 24) & 0x0F));
    ata_delay();
    outb(ATA_FEATURES,   0x00);
    outb(ATA_SECT_COUNT, 1);
    outb(ATA_LBA_LOW,    (unsigned char)(lba));
    outb(ATA_LBA_MID,    (unsigned char)(lba >> 8));
    outb(ATA_LBA_HIGH,   (unsigned char)(lba >> 16));
    outb(ATA_CMD,        0x20); // READ SECTORS
    ata_delay();
    if (ata_wait_bsy() < 0) return -1;
    if (ata_wait_drq() < 0) return -1;
    unsigned short *ptr = (unsigned short*)buf;
    for (int i = 0; i < 256; i++)
        ptr[i] = inw(ATA_DATA);
    return 0;
}

int ata_write(unsigned int lba, unsigned char *buf) {
    if (ata_wait_bsy() < 0) return -1;
    outb(ATA_DRIVE,      0xE0 | ((lba >> 24) & 0x0F));
    ata_delay();
    outb(ATA_FEATURES,   0x00);
    outb(ATA_SECT_COUNT, 1);
    outb(ATA_LBA_LOW,    (unsigned char)(lba));
    outb(ATA_LBA_MID,    (unsigned char)(lba >> 8));
    outb(ATA_LBA_HIGH,   (unsigned char)(lba >> 16));
    outb(ATA_CMD,        0x30); // WRITE SECTORS
    ata_delay();
    if (ata_wait_bsy() < 0) return -1;
    if (ata_wait_drq() < 0) return -1;
    unsigned short *ptr = (unsigned short*)buf;
    for (int i = 0; i < 256; i++)
        outw(ATA_DATA, ptr[i]);
    // cache flush
    outb(ATA_CMD, 0xE7);
    ata_wait_bsy();
    return 0;
}
