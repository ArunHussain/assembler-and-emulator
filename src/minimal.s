on_wait_on_write_full:
    ldr  w0, status_address
    ldr  w0, [w0]
    add  w1, w1, #1
    and  w0, w0, w1, lsl #31
    tst  w0, w0
    b.eq on_can_write
    b    on_wait_on_write_full
on_can_write:
    and  x0, x0, x0
status_address:
    .int 0x100