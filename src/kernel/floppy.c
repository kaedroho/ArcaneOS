#include "floppy.h"
#include "mt.h"
#include "sys.h"
#include "syscall.h"
#include "libs/memory.h"

int floppy_last_error = 0;
volatile int floppy_irq_fired = 1;
volatile int floppy_motor_ticks = 0;
volatile int floppy_motor_state = 0;
unsigned int floppy_drives = 0;
int floppy_base = 0x03f0;
// standard IRQ number for floppy controllers
int floppy_irq = 6;
int floppy_cache_valid = 0;
int floppy_cache_cylinder = 0;
int floppy_cache_flushed = 1;

unsigned char floppy_dmabuf[floppy_dmalen];

struct floppy_command floppy_command_queue[floppy_queue_size];
unsigned int floppy_command_queue_front = 0;
unsigned int floppy_command_queue_back = 0;

int floppy_get_last_error()
{
    return floppy_last_error;
}

void floppy_irq_handler(struct regs *r)
{
    floppy_irq_fired = 1;
}

void floppy_check_disk_change(int base)
{
    if (inb(base+floppy_ccr) & 0x80)
    {
        // Disk changed
        floppy_cache_valid = 0;
    }
}

void floppy_write_cmd(int base, char cmd) {
    int i; // do timeout, 0.6 seconds
    for(i = 0; i < 60; i++) {
        syscall_sleep(10); // sleep 10 ms
        floppy_check_disk_change(base);
        if((0xC0 & inb(base+floppy_msr)) == 0x80) {
            outb(base+floppy_fifo, cmd);
            floppy_last_error = floppy_error_success; // SUCCESS
            return;
        }
    }
    floppy_last_error = floppy_error_timeout; // FAILURE
}

unsigned char floppy_read_data(int base) {

    int i; // do timeout, 6 seconds
    for(i = 0; i < 600; i++) {
        syscall_sleep(10); // sleep 10 ms
        if((0xC0 & inb(base+floppy_msr)) == 0x80) {
            floppy_last_error = floppy_error_success;
            return inb(base+floppy_fifo); // SUCCESS
        }
    }
    floppy_last_error = floppy_error_timeout; // FAILURE
    return 0;
}

void floppy_check_interrupt(int base, int *st0, int *cyl) {

    floppy_write_cmd(base, floppy_cmd_sense_interrupt);
    if (floppy_last_error)
        return;

    *st0 = floppy_read_data(base);
    if (floppy_last_error)
        return;

    *cyl = floppy_read_data(base);
    if (floppy_last_error)
        return;

    floppy_last_error = floppy_error_success;
}

void floppy_wait_irq()
{
    int i;
    for (i = 0; (i < 10) && (!floppy_irq_fired); i++)
        syscall_sleep(10);

    if (floppy_irq_fired)
        floppy_last_error = floppy_error_success;
    else
        floppy_last_error = floppy_error_timeout;
}

void floppy_try_calibrate(int base)
{
    int st0, cyl = -1; // set to bogus cylinder
    floppy_irq_fired = 0;

    // Attempt to positions head to cylinder 0
    floppy_write_cmd(base, floppy_cmd_recalibrate);
    if (floppy_last_error)
        return;

    floppy_write_cmd(base, 0); // argument is drive, we only support 0
    if (floppy_last_error)
        return;

    floppy_wait_irq();
    if (floppy_last_error)
        return;

    floppy_check_interrupt(base, &st0, &cyl);
    if (floppy_last_error)
        return;

    if(st0 & 0xC0) {
        switch (st0 >> 6)
        {
        case 1: floppy_last_error = floppy_error_failed; return;
        case 2: floppy_last_error = floppy_error_invalid; return;
        case 3: floppy_last_error = floppy_error_drive; return;
        }
    }

    if(!cyl) { // found cylinder 0 ?
        floppy_motor(base, floppy_motor_off);
        if (floppy_last_error)
            return;

        floppy_last_error = floppy_error_success;
        return;
    }

    floppy_last_error = floppy_error_failed;
}

