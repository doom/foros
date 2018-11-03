# foros
Tiny kernel (educational project)

## Build instructions

#### Prerequisites

- `make`
- `gcc` or `clang`
- `nasm`
- `ld`
- `grub-mkrescue` / `xorriso`
- `qemu`

#### Building

Build:

```bash
> make
```

Run:

```bash
> make run
```

Debug:

```bash
> make P=debug run
```

## Features

- [x] Boot
- [x] Stack setup
- [x] CPU capabilities checks
- [x] Page table setup (using recursive mapping)
- [x] GDT setup
- [x] Switch to long mode
- [x] VGA text buffer abstraction
- [x] IDT setup / Exceptions handling
