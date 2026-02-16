#include <kernel/fs.h>
#include <kernel/kprint.h>
#include <kernel/utils.h>
#include <kernel/kheap.h>

#include <debug.h>

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

uint32_t octal_to_int(const char *in) {
    uint32_t size = 0;
    unsigned int j;
    unsigned int count = 1;
 
    for (j = 11; j > 0; j--, count *= 8)
        size += ((in[j - 1] - '0') * count);
 
    return size;
}

fs_node_t *tar_root;           
uint32_t tar_start_address;    
fs_node_t *tar_files[128];      
uint32_t tar_files_count = 0;

uint32_t tar_read(fs_node_t *node, uint32_t offset, uint32_t size, uint8_t *buffer) {
    char *data_ptr = (char *)node->impl;
    
    DEBUG_LOG("[KERNEL READ] File: %s, Offset: %d, Size: %d, DataPtr: 0x%x\n", 
           node->name, offset, size, (uint32_t)data_ptr);

    if (offset > node->length) {
        printf("Error: Offset > Length\n");
        return 0;
    }
    
    if (offset + size > node->length) {
        size = node->length - offset;
    }

    for (uint32_t i = 0; i < size; i++) {
        buffer[i] = data_ptr[offset + i];
    }
    
    return size;
}

uint32_t tar_write(fs_node_t *node, uint32_t offset, uint32_t size, uint8_t *buffer) {
    DEBUG_LOG("[KERNEL WRITE] File: %s, Flags: 0x%x\n", node->name, node->flags);

    if (node->flags & FS_STATIC) {
        return 0; 
    }

    char *data_ptr = (char *)node->impl;

    if (offset + size > 1024) {
        size = 1024 - offset;
    }

    for (uint32_t i = 0; i < size; i++) {
        data_ptr[offset + i] = buffer[i];
    }
    
    if (offset + size > node->length) {
        node->length = offset + size;
        DEBUG_LOG("[KERNEL WRITE] New Length: %d\n", node->length);
    }

    return size;
}

fs_node_t *parse_tar(uint32_t address) {
    tar_start_address = address;
    
    struct tar_header *header = (struct tar_header *)address;

    for(int k=0; k<128; k++) tar_files[k] = 0;
    tar_files_count = 0;

    tar_root = (fs_node_t*)kmalloc(sizeof(fs_node_t));
    tar_root->flags = FS_DIRECTORY;
    for(int i=0; i<128; i++) tar_root->name[i] = 0;
    
    DEBUG_LOG("[TAR PARSE] Scanning RAMDisk at 0x%x...\n", address);

    while (header->filename[0] != 0) {
        if (header->filename[0] < 32 || header->filename[0] > 126) {
            DEBUG_LOG("[TAR PARSE] End of valid data reached (Garbage detected).\n");
            break;
        }

        uint32_t size = octal_to_int(header->size);
        
        fs_node_t *node = (fs_node_t*)kmalloc(sizeof(fs_node_t));
        
        int i = 0;
        int offset_name = 0;
        if (header->filename[0] == '.' && header->filename[1] == '/') offset_name = 2;

        while(header->filename[offset_name + i] != 0 && i < 127) {
            node->name[i] = header->filename[offset_name + i];
            i++;
        }
        node->name[i] = 0;

        node->length = size;
        node->flags = FS_FILE | FS_STATIC;
        node->read = &tar_read; 
        node->write = &tar_write; 
        
        node->impl = (uint32_t)((uint32_t)header + 512);

        DEBUG_LOG("[TAR PARSE] Found: %s (Size: %d, DataAddr: 0x%x)\n", node->name, size, node->impl);

        if (tar_files_count < 128) {
            tar_files[tar_files_count++] = node;
        } else {
            printf("Error: Max files limit reached!\n");
            break;
        }

        uint32_t jump = 512 + size;
        if (size % 512 != 0) {
            jump += 512 - (size % 512);
        }
        header = (struct tar_header *)((uint32_t)header + jump);
    }

    DEBUG_LOG("[TAR PARSE] Parsing complete. Total files: %d\n", tar_files_count);
    return tar_root;
}

int tar_create_file(char *name) {
    DEBUG_LOG("[KERNEL CREATE] Request to create: %s\n", name);

    if (tar_files_count >= 128) {
        printf("Error: File list full (%d).\n", tar_files_count);
        return -1; 
    }

    fs_node_t *node = (fs_node_t*)kmalloc(sizeof(fs_node_t));
    if (node == 0) {
        printf("Error: kmalloc failed for Node.\n");
        return -1;
    }
    
    int i = 0;
    while(name[i] != 0 && i < 127) { node->name[i] = name[i]; i++; }
    node->name[i] = 0;

    node->flags = FS_FILE | FS_DYNAMIC;
    node->read = &tar_read;
    node->write = &tar_write;
    
    node->impl = (uint32_t)kmalloc(1024); 
    
    if (node->impl == 0) {
        printf("Error: kmalloc failed for Data Buffer.\n");
        return -1;
    }
    
    char* ptr = (char*)node->impl;
    for(int k=0; k<1024; k++) ptr[k] = 0;

    node->length = 0; 

    tar_files[tar_files_count++] = node;
    
    DEBUG_LOG("[KERNEL CREATE] Success! File created at DataAddr: 0x%x\n", node->impl);
    return 0; 
}

fs_node_t *get_tar_file_by_index(uint32_t index) {
    if (index >= tar_files_count) return 0;
    return tar_files[index];
}

int k_strcmp_tar(const char *s1, const char *s2) {
    while (*s1 && (*s1 == *s2)) { s1++; s2++; }
    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

fs_node_t *get_tar_file_by_name(char *name) {
    for (uint32_t i = 0; i < tar_files_count; i++) {
        if (k_strcmp_tar(tar_files[i]->name, name) == 0) {
            return tar_files[i];
        }
    }
    return 0; 
}

int tar_delete_file(char *name) {
    DEBUG_LOG("[KERNEL DELETE] Request to delete: %s\n", name);

    int idx = -1;
    for (uint32_t i = 0; i < tar_files_count; i++) {
        if (k_strcmp_tar(tar_files[i]->name, name) == 0) {
            idx = (int)i;
            break;
        }
    }

    if (idx == -1) {
        return -1;
    }

    fs_node_t *node = tar_files[idx];

    if (node->flags & FS_STATIC) {
        return -2;
    }

    kfree((void*)node->impl);
    kfree(node);
    
    for (uint32_t i = (uint32_t)idx; i < tar_files_count - 1; i++) {
        tar_files[i] = tar_files[i+1];
    }

    tar_files_count--;
    tar_files[tar_files_count] = 0;

    DEBUG_LOG("[KERNEL DELETE] Success. Files remaining: %d\n", tar_files_count);
    return 0;
}