// Move to cylinder 0, which calibrates the drive..
void floppy_calibrate(int base) {

    int i;

    floppy_motor(base, floppy_motor_on);
    if (floppy_last_error)
        return;

    for(i = 0; i < 10; i++) {
        floppy_try_calibrate(base);
        if (floppy_last_error == floppy_error_success)
            return;
    }

    int error = floppy_last_error;
    floppy_motor(base, floppy_motor_off);
    floppy_last_error = error;
}


void floppy_reset(int base) {

    floppy_irq_fired = 0;

    outb(base + floppy_dor, 0x00); // disable controller
    outb(base + floppy_dor, 0x0C); // enable controller

    floppy_wait_irq();
    if (floppy_last_error)
        return;

    {
        int st0, cyl; // ignore these here..
        floppy_check_interrupt(base, &st0, &cyl);
        if (floppy_last_error)
            return;
    }

    // set transfer speed 500kb/s
    outb(base + floppy_ccr, 0x00);

    //  - 1st byte is: bits[7:4] = steprate, bits[3:0] = head unload time
    //  - 2nd byte is: bits[7:1] = head load time, bit[0] = no-DMA
    //
    //  steprate    = (8.0ms - entry*0.5ms)*(1MB/s / xfer_rate)
    //  head_unload = 8ms * entry * (1MB/s / xfer_rate), where entry 0 -> 16
    //  head_load   = 1ms * entry * (1MB/s / xfer_rate), where entry 0 -> 128
    //
    floppy_write_cmd(base, floppy_cmd_specify);
    if (floppy_last_error)
        return;
    floppy_write_cmd(base, 0xdf); /* steprate = 3ms, unload time = 240ms */
    if (floppy_last_error)
        return;
    floppy_write_cmd(base, 0x02); /* load time = 16ms, no-DMA = 0 */
    if (floppy_last_error)
        return;

    // it could fail...
    floppy_calibrate(base);
    if (floppy_last_error)
        return;

    floppy_last_error = floppy_error_success;
}

void floppy_motor(int base, int onoff) {

    if(onoff) {
        if(!floppy_motor_state) {
            // need to turn on
            outb(base + floppy_dor, 0x1c);
            syscall_sleep(500); // wait 500 ms = hopefully enough for modern drives
        }
        floppy_motor_state = floppy_motor_on;
    } else {
        floppy_motor_ticks = 300; // 3 seconds, see floppy_timer() below
        floppy_motor_state = floppy_motor_wait;
    }
}

void floppy_motor_kill(int base) {
    outb(base + floppy_dor, 0x0c);
    floppy_motor_state = floppy_motor_off;
}

void floppy_try_seek(int base, unsigned cyli, int head)
{
    int st0, cyl = -1; // set to bogus cylinder

    // Attempt to position to given cylinder
    // 1st byte bit[1:0] = drive, bit[2] = head
    // 2nd byte is cylinder number

    floppy_irq_fired = 0;

    floppy_write_cmd(base, floppy_cmd_seek);
    if (floppy_last_error)
        return;

    floppy_write_cmd(base, head<<2);
    if (floppy_last_error)
        return;

    floppy_write_cmd(base, cyli);
    if (floppy_last_error)
        return;

    floppy_wait_irq();
    if (floppy_last_error)
        return;

    floppy_check_interrupt(base, &st0, &cyl);
    if (floppy_last_error)
        return;
    cli_puts("Done...\n");


    if(st0 & 0xC0) {
        switch (st0 >> 6)
        {
        case 1: floppy_last_error = floppy_error_failed; return;
        case 2: floppy_last_error = floppy_error_invalid; return;
        case 3: floppy_last_error = floppy_error_drive; return;
        }
    }

    if(cyl == cyli) {
        floppy_motor(base, floppy_motor_off);
        if (floppy_last_error)
            return;

        floppy_last_error = floppy_error_success;
        return;
    }

    floppy_last_error = floppy_error_failed;
}

