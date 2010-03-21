#ifndef REAL_H_INCLUDED
#define REAL_H_INCLUDED

extern void real_init();
extern unsigned int (*real_call)(unsigned short id, ...);

enum real_command_id
{
    real_get_vbe_info_id = 0,
    real_get_vbe_mode_info_id,
    real_set_vbe_mode_id
};

#endif // REAL_H_INCLUDED
