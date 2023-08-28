on_wait_on_write_full:
    ldr  w0, status_address
    ldr  w0, [w0]
    add  w1, w1, #1
    and  w0, w0, w1, lsl #31
    tst  w0, w0
    b.eq on_can_write
    b    on_wait_on_write_full
on_can_write:
    ldr  w0, write_address
    ldr  w1, on_request_address
    add  w2, wzr, w1, lsl #4
    add  w2, w2, #8
    str  w2, [w0]
status_address:
    .int 0x200
write_address:
    .int 0x100
on_request_address:
    .int 0x3c
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