// Seek for a given cylinder, with a given head
void floppy_seek(int base, unsigned cyli, int head) {

    unsigned i; // set to bogus cylinder

    floppy_motor(base, floppy_motor_on);
    if (floppy_last_error)
        return;

    for(i = 0; i < 3; i++) {
        floppy_try_seek(base, cyli, head);
        if (floppy_last_error == floppy_error_success)
            return;
    }

    int error = floppy_last_error;
    floppy_motor(base, floppy_motor_off);
    floppy_last_error = error;
}

//
// THIS SHOULD BE STARTED IN A SEPARATE THREAD.
//
//
void floppy_timer() {
    while(1) {
        // sleep for 500ms at a time, which gives around half
        // a second jitter, but that's hardly relevant here.
        syscall_sleep(500);
        if(floppy_motor_state == floppy_motor_wait) {
            floppy_motor_ticks -= 50;
            if(floppy_motor_ticks <= 0) {
                floppy_motor_kill(floppy_base);
            }
        }
    }
}

void floppy_driver()
{
    while (1)
    {
        while (floppy_command_queue_front != floppy_command_queue_back)
        {
            struct floppy_command* cmd = floppy_command_queue + floppy_command_queue_front;
            switch (cmd->command)
            {
            case floppy_command_read:
                floppy_read(cmd->cylinder,cmd->head,cmd->sector,cmd->offset,cmd->size,cmd->buffer);
                *cmd->error = floppy_last_error;
                break;
            case floppy_command_write:
                floppy_write(cmd->cylinder,cmd->head,cmd->sector,cmd->offset,cmd->size,cmd->buffer);
                *cmd->error = floppy_last_error;
                break;
            case floppy_command_flush:
                floppy_flush();
                *cmd->error = floppy_last_error;
                break;
            }

            floppy_command_queue_front = (floppy_command_queue_front+1) % floppy_queue_size;
        }

        syscall_sleep(50);
    }
}

void floppy_push_command(unsigned int id, unsigned int cylinder, unsigned int head, unsigned int sector, unsigned int offset, unsigned int size, const unsigned char* buffer, unsigned int* error)
{
    *error = floppy_error_queueing;

    if (((floppy_command_queue_back+1) % floppy_queue_size) == floppy_command_queue_front)
    {
        *error = floppy_error_full_queue;
        return;
    }

    struct floppy_command* cmd = floppy_command_queue + floppy_command_queue_back;
    cmd->command = id;
    cmd->cylinder = cylinder;
    cmd->head = head;
    cmd->sector = sector;
    cmd->offset = offset;
    cmd->size = size;
    cmd->buffer = (unsigned char*)buffer;
    cmd->error = error;

    floppy_command_queue_back = (floppy_command_queue_back+1) % floppy_queue_size;
}

void floppy_dma_init(floppy_dir dir) {

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
        floppy_last_error = floppy_error_dma_buffer;
        return;
    }

    unsigned char mode;
    switch(dir) {
        // 01:0:0:01:10 = single/inc/no-auto/to-mem/chan2
        case floppy_dir_read:  mode = 0x46; break;
        // 01:0:0:10:10 = single/inc/no-auto/from-mem/chan2
        case floppy_dir_write: mode = 0x4a; break;
        default: floppy_last_error = floppy_error_dma_direction;
                 return; // not reached, please "mode user uninitialized"
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
}

// This monster does full cylinder (both tracks) transfer to
// the specified direction (since the difference is small).
//
// It retries (a lot of times) on all errors except write-protection
// which is normally caused by mechanical switch on the disk.
//

