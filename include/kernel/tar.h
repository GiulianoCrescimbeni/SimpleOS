#include <kernel/fs.h>
#include <kernel/kprint.h>
#include <kernel/utils.h>
#include <kernel/kheap.h>

struct tar_header {
    char filename[100];
    char mode[8];
    char uid[8];
    char gid[8];
    char size[12];
    char mtime[12];
    char chksum[8];
    char typeflag[1];
};

uint32_t octal_to_int(const char *in);
uint32_t tar_read(fs_node_t *node, uint32_t offset, uint32_t size, uint8_t *buffer);
fs_node_t *parse_tar(uint32_t address);
fs_node_t *get_tar_file_by_index(uint32_t index);
fs_node_t *get_tar_file_by_name(char *name);
uint32_t tar_write(fs_node_t *node, uint32_t offset, uint32_t size, uint8_t *buffer);
int tar_create_file(char *name);
int tar_delete_file(char *name);