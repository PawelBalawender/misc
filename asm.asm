format ELF

section '.text'
  public _start

_start:
  mov eax, 4
  mov ebx, 1
  mov ecx, tekst
  mov edx, 5
  int 80h
  
  xor ebx,ebx
  mov eax,1
  int 80h

section '.data'
  tekst db "Hello", 0

