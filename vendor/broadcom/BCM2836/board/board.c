#include "los_base.h"
#include "los_config.h"
#include "los_process_pri.h"
#include "lwip/init.h"
#include "lwip/tcpip.h"
#include "sys/mount.h"
#include "mtd_partition.h"
#include "console.h"

UINT32 OsRandomStackGuard(VOID)
{
    return 0;
}

static void BCM2836_mount_rootfs()
{
#if 1	
	int fd;
    dprintf("register parition ...\n");
    if (add_mtd_partition("spinor", 0, DDR_RAMFS_PART0_SIZE, 0))
    {
        PRINT_ERR("add_mtd_partition fail\n");
    }
    
    dprintf("mount /dev/spinorblk0 / ...\n");
    //if (mount("/dev/spinorblk0", "/", "jffs2", MS_RDONLY, NULL))
    if (mount("/dev/spinorblk0", "/", "jffs2", 0, NULL))
    {
        PRINT_ERR("mount failed\n");
    }else
    	{PRINT_ERR("mount completed\n");}
	
	fd = open("/bin/init", O_RDONLY);
    dprintf("open /bin/init, fd = %d\n", fd);
#else
    dprintf("mount /dev/ramdisk / ...\n");
    //if (mount("/dev/spinorblk0", "/", "jffs2", MS_RDONLY, NULL))
    if (mount("/dev/ramdisk", "/", "vfat", 0, NULL))
    {
        PRINT_ERR("mount failed\n");
    }
#endif
}

static void BCM2836_driver_init()
{
#if 0	
	extern int my_ramdisk_init(void);
    if (my_ramdisk_init())
    {
        PRINT_ERR("my_ramdisk_init failed\n");
    }

#else	
    extern int spinor_init(void);
    dprintf("spinor_init init ...\n");
    if (!spinor_init())
    {
        PRINT_ERR("spinor_init failed\n");
    }
#endif

#ifdef LOSCFG_DRIVERS_VIDEO
    dprintf("BCM2836_fb_init init ...\n");
	extern int imx6ull_fb_init(void);
    if (imx6ull_fb_init())
    {
        PRINT_ERR("BCM2836_fb_init failed\n");
    }
#endif	

}


void SystemInit()
{
#ifdef LOSCFG_FS_PROC
    dprintf("proc fs init ...\n");
    extern void ProcFsInit(void);
    ProcFsInit();
#endif

#ifdef LOSCFG_DRIVERS_MEM
    dprintf("mem dev init ...\n");
    extern int mem_dev_register(void);
    mem_dev_register();
#endif
    BCM2836_driver_init();
    BCM2836_mount_rootfs();

#ifdef LOSCFG_DRIVERS_HDF
	    extern int DeviceManagerStart(void);
		PRINT_RELEASE("DeviceManagerStart start ...\n");	
		if (DeviceManagerStart()) {
			PRINT_ERR("No drivers need load by hdf manager!");
		}
		dprintf("DeviceManagerStart end ...\n");
#endif

    extern int uart_dev_init(void);
    uart_dev_init();

    if (virtual_serial_init("/dev/uartdev-0") != 0)
    {
        PRINT_ERR("virtual_serial_init failed\n");
    }else
		dprintf("virtual_serial_init completed\n");

    if (system_console_init(SERIAL) != 0)  //  SERIAL = /dev/serial
    {
        PRINT_ERR("system_console_init failed\n");
    }else
		dprintf("system_console_init completed\n");

    if (OsUserInitProcess())
    {
        PRINT_ERR("Create user init process faialed!\n");
    }else
		dprintf("Create user init process completed!\n");
}

