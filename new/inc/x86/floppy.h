/* 
 * File:   floppy.h
 * Author: Diggory
 *
 * Created on 22 November 2010, 22:38
 */

#ifndef FLOPPY_H
#define	FLOPPY_H

#include "x86/syscall.h"

enum floppy_err_code {
    floppy_err_none = 0,
    floppy_err_invalidop,
    floppy_err_trackerror,
    floppy_err_invalidcall,
    floppy_err_drivenotready,
    floppy_err_endofcylinder,
    floppy_err_crcerror,
    floppy_err_controllertimeout,
    floppy_err_nodatafound,
    floppy_err_noaddressmarkfound,
    floppy_err_deletedaddressmark,
    floppy_err_crcerrorindata,
    floppy_err_wrongcylinder,
    floppy_err_sectornotfound,
    floppy_err_badcylinder,
    floppy_err_invalidsectorsize,
    floppy_err_notwritable,
    floppy_err_invalidbufferaddress
};

enum floppy_reg
{
    floppy_reg_status_a = 0x0, // read-only
    floppy_reg_status_b = 0x1, // read-only
    floppy_reg_digital_output = 0x2,
    floppy_reg_tape_drive = 0x3,
    floppy_reg_main_status = 0x4, // read-only
    floppy_reg_datarate_select = 0x4, // write-only
    floppy_reg_data_fifo = 0x5,
    floppy_reg_digital_input = 0x7, // read-only
    floppy_reg_config_control = 0x7 // write-only
};

enum floppy_cmd
{
    floppy_cmd_read_track = 2, // generates irq6
    floppy_cmd_specify = 3, // * set drive parameters
    floppy_cmd_sense_drive_status = 4,
    floppy_cmd_write_data = 5, // * write to the disk
    floppy_cmd_read_data = 6, // * read from the disk
    floppy_cmd_recalibrate = 7, // * seek to cylinder 0
    floppy_cmd_sense_interrupt = 8, // * ack irq6, get status of last command
    floppy_cmd_write_deleted_data = 9,
    floppy_cmd_read_id = 10, // generates irq6
    floppy_cmd_read_deleted_data = 12,
    floppy_cmd_format_track = 13, // *
    floppy_cmd_seek = 15, // * seek both heads to cylinder x
    floppy_cmd_version = 16, // * used during initialization, once
    floppy_cmd_scan_equal = 17,
    floppy_cmd_perpendicular_mode = 18, // * used during initialization, once, maybe
    floppy_cmd_configure = 19, // * set controller parameters
    floppy_cmd_lock = 20, // * protect controller params from a reset
    floppy_cmd_verify = 22,
    floppy_cmd_scan_low_or_equal = 25,
    floppy_cmd_scan_high_or_equal = 29
};

enum { floppy_motor_off = 0, floppy_motor_on, floppy_motor_wait };

// Used by floppy_dma_init and floppy_do_track to specify direction
enum floppy_dir {
    floppy_dir_read = 1,
    floppy_dir_write = 2
};


// we statically reserve a totally uncomprehensive amount of memory
// must be large enough for whatever DMA transfer we might desire
// and must not cross 64k borders so easiest thing is to align it
// to 2^N boundary at least as big as the block
#define floppy_dmalen 0x4800
#define floppy_max 2

extern unsigned char floppy_dmabuf[floppy_dmalen] __attribute__((aligned(0x8000)));
extern unsigned int floppy_base[floppy_max];
extern unsigned floppy_current_track[floppy_max];

void floppy_init();
err_t floppy_write_cmd(int index, char cmd);
unsigned char floppy_read_data(int index);
err_t floppy_check_interrupt(int index, int *st0, int *cyl);
err_t floppy_calibrate(int index);
err_t floppy_motor(int index, int onoff);
err_t floppy_motor_kill(int index);
void floppy_timer(void* data);
err_t floppy_seek(int index, unsigned cyli, int head);
err_t floppy_dma_init(enum floppy_dir dir);
err_t floppy_do_track(int index, unsigned cyl, enum floppy_dir dir);

// Safe to use
SYSCALL_DECLARE(floppy_reset)(int index);
SYSCALL_DECLARE(floppy_read_track)(int index, unsigned cyl);
SYSCALL_DECLARE(floppy_write_track)(int index, unsigned cyl);
SYSCALL_DECLARE(floppy_lock_buffer)(unsigned char** buffer);
SYSCALL_DECLARE(floppy_unlock_buffer)();

#endif	/* FLOPPY_H */

