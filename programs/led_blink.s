ldr w1, one    
on_can_wait_init:
    ldr w10, loop_times
    ldr w11, zero
    ldr w12, one
on_wait:
    sub w10, w10, w12
    cmp w10, w11
    b.ne on_wait
on_wait_on_write_full:
    ldr  w0, status_address
    ldr  w0, [w0]
    ldr  w1, one
    and  w0, w0, w1, lsl #31
    tst  w0, w0
    b.eq copy_on
    b    on_wait_on_write_full
on_can_write:
    ldr  w0, write_address
    ldr  w4, on_request_address
    ldr  w3, zero 
    ldr  w8, eight
    add  w2, w3, w4
    add  w2, w2, w8
    str  w2, [w0]
on_wait_on_read_full:
    ldr  w0, status_address
    ldr  w0, [w0]
    and  w0, w0, w1, lsl #30
    tst  w0, w0
    b.eq on_can_read
    b    on_wait_on_read_full
on_can_read:
    ldr  w0, read_address
    ldr  w0,[w0]
    b    on_can_wait_init_2
on_wait_on_write_full_2:
    ldr  w0, status_address_2
    ldr  w0, [w0]
    and  w0, w0, w1, lsl #31
    tst  w0, w0
    b.eq copy_off
    b    on_wait_on_write_full_2
on_can_write_2:
    ldr  w0, write_address
    ldr  w4, off_request_address
    ldr  w3, zero 
    add  w2, w3, w4
    add  w2, w2, w8
    str  w2, [w0]
on_wait_on_read_full_2:
    ldr  w0, status_address_2
    ldr  w0, [w0]
    ldr  w1, one 
    and  w0, w0, w1, lsl #30
    tst  w0, w0
    b.eq on_can_read_2
    b    on_wait_on_read_full_2
on_can_read_2:
    ldr  w0, read_address
    ldr  w0, [w0]
pad:
    b on_can_wait_init
status_address:
    .int 0x3f00b8b8
write_address:
    .int 0x3f00b8a0
pad3:
    .int 0
on_request_address:
    .int 0x800e0
on_request:
    .int 0
    .int 0
    .int 0
    .int 0
    .int 0
    .int 0
    .int 0
    .int 0
zero:
    .int 0
pad2:
    .int 0
    .int 0
off_request_address:
    .int 0x80110
off_request:
    .int 0
    .int 0
    .int 0
    .int 0
    .int 0
    .int 0
    .int 0
    .int 0
zero:
    .int 0
loop_times:
    .int 10000000
one:
    .int 1
f1:
    .int 32
f2:
    .int 0
f3:
    .int 0x00038041
f4:
    .int 8
f5:
    .int 0x0
f6:
    .int 130
f7:
    .int 0x0
f8:
    .int 0
read_address:
    .int 0x3f00b880 
status_address_2:
    .int 0x3f00b8b8
copy_on:
    ldr  w20, on_request_address
    ldr  w3, f1
    str  w3, [w20, #0]
    ldr  w3, f2
    str  w3, [w20, #4]
    ldr  w3, f3
    str  w3, [w20, #8]
    ldr  w3, f4
    str  w3, [w20, #12]
    ldr  w3, f5
    str  w3, [w20, #16]
    ldr  w3, f6
    str  w3, [w20, #20]
    ldr  w3, f7
    ldr  w19, one 
    sub  w3, w19, w3
    str  w3, [w20, #24]
    ldr  w3, f8
    str  w3, [w20, #28]
    b    on_can_write
copy_off:
    ldr  w20, off_request_address
    ldr  w3, f1
    str  w3, [w20, #0]
    ldr  w3, f2
    str  w3, [w20, #4]
    ldr  w3, f3
    str  w3, [w20, #8]
    ldr  w3, f4
    str  w3, [w20, #12]
    ldr  w3, f5
    str  w3, [w20, #16]
    ldr  w3, f6
    str  w3, [w20, #20]
    ldr  w3, f7
    str  w3, [w20, #24]
    ldr  w3, f8
    str  w3, [w20, #28]
    b    on_can_write_2
eight:
    .int 8
on_can_wait_init_2:
    ldr w10, loop_times
    ldr w11, zero
    ldr w12, one
on_wait_2:
    sub w10, w10, w12
    cmp w10, w11
    b.ne on_wait_2
    b  on_wait_on_write_full_2