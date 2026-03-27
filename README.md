# ShellOS

Um sistema operacional básico feito do zero em Assembly e C, com foco educacional.

## Sobre o projeto

**ShellOS** é um sistema operacional minimalista construído do zero, composto por:

- **Basix** — o kernel do sistema, escrito em C
- **Bootloader** — escrito em Assembly x86
- **Shell** — interface de linha de comando integrada ao kernel

O projeto nasceu com fins educacionais, explorando conceitos fundamentais de
sistemas operacionais como bootloader, modo protegido x86, VGA memory-mapped,
driver de teclado PS/2 e arquitetura de shell.

## Comandos disponíveis

| Comando | Descrição |
|---------|-----------|
| `help`  | Lista todos os comandos disponíveis |
| `clear` | Limpa a tela |
| `echo`  | Imprime texto na tela |
| `mem`   | Exibe informações de memória |
| `ver`   | Exibe a versão do ShellOS |
| `halt`  | Desliga o sistema |

## Requisitos

- `nasm`
- `gcc` (com suporte a `-m32`)
- `binutils` (ld, objcopy)
- `qemu-system-x86`
- `xorriso`
- `VBoxManage` (opcional, para gerar .vdi)
```bash
sudo apt install nasm gcc make qemu-system-x86 xorriso
```

## Compilar e rodar
```bash
# compilar
make

# testar no QEMU (recomendado)
make run

# gerar pacotes de distribuição (.img, .vdi, .iso)
make package
```

## Compatibilidade

| Ambiente | Status |
|----------|--------|
| QEMU | ✅ Testado e funcional |
| VirtualBox | ⚠️ Funcional, sem garantia total |
| VMware | ⚠️ Sem garantia |
| Hardware real | ⚠️ Sem garantia |

> O ambiente de desenvolvimento e testes foi **Linux Mint**, utilizando **Visual Studio Code**.

## Estrutura do projeto
```
shellOS/
├── bootloader.asm   # bootloader em Assembly
├── entry.asm        # entry point do kernel
├── kernel.c         # Basix — driver VGA, teclado, scroll
├── shell.c          # loop do shell e parser de comandos
├── commands.c       # implementação dos comandos
├── linker.ld        # script de linkagem
└── Makefile         # compilação e empacotamento
```

## Licença

MIT License — livre para usar, modificar e distribuir.
Qualquer pessoa pode forkear e distribuir sua própria versão gratuitamente.

## Aviso

Este projeto foi construído com auxílio de IA como ferramenta de aprendizado,
similar ao uso de documentação técnica e OSDev Wiki. Todas as decisões de
arquitetura, debug e implementação foram tomadas e compreendidas pelo autor.
Produzido no **Linux Mint** com **Visual Studio Code**.
