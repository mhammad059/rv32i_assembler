# rv32i_assembler
A basic RISC-V assemble capable of converting rv32i instructions to machine code.

## Usage

```shell
./assembler.exe assemblyFile.s outputFile.h -h
```
or if the required file is binary file,

```shell
./assembler.exe assemblyFile.s outputFile.bin -b
```
Single line instructions are supported only. Use labels and instruction on seperate lines.