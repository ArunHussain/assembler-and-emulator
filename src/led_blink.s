on_wait_on_write_full:
    ldr  w0, status_address
    ldr  w0, [w0]
    and  w0, w0, #1, lsl #31
    tst  w0, w0
    b.eq on_can_write
    b    on_wait_on_write_full
on_can_write:
    ldr  w0, write_address
    ldr  w1, on_request_address
    add  w2, wzr, w1, lsl #5
    movk w2, #8
    str  w2, [w0]
on_wait_on_read_full:
    ldr  w0, status_address
    ldr  w0, [w0]
    and  w0, w0, #1, lsl #30
    tst  w0, w0
    b.eq on_can_read
    b    on_wait_on_read_full
on_can_read:
    ldr  w0, read_address
on_wait_1_billion_cycles:
    movz x0, #1000000000
on_wait_loop_start:
    sub  x0, x0, #1
    tst  x0, x0
    b.eq off_wait_on_write_full
    b    on_wait_loop_start
off_wait_on_write_full:
    ldr  w0, status_address
    ldr  w0, [w0]
    and  w0, w0, #1, lsl #31
    tst  w0, w0
    b.eq off_can_write
    b    off_wait_on_write_full
off_can_write:
    ldr  w0, write_address
    ldr  w1, off_request_address
    add  w2, wzr, w1, lsl #5
    movk w2, #8
    str  w2, [w0]
off_wait_1_billion_cycles:
    movz x0, #1000000 000
off_wait_loop_start:
    sub  x0, x0, #1
    tst  x0, x0
    b.eq on_wait_on_write_full
    b    off_wait_loop_start

start_address:
    .int 0x80000
mailbox_address:
    .int 0x3f00b880
status_address:
    .int 0x3f00b8b8
write_address:
    .int 0x3f00b8a0
read_address:
    .int 0x3f00b880
on_request_address:
    .int 0x800b8
off_request_address:
    .int 0x800c0

on_request:
    .int 32
    .int 0
on_tag:
    .int 0x00038041
    .int 8
    .int 0x0
    .int 130
    .int 0x1
on_endtag:
    .int 0x0

off_request:
    .int 32
    .int 0
off_tag:
    .int 0x00038041
    .int 8
    .int 0x0
    .int 130
    .int 0x0
off_endtag:
    .int 0x0
    

