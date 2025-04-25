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
這裡要手動把 `trampoline` 的 address 放進 `rax`，再 `call rax` 去 indirect call[^1]，而不是直接 `call trampoline`，是因為 `call trampoline` 是 runtime 時手動塞進 memory 中的，compiler, linker 不能幫我 resolve symbol。

### Rewrite `syscall`
Implemented in `rewrite_syscall()`.
想了老半天，研究老半天到底要怎麼從 `syscall` 換成 `call rax`，又可以讓 `rax` 剛好裝著會去到 trampoline 的地方。一開始硬要把 `movabs rax, 0x200; call rax` 裝到 `syscall` 的位置，但 12 bytes 的那兩條根本就放不進去，然後又一直把東西寫爛；後來才意識到前面 map 512 bytes 的 `nop` 就是為了開開心心讓所有 `syscall` 都跳去發呆，然後走到 trampoline 去。

### `clone` & `clone3`
這兩個的問題是，如果 child process 和 parent 沒有共用 stack (`CLONE_VM` being set) 的情況下，child process stack 中本來應該存著的 return 回到 `syscall` 的 caller 的 address 會不見，child 就會跳到一個 pop 出來的亂七八糟的宇宙盡頭。所以如果有 `CLONE_VM`，就要手動維護 child process 的 stack。
左思右想，覺得 handle 特殊的 `syscall` & 處理 child process stack 的工作不能給 `hooked_syscall` 以後的東西做，因為 `syscall` 的 hooker 不應該關心 `syscall` hooking 有什麼特殊的細節。那就只能在 `syscall_handler` 這裡做，而且他名字就叫 `syscall_handler` 了哈哈。
具體做法基本上就是把本來 `clone` 和 `clone3` 的裝著 child stack pointer value 的東西拿出來，往 child stack pointer 塞他本來應該要有的 return address。Child process 的 return address 的取得就在 `trampoline` 裡面去做，拿到之後 call `set_retptr` 去把 return pointer 放到 thread local storage。放 TLS 是因為隨便放個 global variable 會 race condition，放 stack 又很容易 push, pop 幾次就搞砸 return pointer，TLS 是不用上 lock 又最好維護的東西了。

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