void floppy_try_do_track(int base, unsigned cyl, floppy_dir dir, unsigned char cmd)
{
    floppy_motor(base, floppy_motor_on);
    if (floppy_last_error)
        return;

    // init dma..
    floppy_dma_init(dir);
    if (floppy_last_error)
        return;

    syscall_sleep(100); // give some time (100ms) to settle after the seeks

    floppy_irq_fired = 0;

    floppy_write_cmd(base, cmd);  // set above for current direction
    if (floppy_last_error)
        return;
    floppy_write_cmd(base, 0);    // 0:0:0:0:0:HD:US1:US0 = head and drive
    if (floppy_last_error)
        return;
    floppy_write_cmd(base, cyl);  // cylinder
    if (floppy_last_error)
        return;
    floppy_write_cmd(base, 0);    // first head (should match with above)
    if (floppy_last_error)
        return;
    floppy_write_cmd(base, 1);    // first sector, strangely counts from 1
    if (floppy_last_error)
        return;
    floppy_write_cmd(base, 2);    // bytes/sector, 128*2^x (x=2 -> 512)
    if (floppy_last_error)
        return;
    floppy_write_cmd(base, 18);   // number of tracks to operate on
    if (floppy_last_error)
        return;
    floppy_write_cmd(base, 0x1b); // GAP3 length, 27 is default for 3.5"
    if (floppy_last_error)
        return;
    floppy_write_cmd(base, 0xff); // data length (0xff if B/S != 0)
    if (floppy_last_error)
        return;

    floppy_wait_irq(); // don't SENSE_INTERRUPT here!
    if (floppy_last_error)
        return;

    // first read status information
    unsigned char st0, st1, st2, rcy, rhe, rse, bps;
    st0 = floppy_read_data(base);
    if (floppy_last_error)
        return;
    st1 = floppy_read_data(base);
    if (floppy_last_error)
        return;
    st2 = floppy_read_data(base);
    if (floppy_last_error)
        return;
    /*
     * These are cylinder/head/sector values, updated with some
     * rather bizarre logic, that I would like to understand.
     *
     */
    rcy = floppy_read_data(base);
    if (floppy_last_error)
        return;
    rhe = floppy_read_data(base);
    if (floppy_last_error)
        return;
    rse = floppy_read_data(base);
    if (floppy_last_error)
        return;
    // bytes per sector, should be what we programmed in
    bps = floppy_read_data(base);
    if (floppy_last_error)
        return;

    if(st0 & 0xC0) {
        switch (st0 >> 6)
        {
        case 1: floppy_last_error = floppy_error_failed; return;
        case 2: floppy_last_error = floppy_error_invalid; return;
        case 3: floppy_last_error = floppy_error_drive; return;
        }
    }
    if(st1 & 0x80) {
        floppy_last_error = floppy_error_end_of_cylinder;
        return;
    }
    if(st0 & 0x08) {
        floppy_last_error = floppy_error_drive;
        return;
    }
    if(st1 & 0x20) {
        floppy_last_error = floppy_error_crc;
        return;
    }
    if(st1 & 0x10) {
        floppy_last_error = floppy_error_timeout;
        return;
    }
    if(st1 & 0x04) {
        floppy_last_error = floppy_error_no_data;
        return;
    }
    if((st1|st2) & 0x01) {
        floppy_last_error = floppy_error_no_address_mark;
        return;
    }
    if(st2 & 0x40) {
        floppy_last_error = floppy_error_deleted_address_mark;
        return;
    }
    if(st2 & 0x20) {
        floppy_last_error = floppy_error_crc;
        return;
    }
    if(st2 & 0x10) {
        floppy_last_error = floppy_error_wrong_cylinder;
        return;
    }
    if(st2 & 0x04) {
        floppy_last_error = floppy_error_sector_not_found;
        return;
    }
    if(st2 & 0x02) {
        floppy_last_error = floppy_error_bad_cylinder;
        return;
    }
    if(bps != 0x2) {
        floppy_last_error = floppy_error_sector_size;
        return;
    }
    if(st1 & 0x02) {
        floppy_last_error = floppy_error_not_writable;
        return;
    }

    floppy_motor(base, floppy_motor_off);
    if (floppy_last_error)
        return;

    floppy_last_error = floppy_error_success;
}

