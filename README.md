<p align="center">
  <img src="assets/ShellLogo.png" width="120"/>
</p>

<h1 align="center">ShellOS</h1>

<p align="center">
  Um sistema operacional minimalista feito do zero em Assembly e C,<br>
  com módulos em Rust e shell interativo próprio.
</p>

<p align="center">
  <img src="https://img.shields.io/badge/licença-MIT-green"/>
  <img src="https://img.shields.io/badge/linguagens-Assembly%20%7C%20C%20%7C%20Rust-blue"/>
  <img src="https://img.shields.io/badge/testado-QEMU-orange"/>
  <img src="https://img.shields.io/badge/status-em%20desenvolvimento-yellow"/>
</p>

---

## Sobre

**ShellOS** é um sistema operacional básico construído do zero com fins educacionais.
Possui bootloader próprio, kernel chamado **Basix**, shell interativo e sistema de
arquivos em memória RAM.

<p align="center">
  <img src="assets/BasixLogo.png" width="80"/>
  <br>
  <i>Basix — o kernel do ShellOS</i>
</p>

---

## Estrutura do projeto

| Arquivo | Descrição |
|---------|-----------|
| `bootloader.asm` | Bootloader em Assembly x86 — inicializa o sistema e carrega o kernel |
| `entry.asm` | Entry point do kernel — ponto de entrada em 32 bits |
| `kernel.c` | **Basix** — driver VGA, teclado PS/2, scroll, inicialização |
| `shell.c` | Loop do shell, leitura de input e parser de comandos |
| `commands.c` | Implementação de todos os comandos do shell |
| `fs.c` | Sistema de arquivos em RAM — diretórios e arquivos voláteis |
| `linker.ld` | Script de linkagem — define layout do kernel na memória |
| `Makefile` | Compilação, empacotamento e execução |
| `memman/` | Módulo de gerenciamento de memória escrito em **Rust** |

---

## Comandos disponíveis

| Comando | Descrição |
|---------|-----------|
| `help` | Lista todos os comandos |
| `clear` | Limpa a tela |
| `echo <texto>` | Imprime texto na tela |
| `ls` | Lista diretórios e arquivos |
| `mkdir <nome>` | Cria um diretório |
| `rm <dir>/<arq>` | Remove um arquivo |
| `mem` | Informações de memória do kernel |
| `ver` | Versão do ShellOS |
| `halt` | Desliga o sistema |

---

## Requisitos
```bash
sudo apt install nasm gcc make qemu-system-x86 xorriso
```

---

## Compilar e rodar
```bash
# compilar
make

# testar no QEMU (ambiente recomendado)
make run

# gerar pacotes (.img .vdi .iso)
make package
```

---

## Compatibilidade

| Ambiente | Status |
|----------|--------|
| QEMU | ✅ Testado e funcional |
| VirtualBox | ⚠️ Funcional, sem garantia total |
| Hardware real | ⚠️ Sem garantia |

---

## Tecnologias

- **Assembly x86** — bootloader e entry point
- **C** — kernel Basix, shell, sistema de arquivos
- **Rust** — módulo de gerenciamento de memória (`memman`)

---

## Licença

MIT — livre para usar, modificar e distribuir.
Qualquer pessoa pode forkear e distribuir sua própria versão gratuitamente.

---

## Aviso

Projeto construído com auxílio de IA como ferramenta de aprendizado,
similar ao uso de documentação técnica e OSDev Wiki.
Desenvolvido no **Linux Mint** com **Visual Studio Code**.
