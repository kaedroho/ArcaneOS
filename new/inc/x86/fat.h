/* 
 * File:   fat.h
 * Author: Diggory
 *
 * Created on 26 November 2010, 16:58
 */

#ifndef FAT_H
#define	FAT_H

struct fat_boot_record {
    unsigned char jmp_boot[3];
    char oem_name[8];
    unsigned short bytes_per_sector;
    unsigned char sectors_per_cluster;
    unsigned short reserved_sector_count;
    unsigned char num_fats;
    unsigned short root_entry_count;
    unsigned short total_sectors_16;
    unsigned char media;
    unsigned short fat_size_16;
    unsigned short sectors_per_track;
    unsigned short num_heads;
    unsigned int hidden_sector_count;
    unsigned int total_sectors_32;
} __attribute__((packed));

struct fat_boot_record_12_16 {
    struct fat_boot_record common;
    unsigned char drive_num;
    unsigned char reserved1;
    unsigned char boot_signature;
    unsigned int volume_id;
    char volume_label[11];
    char file_system_type[8];
} __attribute__((packed));

struct fat_boot_record_32 {
    struct fat_boot_record common;
    unsigned int fat_size_32;
    unsigned short extended_flags;
    unsigned short fs_version;
    unsigned int root_cluster;
    unsigned short fs_info;
    unsigned short backup_boot_cluster;
    unsigned char reserved[12];
    unsigned char drive_num;
    unsigned char reserved1;
    unsigned char boot_signature;
    unsigned int volume_id;
    char volume_label[11];
    char file_system_type[8];
} __attribute__((packed));

#endif	/* FAT_H */

