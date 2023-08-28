on_wait_on_write_full:
    ldr  w0, status_address
    movk w1, #1
    and  w0, w0, w1, lsl #31
    tst  w0, w0
    b.eq on_can_write
    b    on_wait_on_write_full
    
on_can_write:
    ldr  w0, write_address
    ldr  w1, on_request
    add  w2, w1, #8
    str  w2, w0

on_wait_on_read_full:
    ldr  w0, status_address
    and  w0, w0, #1, lsl #30
    tst  w0, w0
    b.eq on_can_read
    b    on_wait_on_read_full
on_can_read:
    ldr  w0, read_address

status_address:
    .int 0x3f00b8b8
    .int 0
write_address:
    .int 0x3f00b8a0
    .int 0
on_request:
    .int 32
    .int 0
on_tag:
    .int 0x00038041
    .int 8
    .int 0x0
    .int 130
    .int 0x0
on_endtag:
    .int 0x0
read_address:
    .int 0x3f00b880 
    .int 0   

