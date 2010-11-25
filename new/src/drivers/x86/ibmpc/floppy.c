#include "x86/floppy.h"
#include "x86/sys.h"
#include "x86/syscall.h"
#include "console.h"
#include "x86/mt.h"
#include "x86/sync.h"

static char * drive_types[8] = {
    "None",
    "360kB 5.25\"",
    "1.2MB 5.25\"",
    "720kB 3.5\"",

    "1.44MB 3.5\"",
    "2.88MB 3.5\"",
    "Unknown type",
    "Unknown type"
};

// Create a thread owned mutex
struct sync_mutex floppy_mutex = sync_thread_mutex;

const unsigned floppy_irq = 6;

unsigned char floppy_dmabuf[floppy_dmalen] __attribute__((aligned(0x8000)));

struct mt_thread* volatile floppy_waiting_thread = 0;

void floppy_irq_begin_wait() {
    floppy_waiting_thread = mt_current_thread;
}
void floppy_irq_end_wait() {
    // Maximum delay of 5 seconds
    int i = 50;
    while (floppy_waiting_thread && --i)
        syscall(&mt_sleep, 100);
}
void floppy_irq_handler(struct regs* r) {
    if (floppy_waiting_thread) {
        syscall(&mt_thread_resume, floppy_waiting_thread);
        floppy_waiting_thread = 0;
    }
}
void floppy_init() {
    irq_install_handler(floppy_irq, &floppy_irq_handler);

    outb(0x70, 0x10);
    unsigned drives = inb(0x71);

    console_puts_protected("FLOPPY DRIVE 0: ");
    console_puts_protected(drive_types[drives >> 4]);
    console_puts_protected("\n");
    console_puts_protected("FLOPPY DRIVE 1: ");
    console_puts_protected(drive_types[drives & 0xF]);
    console_puts_protected("\n");
}
err_t floppy_write_cmd(int base, char cmd) {
    int i; // do timeout, 6 seconds
    for(i = 0; i < 600; i++) {
        syscall(&mt_sleep, 10); // sleep 10 ms
        if(0x80 & inb(base + floppy_reg_main_status)) {
            outb(base + floppy_reg_data_fifo, cmd);
            return ERR_OK;
        }
    }
    return ERR(1, err_facility_floppy, floppy_err_controllertimeout);
}
unsigned char floppy_read_data(int base) {

    int i; // do timeout, 60 seconds
    for(i = 0; i < 600; i++) {
        syscall(&mt_sleep, 10); // sleep 10 ms
        if(0x80 & inb(base + floppy_reg_main_status)) {
            return inb(base + floppy_reg_data_fifo);
        }
    }
    return 0;
}
err_t floppy_check_interrupt(int base, int *st0, int *cyl) {

    floppy_write_cmd(base, floppy_cmd_sense_interrupt);

    *st0 = floppy_read_data(base);
    *cyl = floppy_read_data(base);

    return ERR_OK;
}
// Move to cylinder 0, which calibrates the drive..
err_t floppy_calibrate(int base) {
    err_t result = ERR_OK;

    int i, st0, cyl = -1; // set to bogus cylinder

    floppy_motor(base, floppy_motor_on);

    for(i = 0; i < 10; i++) {
        result = ERR_OK;

        floppy_irq_begin_wait();
        // Attempt to positions head to cylinder 0
        floppy_write_cmd(base, floppy_cmd_recalibrate);
        floppy_write_cmd(base, 0); // argument is drive, we only support 0

        floppy_irq_end_wait();

        if (ERR_FAILED(result = floppy_check_interrupt(base, &st0, &cyl))) return result;

        if(st0 & 0xC0) {
            switch (st0 >> 6) {
                case 1: result = ERR(1, err_facility_floppy, floppy_err_trackerror); break;
                case 2: result = ERR(1, err_facility_floppy, floppy_err_invalidcall); break;
                case 3: result = ERR(1, err_facility_floppy, floppy_err_drivenotready); break;
            }
            continue;
        }

        if(!cyl) { // found cylinder 0 ?
            floppy_motor(base, floppy_motor_off);
            return ERR_OK;
        }
    }

    floppy_motor(base, floppy_motor_off);

    return result;
}

