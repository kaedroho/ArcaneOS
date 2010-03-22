#ifndef FLOPPY_H_INCLUDED
#define FLOPPY_H_INCLUDED

#define floppy_dmalen 0x4800
#define floppy_queue_size 32
extern unsigned char floppy_dmabuf[floppy_dmalen];

extern int floppy_base;
// standard IRQ number for floppy controllers
extern int floppy_irq;

enum {
    floppy_command_read,
    floppy_command_write,
    floppy_command_flush
};

struct floppy_command
{
    unsigned int command;
    unsigned int cylinder;
    unsigned int head;
    unsigned int sector;
    unsigned int offset;
    unsigned int size;
    unsigned char* buffer;
    unsigned int* error;
};

extern struct floppy_command floppy_command_queue[floppy_queue_size];
extern unsigned int floppy_command_queue_front;
extern unsigned int floppy_command_queue_back;

// The registers of interest. There are more, but we only use these here.
enum floppy_registers {
   floppy_dor  = 2,  // digital output register
   floppy_msr  = 4,  // master status register, read only
   floppy_fifo = 5,  // data FIFO, in DMA operation for commands
   floppy_ccr  = 7  // configuration control register, write only
};

// The commands of interest. There are more, but we only use these here.
enum floppy_commands {
   floppy_cmd_specify = 3,            // specify
   floppy_cmd_write_data = 5,         // write data
   floppy_cmd_read_data = 6,          // read data
   floppy_cmd_recalibrate = 7,        // recalibrate
   floppy_cmd_sense_interrupt = 8,    // sense interrupt
   floppy_cmd_seek = 15,              // seek
};

enum floppy_error {
    floppy_error_success = 0,
    floppy_error_timeout,
    floppy_error_failed,
    floppy_error_invalid,
    floppy_error_drive,
    floppy_error_dma_buffer,
    floppy_error_dma_direction,
    floppy_error_end_of_cylinder,
    floppy_error_crc,
    floppy_error_no_data,
    floppy_error_no_address_mark,
    floppy_error_deleted_address_mark,
    floppy_error_wrong_cylinder,
    floppy_error_sector_not_found,
    floppy_error_bad_cylinder,
    floppy_error_sector_size,
    floppy_error_not_writable,
    floppy_error_full_queue,
    floppy_error_queueing
};

enum
{
    floppy_motor_off = 0,
    floppy_motor_on,
    floppy_motor_wait
};

typedef enum {
    floppy_dir_read = 1,
    floppy_dir_write = 2
} floppy_dir;

extern void floppy_init();
extern int floppy_get_last_error();
extern void floppy_check_disk_change(int base);
extern void floppy_write_cmd(int base, char cmd);
extern unsigned char floppy_read_data(int base);
extern void floppy_check_interrupt(int base, int *st0, int *cyl);
extern void floppy_wait_irq();
extern void floppy_try_calibrate(int base);
extern void floppy_calibrate(int base);
extern void floppy_reset(int base);
extern void floppy_motor(int base, int onoff);
extern void floppy_motor_kill(int base);
extern void floppy_try_seek(int base, unsigned cyli, int head);
extern void floppy_seek(int base, unsigned cyli, int head);
extern void floppy_timer();
extern void floppy_driver();
extern void floppy_push_command(unsigned int id, unsigned int cylinder, unsigned int head, unsigned int sector, unsigned int offset, unsigned int size, const unsigned char* buffer, unsigned int* error);
extern void floppy_dma_init(floppy_dir dir);
extern void floppy_try_do_track(int base, unsigned cyl, floppy_dir dir, unsigned char cmd);
extern void floppy_do_track(int base, unsigned cyl, floppy_dir dir);
extern void floppy_read_track(unsigned cyl);
extern void floppy_write_track(unsigned cyl);
extern int floppy_drive_exists();
extern void floppy_prepare(unsigned int cylinder);
extern void floppy_read(unsigned int cylinder, unsigned int head, unsigned int sector, unsigned int offset, unsigned int size, unsigned char* buffer);
extern void floppy_write(unsigned int cylinder, unsigned int head, unsigned int sector, unsigned int offset, unsigned int size, const unsigned char* buffer);
extern void floppy_flush();

#endif // FLOPPY_H_INCLUDED
