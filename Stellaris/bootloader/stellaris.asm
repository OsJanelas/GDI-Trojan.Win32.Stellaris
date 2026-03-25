[bits 16]
[ORG 0x7C00]

%define WSCREEN 320
%define HSCREEN 200

start:
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00

    mov ax, 0x0013
    int 0x10

main_sequence:
    mov ax, 0x0040
    mov gs, ax
    mov eax, [gs:0x006C]
    sub eax, [start_tick]
    
    cmp eax, 55
    jb statics
    cmp eax, 145
    jb xorfractal
    cmp eax, 236
    jb formule
    cmp eax, 327
    jb sierspinski
    cmp eax, 454
    jb aurora

    mov eax, [gs:0x006C]
    mov [start_tick], eax
    jmp statics

statics:
    mov ax, 0xa000
    mov es, ax
    xor di, di
    mov cx, 64000
.bg:
    in al, 0x40
    and al, 0x1F
    stosb
    loop .bg

    mov si, msg
    call print_string_center
    inc byte [timer]
    jmp end_frame

sierspinski:
    mov ax, 0xa000
    mov es, ax
    xor di, di
.loop_y:
    mov ax, di
    xor dx, dx
    mov bx, 320
    div bx          ; ax = y, dx = x
    mov bx, ax
    and bx, dx
    jnz .black
    mov al, dl
    add al, [timer]
    stosb
    jmp .next
.black:
    xor al, al
    stosb
.next:
    cmp di, 64000
    jne .loop_y
    inc byte [timer]

    mov si, msg
    call print_string_center
    jmp end_frame

formule:
    mov ax, 0xa000
    mov es, ax
    xor di, di
    xor cx, cx      ; y
.y:
    xor dx, dx      ; x
.x:
    mov ax, dx
    add ax, [word_timer]
    mov si, cx
    add si, [word_timer]
    imul ax, si
    and al, 0x3F
    stosb
    inc dx
    cmp dx, 320
    jne .x
    inc cx
    cmp cx, 200
    jne .y
    
    mov si, msg
    call print_string_center
    inc word [word_timer]
    jmp end_frame

xorfractal:
    mov ax, 0xa000
    mov es, ax
    xor di, di
.draw:
    mov ax, di
    mov bx, 320
    xor dx, dx
    div bx          ; ax=y, dx=x
    xor al, dl
    add al, [timer]
    stosb
    cmp di, 64000
    jne .draw
    
    mov si, msg
    call print_string_center
    inc byte [timer]
    jmp end_frame

aurora:
    inc byte [time]

    movzx ax, byte [time]
    mov [temp_w], ax

    fild word [temp_w]
    fsincos
    fstp dword [cosa]
    fstp dword [sina]

    push es
    mov ax, 0xA000
    mov es, ax
    xor di, di

    mov word [y], 0
.y_loop:
    mov word [x], 0
.x_loop:
    mov ax, [x]
    sub ax, 160
    mov [temp_w], ax
    fild word [temp_w]
    fmul dword [cosa]   ; X * cos

    ; (Y - 100)
    mov ax, [y]
    sub ax, 100
    mov [temp_w], ax
    fild word [temp_w]
    fmul dword [sina]   ; Y * sin

    fsubp               ; (X*cos) - (Y*sin)
    fistp word [gx]

    mov ax, [gx]
    add al, [time]
    stosb

    inc word [x]
    cmp word [x], WSCREEN
    jl .x_loop

    inc word [y]
    cmp word [y], HSCREEN
    jl .y_loop
    pop es

    mov ah, 0x02
    xor bh, bh
    mov dx, 0x010E
    int 0x10

    mov si, msg
    call print_string_center
    jmp end_frame

end_frame:
    ; Wait for vertical retrace (prevents flickering)
    mov dx, 0x3DA
.wait:
    in al, dx
    test al, 8
    jz .wait
    
    jmp main_sequence

print_string_center:
    ; Reset cursor
    mov ah, 02h
    xor bh, bh
    mov dx, 0x0C0F
    int 10h
.loop:
    lodsb
    test al, al
    jz .done
    mov ah, 0x0E
    mov bl, [timer]
    and bl, 0x0F
    add bl, 32
    int 10h
    jmp .loop
.done:
    ret

; DATA
t dw 0
time:   db 0
temp_w: dw 0
x:      dw 0
y:      dw 0
gx:     dw 0
sina:   dd 0.0
cosa:   dd 0.0
start_tick  dd 0
timer       db 0
word_timer  dw 0
msg   db "YOU HAVING A BAD DREAM", 0x00

times 510-($-$$) db 0
dw 0xAA55