static volatile int floppy_motor_ticks = 0;
static volatile int floppy_motor_state = 0;

SYSCALL_DEFINE( floppy_reset,
                syscall_state_kernel_sti, &floppy_mutex, syscall_datatype_s32
                )(int base) {
    err_t result = ERR_OK;

    floppy_irq_begin_wait();

    outb(base + floppy_reg_digital_output, 0x00); // disable controller
    outb(base + floppy_reg_digital_output, 0x0C); // enable controller

    floppy_irq_end_wait(); // sleep until interrupt occurs

    {
        int st0, cyl; // ignore these here..
        if(ERR_FAILED(result = floppy_check_interrupt(base, &st0, &cyl))) return result;
    }

    // set transfer speed 500kb/s
    outb(base + floppy_reg_config_control, 0x00);

    //  - 1st byte is: bits[7:4] = steprate, bits[3:0] = head unload time
    //  - 2nd byte is: bits[7:1] = head load time, bit[0] = no-DMA
    //
    //  steprate    = (8.0ms - entry*0.5ms)*(1MB/s / xfer_rate)
    //  head_unload = 8ms * entry * (1MB/s / xfer_rate), where entry 0 -> 16
    //  head_load   = 1ms * entry * (1MB/s / xfer_rate), where entry 0 -> 128
    //
    floppy_write_cmd(base, floppy_cmd_specify);
    floppy_write_cmd(base, 0xdf); /* steprate = 3ms, unload time = 240ms */
    floppy_write_cmd(base, 0x02); /* load time = 16ms, no-DMA = 0 */

    // it could fail...
    if(ERR_FAILED(result = floppy_calibrate(base))) return result;

    return result;
}
err_t floppy_motor(int base, int onoff) {

    if(onoff) {
        if(!floppy_motor_state) {
            // need to turn on
            outb(base + floppy_reg_digital_output, 0x1c);
            syscall(&mt_sleep, 500); // wait 500 ms = hopefully enough for modern drives
        }
        floppy_motor_state = floppy_motor_on;
    } else {
        floppy_motor_ticks = 3000; // 3 seconds, see floppy_timer() below
        floppy_motor_state = floppy_motor_wait;
    }

    return ERR_OK;
}
err_t floppy_motor_kill(int base) {
    outb(base + floppy_reg_digital_output, 0x0c);
    floppy_motor_state = floppy_motor_off;

    return ERR_OK;
}
//
// THIS SHOULD BE STARTED IN A SEPARATE THREAD.
//
//
void floppy_timer(int floppy_base) {
    while(1) {
        // sleep for 500ms at a time, which gives around half
        // a second jitter, but that's hardly relevant here.
        syscall(&mt_sleep, 500);
        if(floppy_motor_state == floppy_motor_wait) {
            floppy_motor_ticks -= 500;
            if(floppy_motor_ticks <= 0) {
                floppy_motor_kill(floppy_base);
            }
        }
    }
}
// Seek for a given cylinder, with a given head
err_t floppy_seek(int base, unsigned cyli, int head) {

    err_t result = ERR_OK;
    
    int i, st0, cyl = -1; // set to bogus cylinder

    floppy_motor(base, floppy_motor_on);

    for(i = 0; i < 10; i++) {
        result = ERR_OK;

        floppy_irq_begin_wait();
        // Attempt to position to given cylinder
        // 1st byte bit[1:0] = drive, bit[2] = head
        // 2nd byte is cylinder number
        floppy_write_cmd(base, floppy_cmd_seek);
        floppy_write_cmd(base, head<<2);
        floppy_write_cmd(base, cyli);

        floppy_irq_end_wait();

        floppy_check_interrupt(base, &st0, &cyl);

        if(st0 & 0xC0) {
            switch (st0 >> 6) {
                case 1: result = ERR(1, err_facility_floppy, floppy_err_trackerror); break;
                case 2: result = ERR(1, err_facility_floppy, floppy_err_invalidcall); break;
                case 3: result = ERR(1, err_facility_floppy, floppy_err_drivenotready); break;
            }
            continue;
        }

        if(cyl == cyli) {
            floppy_motor(base, floppy_motor_off);
            return result;
        }

    }

    floppy_motor(base, floppy_motor_off);
    return result;
}
err_t floppy_dma_init(enum floppy_dir dir) {
    err_t result = ERR_OK;

    union {
        unsigned char b[4]; // 4 bytes
        unsigned long l;    // 1 long = 32-bit
    } a, c; // address and count

    a.l = (unsigned) &floppy_dmabuf;
    c.l = (unsigned) floppy_dmalen - 1; // -1 because of DMA counting

    // check that address is at most 24-bits (under 16MB)
    // check that count is at most 16-bits (DMA limit)
    // check that if we add count and address we don't get a carry
    // (DMA can't deal with such a carry, this is the 64k boundary limit)
    if((a.l >> 24) || (c.l >> 16) || (((a.l&0xffff)+c.l)>>16)) {
        return ERR(1, err_facility_floppy, floppy_err_invalidbufferaddress);
    }

    unsigned char mode;
    switch(dir) {
        // 01:0:0:01:10 = single/inc/no-auto/to-mem/chan2
        case floppy_dir_read: 
            mode = 0x46;
            break;
        // 01:0:0:10:10 = single/inc/no-auto/from-mem/chan2
        case floppy_dir_write:
            mode = 0x4a;
            break;
        default:
            return ERR(1, err_facility_floppy, floppy_err_invalidop);
    }

    outb(0x0a, 0x06);   // mask chan 2

    outb(0x0c, 0xff);   // reset flip-flop
    outb(0x04, a.b[0]); //  - address low byte
    outb(0x04, a.b[1]); //  - address high byte

    outb(0x81, a.b[2]); // external page register

    outb(0x0c, 0xff);   // reset flip-flop
    outb(0x05, c.b[0]); //  - count low byte
    outb(0x05, c.b[1]); //  - count high byte

    outb(0x0b, mode);   // set mode (see above)

    outb(0x0a, 0x02);   // unmask chan 2

    return result;
}
// This monster does full cylinder (both tracks) transfer to
// the specified direction (since the difference is small).
//
// It retries (a lot of times) on all errors except write-protection
// which is normally caused by mechanical switch on the disk.
//
err_t floppy_do_track(int base, unsigned cyl, enum floppy_dir dir) {
   
    err_t result = ERR_OK;

    // transfer command, set below
    unsigned char cmd;

    // Read is MT:MF:SK:0:0:1:1:0, write MT:MF:0:0:1:0:1
    // where MT = multitrack, MF = MFM mode, SK = skip deleted
    //
    // Specify multitrack and MFM mode
    static const int flags = 0xC0;
    switch(dir) {
        case floppy_dir_read:
            cmd = floppy_cmd_read_data | flags;
            break;
        case floppy_dir_write:
            cmd = floppy_cmd_write_data | flags;
            break;
        default:

            return ERR(1, err_facility_floppy, floppy_err_invalidop); // not reached, but pleases "cmd used uninitialized"
    }

    // seek both heads <-- necessary to seek both?
    if (ERR_FAILED(result = floppy_seek(base, cyl, 0))) return result;
    if (ERR_FAILED(result = floppy_seek(base, cyl, 1))) return result;

    int i;
    for(i = 0; i < 20; i++) {
        result = ERR_OK;

        floppy_motor(base, floppy_motor_on);

        // init dma..
        floppy_dma_init(dir);

        syscall(&mt_sleep, 100); // give some time (100ms) to settle after the seeks

        floppy_irq_begin_wait();

        floppy_write_cmd(base, cmd);  // set above for current direction
        floppy_write_cmd(base, 0);    // 0:0:0:0:0:HD:US1:US0 = head and drive
        floppy_write_cmd(base, cyl);  // cylinder
        floppy_write_cmd(base, 0);    // first head (should match with above)
        floppy_write_cmd(base, 1);    // first sector, strangely counts from 1
        floppy_write_cmd(base, 2);    // bytes/sector, 128*2^x (x=2 -> 512)
        floppy_write_cmd(base, 18);   // number of tracks to operate on
        floppy_write_cmd(base, 0x1b); // GAP3 length, 27 is default for 3.5"
        floppy_write_cmd(base, 0xff); // data length (0xff if B/S != 0)
       
        floppy_irq_end_wait(); // don't SENSE_INTERRUPT here!

        // first read status information
        unsigned char st0, st1, st2, rcy, rhe, rse, bps;
        st0 = floppy_read_data(base);
        st1 = floppy_read_data(base);
        st2 = floppy_read_data(base);
        /*
         * These are cylinder/head/sector values, updated with some
         * rather bizarre logic, that I would like to understand.
         *
         */
        rcy = floppy_read_data(base);
        rhe = floppy_read_data(base);
        rse = floppy_read_data(base);
        // bytes per sector, should be what we programmed in
        bps = floppy_read_data(base);

        if(st0 & 0xC0) {
            switch (st0 >> 6) {
            case 1: result = ERR(1, err_facility_floppy, floppy_err_trackerror); break;
            case 2: result = ERR(1, err_facility_floppy, floppy_err_invalidcall); break;
            case 3: result = ERR(1, err_facility_floppy, floppy_err_drivenotready); break;
            }
        }
        if(st1 & 0x80) {
            result = ERR(1, err_facility_floppy, floppy_err_endofcylinder);
        }
        if(st0 & 0x08) {
            result = ERR(1, err_facility_floppy, floppy_err_drivenotready);
        }
        if(st1 & 0x20) {
            result = ERR(1, err_facility_floppy, floppy_err_crcerror);
        }
        if(st1 & 0x10) {
            result = ERR(1, err_facility_floppy, floppy_err_controllertimeout);
        }
        if(st1 & 0x04) {
            result = ERR(1, err_facility_floppy, floppy_err_nodatafound);
        }
        if((st1|st2) & 0x01) {
            result = ERR(1, err_facility_floppy, floppy_err_noaddressmarkfound);
        }
        if(st2 & 0x40) {
            result = ERR(1, err_facility_floppy, floppy_err_deletedaddressmark);
        }
        if(st2 & 0x20) {
            result = ERR(1, err_facility_floppy, floppy_err_crcerrorindata);
        }
        if(st2 & 0x10) {
            result = ERR(1, err_facility_floppy, floppy_err_wrongcylinder);
        }
        if(st2 & 0x04) {
            result = ERR(1, err_facility_floppy, floppy_err_sectornotfound);
        }
        if(st2 & 0x02) {
            result = ERR(1, err_facility_floppy, floppy_err_badcylinder);
        }
        if(bps != 0x2) {
            result = ERR(1, err_facility_floppy, floppy_err_invalidsectorsize);
        }
        if(st1 & 0x02) {
            result = ERR(1, err_facility_floppy, floppy_err_notwritable);
        }

        if(ERR_SUCCEEDED(result)) {
            floppy_motor(base, floppy_motor_off);
            return result;
        }
        if(result == ERR(1, err_facility_floppy, floppy_err_notwritable)) {
            floppy_motor(base, floppy_motor_off);
            return result;
        }
    }

    floppy_motor(base, floppy_motor_off);
    return result;

}
SYSCALL_DEFINE( floppy_read_track,
                syscall_state_kernel_sti, &floppy_mutex, syscall_datatype_s32, syscall_datatype_u32
                )(int base, unsigned cyl) {
    return floppy_do_track(base, cyl, floppy_dir_read);
}
SYSCALL_DEFINE( floppy_write_track,
                syscall_state_kernel_sti, &floppy_mutex, syscall_datatype_s32, syscall_datatype_u32
                )(int base, unsigned cyl) {
    return floppy_do_track(base, cyl, floppy_dir_write);
}
SYSCALL_DEFINE( floppy_lock_buffer,
                syscall_state_none, 0, syscall_datatype_ptr
                )(unsigned char** buffer) {
    err_t result = syscall(&sync_mutex_lock, &floppy_mutex);

    if (ERR_SUCCEEDED(result))
        *buffer = floppy_dmabuf;
    else
        *buffer = 0;

    return result;
}
SYSCALL_DEFINE( floppy_unlock_buffer,
                syscall_state_none, 0, syscall_datatype_void
                )() {
    return syscall(&sync_mutex_unlock, &floppy_mutex);
}
