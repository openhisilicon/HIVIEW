#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>
#include <mntent.h>
#include <libgen.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "scan_block_device.h"

#define MAX_DIR_PATH 				1024//4096
#define DENT_NAME_LEN				64
#define MAX_RECUR_DEPTH				32
#define SYSFS_BLOCKDEVICE			"/sys/block"

#define ARRAY_SIZE(arr) (sizeof(arr)/sizeof(arr[0]))
#ifndef NDEBUG
#define ERROR(...) fprintf(stderr, "__ERROR__ At FILE:%s LINE:%d\n", __FILE__, __LINE__)
#define ERROR_STR(x, args...) fprintf(stderr, "__ERROR__ At FILE:%s LINE:%d "x"\n", __FILE__, __LINE__, ##args)
#else
#define ERROR(...)
#define ERROR_STR(x)
#endif

static char *bd_ignore_features_table[] =
{
	"loop", "ram", "fd", "mtd", "sr", "rom"
};


struct dentry
{
	char name[DENT_NAME_LEN];
	char dir[MAX_DIR_PATH];
	unsigned int flags;
	struct dentry *next;
};


struct detries
{
	struct dentry *all;
	void *u;
	disk_func call_me;
	unsigned int mask;
	unsigned int stop;
};

static const char *sysfs_mountpoint = NULL;


static __inline__ void*
__alloc(unsigned int size)
{
	return malloc(size);
}


static __inline__ void
__dealloc(void *ptr, unsigned int size)
{
	free(ptr);
}


static __inline__ int
string_match_prefix(const char *str,
	const char *prefix)
{
	return strstr((char*)str, (char*)prefix) == str;
}


static __inline__ void
free_dentries(struct detries *dents)
{
	struct dentry *d;

	while (dents->all)
	{
		d = dents->all;
		dents->all = d->next;
		__dealloc(d, sizeof(*d));
	}
}


static __inline__ int
get_sub_dentry(const char* parent, struct detries *dents)
{
	struct dentry *dent;
	DIR *dir;
	struct dirent *d;
	
	if (parent[0] != '/' || !dents)
	{
		ERROR();
		return -EINVAL;
	}

	dir = opendir(parent);
	if (!dir)
	{
		ERROR_STR("opendir()");
		return -errno;
	}

	while ((d = readdir(dir)))
	{
		if (d->d_name[0] == '.')
		{
			continue;
		}

		dent = (struct dentry*)__alloc(sizeof(struct dentry));
		if (!dent)
		{
			ERROR_STR("__new()");
			closedir(dir);
			return -ENOMEM;
		}

		strncpy(dent->name, d->d_name, DENT_NAME_LEN - 1);
		dent->name[DENT_NAME_LEN - 1] = 0;
		snprintf(dent->dir, MAX_DIR_PATH, "%s/%s", parent, dent->name);
		dent->flags = 0;

		dent->next = dents->all;
		dents->all = dent;
	}

	closedir(dir);
	return 0;
}


static __inline__ int
init_sysfs_mp( void )
{
	sysfs_mountpoint = SYSFS_BLOCKDEVICE;
	if (access(sysfs_mountpoint, X_OK))
	{
		sysfs_mountpoint = NULL;
		return -errno;
	}

	return 0;
}


static __inline__ int
get_all_block_dentry(struct detries *bd)
{
	int err;

	err = init_sysfs_mp();
	if (err)
	{
		ERROR_STR("init_sysfs_mp()");
		return err;
	}

	err = get_sub_dentry(sysfs_mountpoint, bd);
	if (err)
	{
		ERROR_STR("get_sub_dentry()");
		return err;
	}

	return 0;
}


static __inline__ void
visit_each_dentry(struct detries *bd,
	void (*f)(struct dentry *, void *), void *u)
{
	struct dentry **pd = &bd->all;

	while (*pd)
	{
		f(*pd, u);
		pd = &((*pd)->next);
	}
}


static __inline__ void
test_dentry_ignored(struct dentry *d, void *u)
{
	int ign = 0;

	for (; ign < ARRAY_SIZE(bd_ignore_features_table); ++ign)
	{
		if (string_match_prefix(d->name, bd_ignore_features_table[ign]))
		{
			d->flags |= DENT_FLGS_IGNORE;
			break;
		}
	}
}


