#ifndef FS_H
#define FS_H

#include <stdint.h>

#define FS_FILE         0x01
#define FS_DIRECTORY    0x02
#define FS_CHARDEVICE   0x03
#define FS_BLOCKDEVICE  0x04
#define FS_PIPE         0x05
#define FS_SYMLINK      0x06
#define FS_MOUNTPOINT   0x08 
#define FS_STATIC       0x10  // RAM Disk file (tar)
#define FS_DYNAMIC      0x20  // Heap file (kmalloc)

struct fs_node;

typedef uint32_t (*read_type_t)(struct fs_node*, uint32_t, uint32_t, uint8_t*);
typedef uint32_t (*write_type_t)(struct fs_node*, uint32_t, uint32_t, uint8_t*);
typedef void (*open_type_t)(struct fs_node*);
typedef void (*close_type_t)(struct fs_node*);
typedef struct dirent * (*readdir_type_t)(struct fs_node*, uint32_t);
typedef struct fs_node * (*finddir_type_t)(struct fs_node*, char *name);

// Structure that represents a file in the filesystem
typedef struct fs_node {
    char name[128];     // Name of the file
    uint32_t mask;      // Permissions (read/write/execute)
    uint32_t uid;       // User ID
    uint32_t gid;       // Group ID
    uint32_t flags;     // Type (File, Directory, etc.)
    uint32_t inode;     // Identifier
    uint32_t length;    // Dimensions in bytes
    uint32_t impl;      // Implementation-specific data 

    read_type_t read;
    write_type_t write;
    open_type_t open;
    close_type_t close;
    readdir_type_t readdir;
    finddir_type_t finddir;
    
    struct fs_node *ptr;
} fs_node_t;

// Structure that represents a directory entry
struct dirent {
    char name[128];
    uint32_t ino;
};

// Root of the filesystem
extern fs_node_t *fs_root; 

uint32_t read_fs(fs_node_t *node, uint32_t offset, uint32_t size, uint8_t *buffer);
uint32_t write_fs(fs_node_t *node, uint32_t offset, uint32_t size, uint8_t *buffer);
void open_fs(fs_node_t *node, uint8_t read, uint8_t write);
void close_fs(fs_node_t *node);
struct dirent *readdir_fs(fs_node_t *node, uint32_t index);
fs_node_t *finddir_fs(fs_node_t *node, char *name);

#endif