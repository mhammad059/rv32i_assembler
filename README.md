# rv32i_assembler
A basic RISC-V assemble capable of converting rv32i instructions to machine code.

## Usage

```shell
./assembler assemblyFile.s outputFile.h -h
```
or if the required file is binary file,

```shell
./assembler assemblyFile.s outputFile.bin -b
```
### Example Usage
```shell
./assembler ./outputs_and_test_files/test_jumps_only.s ./outputs_and_test_files/output_jumps_only.dump -h
```

Single line instructions are supported only. Use labels and instruction on seperate lines.