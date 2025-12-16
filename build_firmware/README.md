

```
arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -O2 -T linker.ld -nostartfiles startup.c arm_main.c -o output.elf
```
