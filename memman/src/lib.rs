#![no_std]
#![allow(dead_code)]

const HEAP_START: usize = 0x100000; // 1MB
const HEAP_END:   usize = 0x400000; // 4MB
const HEAP_SIZE:  usize = HEAP_END - HEAP_START;

static mut BUMP_PTR: usize = HEAP_START;

#[no_mangle]
pub extern "C" fn mem_alloc(size: usize) -> *mut u8 {
    unsafe {
        let aligned = (BUMP_PTR + 3) & !3; // alinha em 4 bytes
        if aligned + size > HEAP_END {
            return core::ptr::null_mut(); // sem memória
        }
        BUMP_PTR = aligned + size;
        aligned as *mut u8
    }
}

#[no_mangle]
pub extern "C" fn mem_used() -> usize {
    unsafe { BUMP_PTR - HEAP_START }
}

#[no_mangle]
pub extern "C" fn mem_free_space() -> usize {
    unsafe { HEAP_END - BUMP_PTR }
}

#[no_mangle]
pub extern "C" fn mem_reset() {
    unsafe { BUMP_PTR = HEAP_START; }
}

#[panic_handler]
fn panic(_info: &core::panic::PanicInfo) -> ! {
    loop {}
}
