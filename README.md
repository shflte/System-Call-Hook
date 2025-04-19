# System Call Hook and Logging
## Setup Trampoline
基本上就是讓 shared library 被 load 時，去把 `0x0` 開始的這段 memory 塞好 trampoline code。

```
movabs rax, address
call rax
ret
```
這裡要手動把 `trampoline_handler` 的 address 放進 `rax`，再 `call rax` 去 indirect call[^1]，而不是直接 `call trampoline_handler`，是因為 `call trampoline_handler` 是 runtime 時手動塞進 memory 中的，compiler, linker 不能幫我 resolve symbol。

---
[^1]: `call rel32` 是 RIP-relative 的 jump；`call r/m64` 是直接 jump 到 `imm64` 或 `r` 中存著的 memory address。
