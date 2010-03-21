#ifndef VESA_H_INCLUDED
#define VESA_H_INCLUDED

struct vesa_vbe_info
{
    char signature[4];
    unsigned short version;
    char* oem_string;
    unsigned char capabilities[4];
    unsigned short* video_modes;
    unsigned short total_memory;
    unsigned short oem_software_revision;
    char* oem_vendor_name;
    char* oem_product_name;
    char* oem_product_revision;
    unsigned char reserved[222];
    unsigned char oem_data[256];
}  __attribute__((packed));

struct vesa_vbe_mode_info {
  unsigned short attributes;
  unsigned char winA,winB;
  unsigned short granularity;
  unsigned short winsize;
  unsigned short segmentA, segmentB;
  unsigned short realFctPtr[2];
  unsigned short pitch; // bytes per scanline

  unsigned short Xres, Yres;
  unsigned char Wchar, Ychar, planes, bpp, banks;
  unsigned char memory_model, bank_size, image_pages;
  unsigned char reserved0;

  unsigned char red_mask, red_position;
  unsigned char green_mask, green_position;
  unsigned char blue_mask, blue_position;
  unsigned char rsv_mask, rsv_position;
  unsigned char directcolor_attributes;

  unsigned int physbase;  // your LFB address ;)
  unsigned int reserved1;
  unsigned short reserved2;
}  __attribute__((packed));

extern void vesa_init();
extern struct vesa_vbe_info* vesa_get_vbe_info();
extern struct vesa_vbe_mode_info* vesa_get_vbe_mode_info(unsigned short mode);
extern int vesa_set_vbe_mode(unsigned short mode);

#endif // VESA_H_INCLUDED