void floppy_do_track(int base, unsigned cyl, floppy_dir dir) {

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
            floppy_last_error = floppy_error_dma_direction;
            return;
    }

    // seek both heads
    floppy_seek(base, cyl, 0);
    if (floppy_last_error)
        return;
    floppy_seek(base, cyl, 1);
    if (floppy_last_error)
        return;

    int i;
    for(i = 0; i < 5; i++) {
        floppy_try_do_track(base,cyl,dir,cmd);
        if (!floppy_last_error)
            return;

        floppy_motor(base, floppy_motor_off);

        if (floppy_last_error == floppy_error_not_writable)
            return;
    }

    int error = floppy_last_error;
    floppy_motor(base, floppy_motor_off);
    floppy_last_error = error;
}

void floppy_read_track(unsigned cyl) {
    floppy_do_track(floppy_base, cyl, floppy_dir_read);
}

void floppy_write_track(unsigned cyl) {
    floppy_do_track(floppy_base, cyl, floppy_dir_write);
}

int floppy_drive_exists()
{
    // Only 1.44MB 3.5" floppy drives are supported
    return (floppy_drives >> 4) == 4;
}

void floppy_prepare(unsigned int cylinder)
{
    if (floppy_cache_valid && (!floppy_cache_flushed) && (floppy_cache_cylinder != cylinder))
    {
        floppy_write_track(cylinder);
        if (floppy_last_error)
            return;

        floppy_cache_flushed = 1;
    }

    if ((!floppy_cache_valid) || (floppy_cache_cylinder != cylinder))
    {
        floppy_cache_valid = 0;
        floppy_read_track(cylinder);
        if (floppy_last_error)
            return;

        floppy_cache_valid = 1;
        floppy_cache_cylinder = cylinder;
    }

    floppy_last_error = floppy_error_success;
}

void floppy_read(unsigned int cylinder, unsigned int head, unsigned int sector, unsigned int offset, unsigned int size, unsigned char* buffer)
{
    if ((cylinder > 79) || (head > 1) || (sector > 17) || (offset > 512) || (size > 512) || (offset+size > 512))
    {
        floppy_last_error = floppy_error_invalid;
        return;
    }

    floppy_prepare(cylinder);
    if (floppy_last_error)
        return;

    offset += (sector*2+head)*512;
    memcpy(buffer,floppy_dmabuf+offset,size);

    floppy_last_error = floppy_error_success;
}

void floppy_write(unsigned int cylinder, unsigned int head, unsigned int sector, unsigned int offset, unsigned int size, const unsigned char* buffer)
{
    if ((cylinder > 79) || (head > 1) || (sector > 17) || (offset > 512) || (size > 512) || (offset+size > 512))
    {
        floppy_last_error = floppy_error_invalid;
        return;
    }

    floppy_prepare(cylinder);
    if (floppy_last_error)
        return;

    offset += (sector*2+head)*512;

    floppy_cache_flushed = 0;
    memcpy(floppy_dmabuf+offset,buffer,size);

    floppy_last_error = floppy_error_success;
}

void floppy_flush()
{
    if (floppy_cache_valid && (!floppy_cache_flushed))
    {
        floppy_write_track(floppy_cache_cylinder);
        if (floppy_last_error)
            return;

        floppy_cache_flushed = 1;
    }

    floppy_last_error = floppy_error_success;
}

void floppy_init()
{
    // Detect floppy drive(s)
    outb(0x70, 0x10);
    floppy_drives = inb(0x71);

    if (floppy_drive_exists())
    {
        irq_install_handler(floppy_irq, floppy_irq_handler);
        mt_create_thread(mt_kernel_process, floppy_timer, 1);   // Start thread to manage floppy motor
        mt_create_thread(mt_kernel_process, floppy_driver, 1);  // Start thread to manage floppy commands
    }
}
