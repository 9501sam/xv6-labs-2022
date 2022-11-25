### 1. Looking at the backtrace output, which function called syscall? 
kernel/trap.c:67

### 2.  What is the value of ```p->trapframe->a7``` and what does that value represent? (Hint: look user/initcode.S, the first user program xv6 starts.) 
```(gdb) p /x p->trapframe->a7```
0x7

### 3. What was the previous mode that the CPU was in? 
    * see kernel/riscv.h
```#define SSTATUS_SPP (1L << 8)  // Previous mode, 1=Supervisor, 0=User```
(gdb) p $sstatus & (1L << 8)
0 # User mode

### 4. Write down the assembly instruction the kernel is panicing at. Which register corresponds to the varialable num? 
in ```kernel/kernel.asm```

``` asm
void
syscall(void)
{
  // ...

  num = * (int *) 0;
    80001ff4:	00002683          	lw	a3,0(zero) # 0 <_entry-0x80000000>
```
register a3

```
(gdb) b *0x0000000080001ff4
(gdb) layout asm
(gdb) c
    lw      a3,0(zero) # 0x0
    a3
```

### Why does the kernel crash? Hint: look at figure 3-3 in the text; is address 0 mapped in the kernel address space? Is that confirmed by the value in scause above? (See description of scause in RISC-V privileged instructions) 
* Load page fault

### What is the name of the binary that was running when the kernel paniced? What is its process id (pid)? 
```
(gdb) p p->name
```
name = initcode

```
(gdb) p p->pid
```
pid = 1