static __inline__ int
tag_ignored_dentry(struct detries *bd)
{
	visit_each_dentry(bd, test_dentry_ignored, NULL);
	return 0;
}


static __inline__ void
__set_dentry_slave(struct dentry *d, void *u)
{
	if (string_match_prefix((char*)u, d->name))
	{
		d->flags |= DENT_FLGS_SLAVE;
	}
}


static __inline__ void
set_dentry_slave(struct detries *bd, const char *name)
{
	visit_each_dentry(bd, __set_dentry_slave, (void*)name);
}


static __inline__ void
__set_dentry_reserved(struct dentry *d, void *u)
{
	if (string_match_prefix((char*)u, d->name))
	{
		d->flags |= DENT_FLGS_OSUSED;
	}
}


static __inline__ void
set_dentry_reserved(struct detries *bd, const char *name)
{
	visit_each_dentry(bd, __set_dentry_reserved, (void*)name);
}


static __inline__ int
traverse_slaves_tree(const char *bd_dir, struct detries *bd)
{
	static int recur_depth = 0;
	char slaves_dir[MAX_DIR_PATH];
	DIR *dir;
	struct dirent *d;
	int err = 0;

	if (recur_depth >= MAX_RECUR_DEPTH)
	{
		ERROR();
		return -ENOMEM;
	}

	snprintf(slaves_dir, MAX_DIR_PATH, "%s/slaves", bd_dir);
	dir = opendir(slaves_dir);
	if (!dir)
	{
		ERROR_STR("opendir()");
		return -errno;
	}

	while ((d = readdir(dir)))
	{
		if (d->d_name[0] == '.')
		{
			continue;
		}

		set_dentry_slave(bd, d->d_name);
		snprintf(slaves_dir, MAX_DIR_PATH, "%s/slaves/%s", bd_dir, d->d_name);
		++recur_depth;
		err = traverse_slaves_tree(slaves_dir, bd);
		--recur_depth;
		if (err)
		{
			ERROR_STR("traverse_slaves_tree()");
			break;
		}
	}

	closedir(dir);
	return err;
}


static __inline__ void
traverse_one_bd_tree(struct dentry *d, void *u)
{
	struct detries *bd;
	int err;

	bd = (struct detries *)u;
	err = traverse_slaves_tree(d->dir, bd);
	if (err)
	{
		ERROR_STR("traverse_slaves_tree()");
	}
}


static __inline__ int
tag_slaves_dentry(struct detries *bd)
{
	visit_each_dentry(bd, traverse_one_bd_tree, (void*)bd);
	return 0;
}


static __inline__ int
get_devname_by_uuid(const char *uuid, char name[], int size)
{
	char uuid_path[MAX_DIR_PATH];
	char base_name[DENT_NAME_LEN];
	int err;

	snprintf(uuid_path, MAX_DIR_PATH, "/dev/disk/by-uuid/%s", uuid);
	err = readlink(uuid_path, uuid_path, sizeof(uuid_path));
	if (err < 0)
	{
		ERROR_STR("readlink()");
		err= -errno;
	}
	else
	{
		uuid_path[err] = 0;
		strncpy(base_name, basename(uuid_path), DENT_NAME_LEN - 1);
		base_name[DENT_NAME_LEN - 1] = 0;

		if (strlen(base_name) < size)
		{
			strncpy(name, base_name, size - 1);
			name[size - 1] = 0;
			err = 0;
		}
		else
		{
			ERROR();
			err = -ENAMETOOLONG;
		}
	}

	return err;
}


