#ifndef _TYPES_H_
#define _TYPES_H_

/******************************************************************************
* SECTION: Type Definitions
*******************************************************************************/
typedef int          boolean;
typedef uint16_t     flag16;

typedef enum nbtrfs_file_type {
    NBTRFS_REG_FILE = 1,
    NBTRFS_DIR      = 2,
    NBTRFS_SYM_LINK = 3 
} NBTRFS_FILE_TYPE;

/******************************************************************************
* SECTION: Macro
*******************************************************************************/
#define TRUE                    1
#define FALSE                   0
#define UINT32_BITS             32
#define UINT8_BITS              8

#define NBTRFS_BLK_SZ           1024
#define NBTRFS_DRIVER_IO_SZ     512
#define NBTRFS_MAX_FILE_NAME    120
#define NBTRFS_ROOT_INO         1           // 根目录Inode编号

// Disk
#define NBTRFS_DISK_SZ          (4 * 1024 * 1024)
#define NBTRFS_DISK_BLKS        (NBTRFS_DISK_SZ / NBTRFS_BLK_SZ)

// Layout
#define NBTRFS_SUPER_BLKS       1 
#define NBTRFS_IMAP_BLKS        1 
#define NBTRFS_DMAP_BLKS        1 
#define NBTRFS_INODE_TBL_BLKS   64
#define NBTRFS_MAX_INO          (NBTRFS_INODE_TBL_BLKS * (NBTRFS_BLK_SZ / sizeof(struct nbtrfs_inode_d)))
// inode.block_pointer 的个数，动态获取，不一定是6
#define NBTRFS_MAX_FILE_SIZE    

#define NBTRFS_SUPER_BLK_ID     0
#define NBTRFS_IMAP_BLK_ID      (NBTRFS_SUPER_BLK_ID + NBTRFS_SUPER_BLKS)
#define NBTRFS_DMAP_BLK_ID      (NBTRFS_IMAP_BLK_ID + NBTRFS_IMAP_BLKS)
#define NBTRFS_INODE_BLK_ID     (NBTRFS_DMAP_BLK_ID + NBTRFS_DMAP_BLKS)
#define NBTRFS_DATA_BLK_ID      (NBTRFS_INODE_BLK_ID + NBTRFS_INODE_TBL_BLKS)

// Error Codes
#define NBTRFS_ERROR_NONE          0
#define NBTRFS_ERROR_ACCESS        EACCES
#define NBTRFS_ERROR_SEEK          ESPIPE     
#define NBTRFS_ERROR_ISDIR         EISDIR
#define NBTRFS_ERROR_NOSPACE       ENOSPC
#define NBTRFS_ERROR_EXISTS        EEXIST
#define NBTRFS_ERROR_NOTFOUND      ENOENT
#define NBTRFS_ERROR_UNSUPPORTED   ENXIO
#define NBTRFS_ERROR_IO            EIO     /* Error Input/Output */
#define NBTRFS_ERROR_INVAL         EINVAL  /* Invalid Args */

/******************************************************************************
* SECTION: Macro Function
*******************************************************************************/
#define NBTRFS_IO_SZ()          (nbtrfs_super.sz_io)
#define NBTRFS_DISK_SZ()        (nbtrfs_super.sz_disk)
#define NBTRFS_DRIVER()         (nbtrfs_super.driver_fd)

#define NBTRFS_ROUND_DOWN(value, round) ((value) % (round) == 0 ? (value) : ((value) / (round)) * (round))
#define NBTRFS_ROUND_UP(value, round)   ((value) % (round) == 0 ? (value) : ((value) / (round) + 1) * (round))

#define NBTRFS_BLKS_SZ(blks)    ((blks) * NBTRFS_BLK_SZ)

#define NBTRFS_INO_OFS(ino)     (NBTRFS_BLKS_SZ(nbtrfs_super.inode_tbl_offset) + (ino) * sizeof(struct nbtrfs_inode_d))
#define NBTRFS_DATA_OFS(blk)    (NBTRFS_BLKS_SZ(blk))

