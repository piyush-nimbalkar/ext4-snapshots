/*
 * linux/fs/ext4/snapshot_debug.h
 *
 * Written by Amir Goldstein <amir73il@users.sf.net>, 2008
 *
 * Copyright (C) 2008-2010 CTERA Networks
 *
 * This file is part of the Linux kernel and is made available under
 * the terms of the GNU General Public License, version 2, or at your
 * option, any later version, incorporated herein by reference.
 *
 * Ext4 snapshot debugging.
 */

#ifndef _LINUX_EXT4_SNAPSHOT_DEBUG_H
#define _LINUX_EXT4_SNAPSHOT_DEBUG_H

#ifdef CONFIG_EXT4_FS_DEBUG
#include <linux/delay.h>

#define SNAPSHOT_INDENT_MAX 4
#define SNAPSHOT_INDENT_STR "\t\t\t\t"
#define KERN_LEVEL_STR "<%d>"
#define SNAP_KERN_LEVEL(n) ((n)+2) /* 1 = KERN_ERR, ..., 5 = KERN_DEBUG */

#define SNAPTEST_TAKE	0
#define SNAPTEST_DELETE	1
#define SNAPTEST_COW	2
#define SNAPTEST_READ	3
#define SNAPTEST_BITMAP	4
#define SNAPSHOT_TESTS_NUM	5

extern const char *snapshot_indent;
extern u8 snapshot_enable_debug;
extern u16 snapshot_enable_test[SNAPSHOT_TESTS_NUM];
extern u8 cow_cache_offset;

#define snapshot_test_delay(i)		     \
	do {							       \
		if (snapshot_enable_test[i])			       \
			msleep_interruptible(snapshot_enable_test[i]); \
	} while (0)

/*
 * Sleep 1ms every 'blocks_per_ms', amounting to the total test delay
 * over 100% of progress (when 'to' reaches 'max').
 * snapshot_enable_test[i] (msec) is limited to 64K and max (blocks_count)
 * is likely much more than 64K, so 'blocks_per_ms' is likely non zero.
 */
#define snapshot_test_delay_progress(i, from, to, max)			\
	do {								\
		if (snapshot_enable_test[i] &&				\
				(max) > snapshot_enable_test[i] &&	\
				(from) <= (to) && (to) <= (max)) {	\
			unsigned long blocks_per_ms =			\
				do_div((max), snapshot_enable_test[i]);	\
			unsigned long x = do_div((from), blocks_per_ms);\
			unsigned long y = do_div((to), blocks_per_ms);	\
			if (y > x)					\
				msleep_interruptible(y - x);		\
		}							\
	} while (0)

#define snapshot_debug_l(n, l, f, a...)					\
	do {								\
		if ((n) <= snapshot_enable_debug &&			\
		    (l) <= SNAPSHOT_INDENT_MAX) {			\
			printk(KERN_LEVEL_STR "snapshot: %s" f,		\
				   SNAP_KERN_LEVEL(n),			\
			       snapshot_indent - (l),			\
				   ## a);				\
		}							\
	} while (0)

#define snapshot_debug(n, f, a...)	snapshot_debug_l(n, 0, f, ## a)

#define snapshot_debug_once(n, f, a...)					\
	do {								\
		static bool __once = false;				\
		if (__once) {						\
			snapshot_debug(n, f, ## a);			\
			__once = true;					\
		}							\
	} while (0)

extern void ext4_snapshot_create_debugfs_entry(struct dentry *debugfs_dir);
extern void ext4_snapshot_remove_debugfs_entry(void);

#else
#define snapshot_test_delay(i)
#define snapshot_test_delay_progress(i, from, to, max)
#define snapshot_debug(n, f, a...)
#define snapshot_debug_l(n, l, f, a...)
#define snapshot_debug_once(n, f, a...)
#define ext4_snapshot_create_debugfs_entry(d)
#define ext4_snapshot_remove_debugfs_entry()
#endif

#ifdef CONFIG_EXT4_FS_SNAPSHOT_CTL_DUMP
#ifdef CONFIG_EXT4_FS_DEBUG
extern void ext4_snapshot_dump(int n, struct inode *inode);
#else
#define ext4_snapshot_dump(n, i)
#endif
#endif

/* debug levels */
#define SNAP_ERR	1 /* errors and summary */
#define SNAP_WARN	2 /* warnings */
#define SNAP_INFO	3 /* info */
#define SNAP_DEBUG	4 /* debug */
#define SNAP_DUMP	5 /* dump snapshot file */

#endif	/* _LINUX_EXT4_SNAPSHOT_DEBUG_H */