static __inline__ int
find_real_device(struct detries *bd, const char *dev_name)
{
	char real_name[DENT_NAME_LEN];
	char real_path[MAX_DIR_PATH];
	int err;
	struct stat st;

	if (string_match_prefix(dev_name, "UUID="))
	{
		err = get_devname_by_uuid(dev_name + 5, real_name, DENT_NAME_LEN);
		if (!err)
		{
//			printf("==> %s (from UUID)\n", real_name);
			set_dentry_reserved(bd, real_name);
		}
		return err;
	}

	err = lstat(dev_name, &st);
	if (err < 0)
	{
		ERROR_STR("lstat(%s)",dev_name);
		return -errno;
	}

	if (S_ISLNK(st.st_mode))
	{
		err = readlink(dev_name, real_path, sizeof(real_path));
		if (err < 0)
		{
			ERROR_STR("readlink()");
			return -errno;
		}

		real_path[err] = 0;
		strncpy(real_name, basename(real_path), DENT_NAME_LEN - 1);
		real_name[DENT_NAME_LEN - 1] = 0;

//		printf("==> %s (from mapper)\n", real_name);
		set_dentry_reserved(bd, real_name);
		return 0;
	}

	strncpy(real_name, basename((char*)dev_name), DENT_NAME_LEN - 1);
	real_name[DENT_NAME_LEN - 1] = 0;
//	printf("==> %s\n", real_name);
	set_dentry_reserved(bd, real_name);
	return 0;
}


static __inline__ int
find_system_mount_devices(struct detries *bd, const char *fstab)
{
	struct mntent mtent;
    FILE *fp;
    char mntent_buf[MAX_DIR_PATH];

	fp = setmntent(fstab, "r");
	if (!fp)
	{
		ERROR_STR("setmntent()");
		return -errno;
	}

    while (getmntent_r(fp, &mtent, mntent_buf, sizeof(mntent_buf)))
    {
        //printf("[%s   %s   %s]\n", mtent.mnt_fsname, mtent.mnt_type, mtent.mnt_dir);
    	
    	if ( string_match_prefix(mtent.mnt_fsname, "/dev") ||
    		!strcmp(mtent.mnt_type, "swap") ||
    		!strcmp(mtent.mnt_dir, "/") ||
    		!strcmp(mtent.mnt_dir, "/boot") ||
    		!strcmp(mtent.mnt_dir, "/home"))
    	{
    		find_real_device(bd, mtent.mnt_fsname);
    	}
    }

    endmntent(fp);
    return 0;
}


static __inline__ int
tag_linux_os_dentry(struct detries *bd)
{
	find_system_mount_devices(bd, "/etc/fstab");
	find_system_mount_devices(bd, "/etc/mtab");
	return 0;
}


static __inline__ int
find_usefull_entries(struct detries *bd)
{
	int err;

	err = get_all_block_dentry(bd);
	if (err)
	{
		ERROR_STR("get_all_block_dentry()");
		return err;		
	}

	err = tag_ignored_dentry(bd);
	if (err)
	{
		ERROR_STR("tag_slaves_dentry()");
		return err;
	}

	err = tag_slaves_dentry(bd);
	if (err)
	{
		ERROR_STR("tag_slaves_dentry()");
		return err;
	}

	err = tag_linux_os_dentry(bd);
	if (err)
	{
		ERROR_STR("tag_linux_os_dentry()");
		return err;
	}

	return 0;
}


static __inline__ void
foreach_block_device(struct dentry *d, void *u)
{
	struct detries *bd = (struct detries*)u;
	int err;

	if (!bd->stop && bd->call_me && !(bd->mask & d->flags))
	{
		err = (*bd->call_me)(d->name, bd->u);
		if (err)
		{
			bd->stop = 1;
		}
	}
}


int scan_block_device(unsigned mask, disk_func f, void *u)
{
	struct detries d;
	int err;

	d.all = NULL;
	d.u = u;
	d.call_me = f;
	d.mask = mask;
	d.stop = 0;

	err = find_usefull_entries(&d);
	if (err)
	{
		ERROR_STR("find_usefull_entries()");
		free_dentries(&d);
		return err;
	}

	visit_each_dentry(&d, foreach_block_device, &d);
	free_dentries(&d);
	return 0;
}


#ifdef __FIND_BLOCK_DEVICE_TEST__

int print_disk(char *name, void *u)
{
	printf("##==> %s\n", name);
	return 0;
}

int main()
{
	int err;

	err = find_block_device(DENT_ALL_UNUSED, print_disk, NULL);
	if (err)
	{
		ERROR_STR("find_block_device()");
		return err;
	}

	return 0;
}


#endif


//:~ End