#define NBTRFS_ASSIGN_FNAME(p_dentry, _fname) \
    do { \
        strncpy(p_dentry->fname, _fname, NBTRFS_MAX_FILE_NAME); \
        p_dentry->fname[NBTRFS_MAX_FILE_NAME - 1] = '\0'; \
    } while(0)

#define NBTRFS_IS_DIR(pinode)   (pinode->ftype == NBTRFS_DIR)
#define NBTRFS_IS_REG(pinode)   (pinode->ftype == NBTRFS_REG_FILE)


/******************************************************************************
* SECTION: On-Disk Structures
*******************************************************************************/
struct nbtrfs_super_d {
    uint32_t magic_num;
    uint32_t sz_usage;

    // 逻辑块信息
    uint32_t blks_size;
    uint32_t blks_nums;

    // layout
    uint32_t imap_blks;          // inode map 索引节点位图
    uint32_t imap_offset;

    uint32_t dmap_blks;          // data map 数据块位图
    uint32_t dmap_offset;

    uint32_t inode_tbl_blks;     // inode table 索引节点表
    uint32_t inode_tbl_offset;

    uint32_t data_area_blks;     // 数据区
    uint32_t data_area_offset;

    // 根目录信息
    uint32_t root_ino;           // 根目录Inode编号，1

    // 限制信息
    uint32_t ino_max;            // 1024
    uint32_t file_max;           // 6 blks, 6 * 1024 Bytes
};

struct nbtrfs_inode_d {     // Size: 64 Bytes
    uint32_t ino;

    uint32_t size;               // 文件已占用空间
    uint32_t link;               // 硬链接数，默认为1
    NBTRFS_FILE_TYPE ftype;      // 文件类型

    uint32_t block_pointer[6];   // 数据块索引 (24 Bytes)

    uint32_t dir_cnt;            // 目录项个数，仅目录有效

    uint8_t  _padding[20];       // Padding to 64 Bytes
};

struct nbtrfs_dentry_d {    // Size: 128 Bytes
    uint32_t ino;                         // 4 Bytes
    char     fname[NBTRFS_MAX_FILE_NAME]; // 120 Bytes
    uint32_t ftype;                       // 4 Bytes
}; 

/******************************************************************************
* SECTION: In-Memory Structures
*******************************************************************************/
struct nbtrfs_dentry;
struct nbtrfs_inode;
struct nbtrfs_super;

struct custom_options {
	const char*        device;
    boolean            show_help;
};

struct nbtrfs_super {
    int driver_fd;
    uint32_t                sz_io;
    uint32_t                sz_disk;
    uint32_t                sz_usage;

    // 布局信息
    uint32_t                imap_offset;
    uint32_t                dmap_offset;
    uint32_t                inode_tbl_offset;
    uint32_t                data_area_offset;

    // Inode 位图和数据位图指针
    uint8_t*                imap_ptr;
    uint8_t*                dmap_ptr;

    // 挂载信息
    boolean                 is_mounted;
    struct nbtrfs_dentry*   root_dentry;
};

struct nbtrfs_inode {
    uint32_t ino;

    uint32_t size;               // 文件已占用空间
    uint32_t link;               // 硬链接数，默认为1
    NBTRFS_FILE_TYPE ftype;      // 文件类型

    uint32_t block_pointer[6];   // 数据块索引 (24 Bytes)

    uint32_t dir_cnt;            // 目录项个数，仅目录有效

    struct nbtrfs_dentry* dentry;   // 该inode对应的dentry链表
    struct nbtrfs_dentry* dentrys;  // 所有目录项
};

struct nbtrfs_dentry {
    uint32_t ino;                         // 4 Bytes
    char     fname[NBTRFS_MAX_FILE_NAME]; // 120 Bytes
    uint32_t ftype;                       // 4 Bytes

    struct nbtrfs_dentry* parent;         // 父目录dentry
    struct nbtrfs_dentry* brother;        // 指向下一个dentry
    struct nbtrfs_inode*  inode;          // 指向的inode
};

#endif /* _TYPES_H_ */
