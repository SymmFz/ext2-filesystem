#ifndef _NBTRFS_H_
#define _NBTRFS_H_

#define FUSE_USE_VERSION 26
#include "stdio.h"
#include "stdlib.h"
#include <unistd.h>
#include "fcntl.h"
#include "string.h"
#include "fuse.h"
#include <stddef.h>
#include "ddriver.h"
#include "errno.h"
#include "types.h"
#include "stdint.h"

#define NBTRFS_MAGIC                  /* TODO: Define by yourself */
#define NBTRFS_DEFAULT_PERM    0777   /* 全权限打开 */

/******************************************************************************
* SECTION: nbtrfs.c
*******************************************************************************/
void* 			   nbtrfs_init(struct fuse_conn_info *);
void  			   nbtrfs_destroy(void *);
int   			   nbtrfs_mkdir(const char *, mode_t);
int   			   nbtrfs_getattr(const char *, struct stat *);
int   			   nbtrfs_readdir(const char *, void *, fuse_fill_dir_t, off_t,
						                struct fuse_file_info *);
int   			   nbtrfs_mknod(const char *, mode_t, dev_t);
int   			   nbtrfs_write(const char *, const char *, size_t, off_t,
					                  struct fuse_file_info *);
int   			   nbtrfs_read(const char *, char *, size_t, off_t,
					                 struct fuse_file_info *);
int   			   nbtrfs_access(const char *, int);
int   			   nbtrfs_unlink(const char *);
int   			   nbtrfs_rmdir(const char *);
int   			   nbtrfs_rename(const char *, const char *);
int   			   nbtrfs_utimens(const char *, const struct timespec tv[2]);
int   			   nbtrfs_truncate(const char *, off_t);
			
int   			   nbtrfs_open(const char *, struct fuse_file_info *);
int   			   nbtrfs_opendir(const char *, struct fuse_file_info *);

#endif  /* _nbtrfs_H_ */