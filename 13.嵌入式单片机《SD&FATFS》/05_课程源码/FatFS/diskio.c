/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2016        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "ff.h"			/* Obtains integer types */
#include "diskio.h"		/* Declarations of disk functions */
#include "mmc_sd.h"

/* Definitions of physical drive number for each drive */
#define DEV_SD	0	


/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber to identify the drive */
)
{
	return 0;
}



/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/
DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber to identify the drive */
)
{
	DSTATUS stat;
	int result;
	stat = stat;
	result = result;
	
	switch (pdrv) {
	case DEV_SD :
		result = SD_Config();
	}
	if(!result)
		return RES_OK;
	return STA_NOINIT;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/
DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber to identify the drive */
	BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,	/* Start sector in LBA */
	UINT count		/* Number of sectors to read */
)
{
	int result;
	switch (pdrv) {
	case DEV_SD :
		result = SD_ReadDisk(buff, sector, count);
	}
	if(!result)
		return RES_OK;
	return RES_PARERR;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if FF_FS_READONLY == 0

DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber to identify the drive */
	const BYTE *buff,	/* Data to be written */
	DWORD sector,		/* Start sector in LBA */
	UINT count			/* Number of sectors to write */
)
{
	int result;

	switch (pdrv) {
	case DEV_SD :
		result = SD_WriteDisk((u8 *)buff, sector, count);
	}
	if(!result)
		return RES_OK;
	return RES_PARERR;
}

#endif

/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/
DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
	u8 result = RES_OK;
    switch (pdrv)
    {
    case DEV_SD :
        switch(cmd)
        {
        case CTRL_SYNC:
            SD_CS(0);
            if(SD_WaitReady())result = RES_ERROR;
            SD_CS(1);
            break;
        case GET_SECTOR_SIZE:
            *(WORD *)buff = 512;
            break;
        case GET_BLOCK_SIZE:
            *(WORD *)buff = 8;
            break;
        case GET_SECTOR_COUNT:
            *(DWORD *)buff = SD_GetSectorCount();
            break;
        default:
            result = RES_PARERR;
            break;
        }
        break;
    }
    if(!result)return RES_OK;

    return RES_PARERR;
}

/****************************
�������ƣ�get_fattime
�������ã�FATFS��ȡʱ��
������������
��������ֵ��ʱ��
�������ߣ�WYC
����ʱ�䣺2020.09.28
�޸�ʱ�䣺2020.09.28
****************************/
DWORD get_fattime (void)
{
    u32 date;
    date =
        (
            ((2015 - 1980) << 25)  |
            (7 <<  21 ) |
            (9 <<  16 ) |
            (12 << 11 ) |
            ( 4 << 5  ) |
            ( 0 )
        );

    return date;
}

/****************************
�������ƣ�ff_memalloc
�������ã�FATFS���ٿռ�
����������
		msize		���ٿռ��С
��������ֵ�����ٿռ��ַ
�������ߣ�WYC
����ʱ�䣺2020.09.28
�޸�ʱ�䣺2020.09.28
****************************/
void *ff_memalloc (UINT msize)
{
    return malloc(msize);
}

/****************************
�������ƣ�ff_memalloc
�������ã�FATFS�ͷſռ�
����������
		mblock		�ͷſռ��ַ
��������ֵ����
�������ߣ�WYC
����ʱ�䣺2020.09.28
�޸�ʱ�䣺2020.09.28
****************************/
void ff_memfree (void *mblock)		/* Free memory block */
{
    free(mblock);
}
