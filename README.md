# System Call Hook and Logging

## Environment Setup
**Tools**
```
sudo apt update
sudo apt install qemu-system-x86 qemu-utils debootstrap libguestfs-tools -y
```

**Build Image**
```
./build_vm_image.sh
```

**Run VM**
```
./run_vm.sh
```

**Mount Host Shared Folder**
```
# as the guest
modprobe 9p
mkdir /mnt/host
mount -t 9p -o trans=virtio hostshare /mnt/host
cd /mnt/host
```

## Implementation
### Hook Flow
- Shared library 被 load 時，會 setup trampoline & rewrite `syscall` 成 trampoline code。
- Trampoline entry 基本上就是把 registers 中本來下 `syscall` 的 parameters 放成 C 的 calling convention，然後去 call C 的 system call handler。
- C 做完事之後會把 registers 還原，然後 trigger 原本的 system call。

### Setup Trampoline
Implemented in `init_trampoline.c`.
這裡基本上就是把 `0x0` 開始的這段 memory 塞好 NOPs 和 trampoline entry。

```
movabs rax, address
call rax
ret
```
這裡要手動把 `trampoline_entry` 的 address 放進 `rax`，再 `call rax` 去 indirect call[^1]，而不是直接 `call trampoline_entry`，是因為 `call trampoline_entry` 是 runtime 時手動塞進 memory 中的，compiler, linker 不能幫我 resolve symbol。

### Rewrite `syscall`
Implemented in `rewrite_syscall()`.
想了老半天，研究老半天到底要怎麼從 `syscall` 換成 `call rax`，又可以讓 `rax` 剛好裝著會去到 trampoline 的地方。一開始硬要把 `movabs rax, 0x200; call rax` 裝到 `syscall` 的位置，但 12 bytes 的那兩條根本就放不進去，然後又一直把東西寫爛；後來才意識到前面 map 512 bytes 的 `nop` 就是為了開開心心讓所有 `syscall` 都跳去發呆，然後走到 trampoline 去。

---
## Calling Convention

| Argument   | System V C Function Call Register | Linux `syscall` Register  |
|------------|-----------------------------------|---------------------------|
| syscall no | -                                 | rax                       |
| arg1       | rdi                               | rdi                       |
| arg2       | rsi                               | rsi                       |
| arg3       | rdx                               | rdx                       |
| arg4       | rcx                               | r10                       |
| arg5       | r8                                | r8                        |
| arg6       | r9                                | r9                        |
| return     | rax                               | rax                       |

---
[^1]: `call rel32` 是 RIP-relative 的 jump；`call r/m64` 是直接 jump 到 `imm64` 或 `r` 中存著的 memory address。
