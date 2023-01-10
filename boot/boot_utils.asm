WRITE_CH equ 0x0E
READ_DISK equ 0x02
BIOS_SERVICES_GENERIC equ 0x10
BIOS_SERVICES_DISK equ 0x13

; Load 'al' sectors from drive 'dl' into ES:BX
; Return number of sectors read in 'al'
disk_load:
  push ax
  mov ah, READ_DISK
  mov cl, 0x02 ; Read from sector 2 on disk
  mov ch, 0x00 ; Read from cylinder 1 on disk
  ; dl -- drive number, set by BIOS
  mov dh, 0x00 ; Read from head 0
  int BIOS_SERVICES_DISK
  pop dx
  sub dl, al
  ; jc disk_error
  je end
  ; jmp sector_error
  ; mov dl, al
  ; pop dx
  ; jmp disk_load

  end:
  ret

disk_error:
  mov si, err_msg
  call bios_print
hang:
  jmp hang

sector_error:
  mov si, sector_msg
  call bios_print
  jmp hang

bios_print:
  lodsb
  or al, al
  jz done
  mov ah, WRITE_CH
  mov bh, 0
  int BIOS_SERVICES_GENERIC
  jmp bios_print
done:
  ret

err_msg    db "Err", 0
sector_msg db "Sector", 0
