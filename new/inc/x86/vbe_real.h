/*
 * File:   vbe_real.h
 * Author: Diggory
 *
 * Created on 05 November 2010, 12:02
 */

#ifndef VBE_REAL_H
#define	VBE_REAL_H

enum vbe_result {
    vbe_result_success = 0,
    vbe_result_failed = 1,
    vbe_result_not_supported = 2,
    vbe_result_invalid_call = 3
};

struct vbe_info_block {
    char            vbe_signature[4];
    unsigned short  vbe_version;
    char*           vbe_oem_string;
    unsigned int    capabilities;
    unsigned short* video_mode_ptr;
    unsigned short  total_memory;

    unsigned short  oem_software_rev;
    char*           oem_vendor_name_ptr;
    char*           oem_product_name_ptr;
    char*           oem_product_rev_ptr;

    unsigned char   reserved[222];

    unsigned char   oem_data[256];
} __attribute__((packed));

struct vbe_mode_info_block
{
  /* Mandatory information for all VBE revisions.  */
  unsigned short mode_attributes;
  unsigned char win_a_attributes;
  unsigned char win_b_attributes;
  unsigned short win_granularity;
  unsigned short win_size;
  unsigned short win_a_segment;
  unsigned short win_b_segment;
  void* win_func_ptr;
  unsigned short bytes_per_scan_line;

  /* Mandatory information for VBE 1.2 and above.  */
  unsigned short x_resolution;
  unsigned short y_resolution;
  unsigned char x_char_size;
  unsigned char y_char_size;
  unsigned char number_of_planes;
  unsigned char bits_per_pixel;
  unsigned char number_of_banks;
  unsigned char memory_model;
  unsigned char bank_size;
  unsigned char number_of_image_pages;
  unsigned char reserved;

  /* Direct Color fields (required for direct/6 and YUV/7 memory models).  */
  unsigned char red_mask_size;
  unsigned char red_field_position;
  unsigned char green_mask_size;
  unsigned char green_field_position;
  unsigned char blue_mask_size;
  unsigned char blue_field_position;
  unsigned char rsvd_mask_size;
  unsigned char rsvd_field_position;
  unsigned char direct_color_mode_info;

  /* Mandatory information for VBE 2.0 and above.  */
  unsigned int phys_base_addr;
  unsigned int reserved2;
  unsigned short reserved3;

  /* Mandatory information for VBE 3.0 and above.  */
  unsigned short lin_bytes_per_scan_line;
  unsigned char bnk_number_of_image_pages;
  unsigned char lin_number_of_image_pages;
  unsigned char lin_red_mask_size;
  unsigned char lin_red_field_position;
  unsigned char lin_green_mask_size;
  unsigned char lin_green_field_position;
  unsigned char lin_blue_mask_size;
  unsigned char lin_blue_field_position;
  unsigned char lin_rsvd_mask_size;
  unsigned char lin_rsvd_field_position;
  unsigned int max_pixel_clock;

  /* Reserved field to make structure to be 256 bytes long, VESA BIOS
     Extension 3.0 Specification says to reserve 189 bytes here but
     that doesn't make structure to be 256 bytes.  So additional one is
     added here.  */
  unsigned char reserved4[189 + 1];
} __attribute__ ((packed));

struct vbe_crtc_info_block
{
    unsigned short horizontal_total;
    unsigned short horizontal_sync_start;
    unsigned short horizontal_sync_end;
    unsigned short vertical_total;
    unsigned short vertical_sync_start;
    unsigned short vertical_sync_end;
    unsigned char flags;
    unsigned int pixel_clock;
    unsigned short refresh_rate;
    unsigned char reserved[40];
} __attribute__ ((packed));

enum vbe_result vbe_get_controller_info(struct vbe_info_block* info);
enum vbe_result vbe_get_mode_info(int mode, struct vbe_mode_info_block* mode_info);
enum vbe_result vbe_set_mode(int mode, struct vbe_crtc_info_block* crtc_info);
enum vbe_result vbe_get_mode(int* mode);

#endif	/* VBE_REAL_H */

