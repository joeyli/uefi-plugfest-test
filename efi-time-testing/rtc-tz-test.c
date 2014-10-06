/*
 * Timezone of ACPI/EFI RTC Driver Test/Example Program
 *
 * Compile with:
 *	gcc rtc-tz-test.c -o rtc-tz-test
 *
 * Copyright (C) 2013 SUSE Linux Products GmbH. All rights reserved.
 * Written by Lee, Chun-Yi (jlee@suse.com)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public Licence
 * as published by the Free Software Foundation; either version
 * 2 of the Licence, or (at your option) any later version.
 */
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>
//#include <linux/rtc.h>
#include "rtc.h"

#ifndef RTC_RD_GMTOFF
#define RTC_RD_GMTOFF   _IOR('p', 0x15, long int)    /* Read time zone return seconds east of UTC */
#define RTC_SET_GMTOFF  _IOW('p', 0x16, long int)    /* Set time zone input seconds east of UTC */
#endif
#ifndef RTC_CAPS_READ
#define RTC_CAPS_READ   _IOR('p', 0x17, unsigned int)    /* Get capabilities, e.g. TZ, DST */
#endif
/* Time Zone and Daylight capabilities */
#ifndef RTC_TZ_CAP
#define RTC_TZ_CAP      (1 << 0)
#define RTC_DST_CAP     (1 << 1)
#endif

#define MAX_DEV		5
#define ADJUST_MIN	60
#define DEFAULT_TZ	28800		/* GMT offset of Taiwan R.O.C (Seconds east of UTC)*/
#define ADJUST_TZ	-28800		/* GMT offset of Los Angeles (Seconds east of UTC) */

static const char dev_path[] = "/dev/rtc";
static const char sys_path[] = "/sys/class/rtc/rtc";

static const struct {
	const char *driver_name;
	const char *name;
} names[] = {
	{"rtc_cmos", "CMOS"},
	{"rtc-efi", "EFI"},
	{"rtc-acpitad", "ACPI-TAD"},
};

struct rtc_dev {
	char dev_path[10];
	char sys_path[21];
	char name[15];
	char driver_name[15];
	unsigned int caps;	
};

struct rtc_dev rtc_devs[5];

void search_rtc_dev(void)
{
	int i, j, fd, ret;
	FILE *fin;

	for (i = 0; i <= MAX_DEV; i++)
	{
		char path_tmp[30];

		sprintf(path_tmp, "%s%d", dev_path, i);

		fd = open(path_tmp, O_RDONLY);
        	if (fd !=  -1) {
			struct rtc_dev *dev = &rtc_devs[i];

			memcpy(dev->dev_path, path_tmp, 10);
			sprintf(dev->sys_path, "%s%d/", sys_path, i);

			sprintf(path_tmp, "%s%s", dev->sys_path, "name");
			if ((fin = fopen(path_tmp, "r")) != NULL)
				fscanf(fin, "%s", dev->driver_name);
			fclose(fin);

			for (j = 0; j < sizeof(names)/sizeof(names[0]); j++) {
				if (!strcmp(dev->driver_name, names[j].driver_name))
					memcpy(dev->name, names[j].name, strlen(names[j].name));
			}

			ret = ioctl(fd, RTC_CAPS_READ, &dev->caps);
			if (ret == -1)
				perror("RTC_CAPS_READ ioctl");
			close(fd);
        	}
	}
}

void print_rtc_dev(struct rtc_dev *dev)
{
	long int gmtoff;
	struct rtc_time rtc_tm;
	int fd, ret;
	
	fd = open(dev->dev_path, O_RDONLY);

	printf("Name: %s(%s)\n", dev->name, dev->driver_name);
	printf("    Device Path: %s\n", dev->dev_path);
	printf("    Sysfs Path : %s\n", dev->sys_path);

 	/* Read the RTC time/date */
        ret = ioctl(fd, RTC_RD_TIME, &rtc_tm);
        if (ret == -1)
                perror("RTC_RD_TIME ioctl");

	printf("    RTC date/time: %d-%d-%d %02d:%02d:%02d\n",
                rtc_tm.tm_mday, rtc_tm.tm_mon + 1, rtc_tm.tm_year + 1900,
                rtc_tm.tm_hour, rtc_tm.tm_min, rtc_tm.tm_sec);
        printf("    Is Daylight: %d (%s)\n", rtc_tm.tm_isdst,
		(rtc_tm.tm_isdst)? (rtc_tm.tm_isdst < 0)? "NOT AVAILABLE":"IN EFFECT":"NOT IN EFFECT");
	printf("    wtime->tm_wday: %d\n", rtc_tm.tm_wday);
	printf("    wtime->tm_yday: %d\n", rtc_tm.tm_yday);

	printf("    Capabilities: %d (%s %s)\n", dev->caps,
		(dev->caps & RTC_TZ_CAP)? "TZ":"", 
		(dev->caps & RTC_DST_CAP)? "DST":"");

	/* Read the GMTOFF (Seconds east of UTC) */
	ret = ioctl(fd, RTC_RD_GMTOFF, &gmtoff);
	if (ret == -1)
		printf("    GMTOFF: not support\n");
	else
		printf("    GMTOFF: %ld    ACPI TIMEZONE: %d\n", gmtoff, gmtoff / 60 * -1);

	close(fd);
	printf("\n");
}

void print_rtc_devs(void)
{
	int i;

	for (i = 0; i <= MAX_DEV; i++) {
		if(strlen(rtc_devs[i].sys_path))
			print_rtc_dev(&rtc_devs[i]);
	}
}

void print_rtc_dev2(struct rtc_dev *dev)
{
	long int gmtoff;
	struct rtc_time2 rtc_tm2;
	struct rtc_time rtc_tm = rtc_tm2.tm;
	int fd, ret;
	
	fd = open(dev->dev_path, O_RDONLY);

	printf("Name: %s(%s)\n", dev->name, dev->driver_name);
	printf("    Device Path: %s\n", dev->dev_path);
	printf("    Sysfs Path : %s\n", dev->sys_path);

 	/* Read the RTC time/date */
        ret = ioctl(fd, RTC_RD_TIME2, &rtc_tm2);
        if (ret == -1) {
		close(fd);
                printf("    not support RTC_RD_TIME2\n");
		printf("\n");
		return;
	}
	
	rtc_tm = rtc_tm2.tm;
	printf("    RTC date/time: %d-%d-%d %02d:%02d:%02d\n",
                rtc_tm.tm_mday, rtc_tm.tm_mon + 1, rtc_tm.tm_year + 1900,
                rtc_tm.tm_hour, rtc_tm.tm_min, rtc_tm.tm_sec);
        printf("    Is Daylight: %d (%s)\n", rtc_tm.tm_isdst,
		(rtc_tm.tm_isdst)? (rtc_tm.tm_isdst < 0)? "NOT AVAILABLE":"IN EFFECT":"NOT IN EFFECT");
        printf("    Daylight: %d\n", rtc_tm2.tm_daylight);
	printf("    wtime->tm_wday: %d\n", rtc_tm.tm_wday);
	printf("    wtime->tm_yday: %d\n", rtc_tm.tm_yday);

	printf("    Capabilities: %d (%s %s)\n", dev->caps,
		(dev->caps & RTC_TZ_CAP)? "TZ":"", 
		(dev->caps & RTC_DST_CAP)? "DST":"");

	/* Read the GMTOFF (Seconds east of UTC) */
	gmtoff = rtc_tm2.tm_gmtoff;
	printf("    GMTOFF: %ld    ACPI TIMEZONE: %d\n", gmtoff, gmtoff / 60 * -1);

	close(fd);
	printf("\n");
}

void print_rtc_devs2(void)
{
	int i;

	for (i = 0; i <= MAX_DEV; i++) {
		if(strlen(rtc_devs[i].sys_path))
			print_rtc_dev2(&rtc_devs[i]);
	}
}

void print_rtc_dev_time(struct rtc_dev *dev)
{
	long int gmtoff;
	struct rtc_time rtc_tm;
	int fd, ret;
	
	fd = open(dev->dev_path, O_RDONLY);

 	/* Read the RTC time/date */
        ret = ioctl(fd, RTC_RD_TIME, &rtc_tm);
        if (ret == -1)
                perror("RTC_RD_TIME ioctl");

	printf("        %s: %d-%d-%d %02d:%02d:%02d\n", dev->name,
                rtc_tm.tm_mday, rtc_tm.tm_mon + 1, rtc_tm.tm_year + 1900,
                rtc_tm.tm_hour, rtc_tm.tm_min, rtc_tm.tm_sec);

	close(fd);
}

void print_rtc_devs_time(void)
{
	int i;

	for (i = 0; i <= MAX_DEV; i++) {
		if(strlen(rtc_devs[i].sys_path))
			print_rtc_dev_time(&rtc_devs[i]);
	}
}

void __time_adjust(struct rtc_time *rtc_tm, int in_min)
{
	struct tm inc_tm;
	
	/* modify minutes */
	inc_tm.tm_sec = rtc_tm->tm_sec;
	inc_tm.tm_min = rtc_tm->tm_min + in_min;
	inc_tm.tm_hour = rtc_tm->tm_hour;
	inc_tm.tm_mday = rtc_tm->tm_mday;
	inc_tm.tm_mon = rtc_tm->tm_mon;
	inc_tm.tm_year = rtc_tm->tm_year;
	inc_tm.tm_wday = rtc_tm->tm_wday;
	inc_tm.tm_yday = rtc_tm->tm_yday;

	mktime(&inc_tm);

	rtc_tm->tm_sec = inc_tm.tm_sec;
	rtc_tm->tm_min = inc_tm.tm_min;
	rtc_tm->tm_hour = inc_tm.tm_hour;
	rtc_tm->tm_mday = inc_tm.tm_mday;
	rtc_tm->tm_mon = inc_tm.tm_mon;
	rtc_tm->tm_year = inc_tm.tm_year;
	rtc_tm->tm_wday = inc_tm.tm_wday;
	rtc_tm->tm_yday = inc_tm.tm_yday;
}

void time_adjust(struct rtc_dev *dev, int in_min)
{
	struct rtc_time rtc_tm;
	struct tm inc_tm;
	int fd, ret;
	
	fd = open(dev->dev_path, O_RDONLY);

	/* Read the RTC time/date */
        ret = ioctl(fd, RTC_RD_TIME, &rtc_tm);
        if (ret == -1)
                perror("RTC_RD_TIME ioctl");

	/* Adjust minutes */
	__time_adjust(&rtc_tm, in_min);

	/* Set adjusted time */
        ret = ioctl(fd, RTC_SET_TIME, &rtc_tm);
        if (ret == -1)
                perror("RTC_SET_TIME ioctl");

	close(fd);
}

void time_adjust2(struct rtc_dev *dev, int in_min)
{
	struct rtc_time2 rtc_tm2;
	struct rtc_time *rtc_tm = &rtc_tm2.tm;
	int fd, ret;
	
	fd = open(dev->dev_path, O_RDONLY);

	/* Read the RTC time/date */
        ret = ioctl(fd, RTC_RD_TIME2, &rtc_tm2);
        if (ret == -1)
                perror("RTC_RD_TIME2 ioctl");

	/* Adjust minutes */
	__time_adjust(rtc_tm, in_min);

	/* Set write mask */
	rtc_tm2.writemask |= RTC_TIME2_TIME;

	/* Set adjusted time */
        ret = ioctl(fd, RTC_SET_TIME2, &rtc_tm2);
        if (ret == -1)
                perror("RTC_SET_TIME2 ioctl");

	close(fd);
}

void set_rtc_time_test(void)
{
	int i;

	for (i = 0; i <= MAX_DEV; i++) {
		struct rtc_dev *dev = &rtc_devs[i];

		if(strlen(dev->sys_path)) {
			printf("Test Target: %s(%s)\n", dev->name, dev->driver_name);

			printf("    Before Increase\n");
			print_rtc_devs_time();
			time_adjust(dev, ADJUST_MIN);
			printf("    After Increased %d minutes\n", ADJUST_MIN);
			print_rtc_devs_time();
			printf("    Before Decrease\n");
			print_rtc_devs_time();
			time_adjust(dev, -ADJUST_MIN);
			printf("    After Decreased %d minutes\n", ADJUST_MIN);
			print_rtc_devs_time();

			printf("\n\n");
		}
	}
}

void set_rtc_time_test2(void)
{
	int i;

	for (i = 0; i <= MAX_DEV; i++) {
		struct rtc_dev *dev = &rtc_devs[i];

		if(strlen(dev->sys_path) &&
		   dev->caps & RTC_TZ_CAP) {
			printf("Test Target: %s(%s)\n", dev->name, dev->driver_name);

			printf("    Before Increase\n");
			print_rtc_devs_time();
			time_adjust2(dev, ADJUST_MIN);
			printf("    After Increased %d minutes\n", ADJUST_MIN);
			print_rtc_devs_time();
			printf("    Before Decrease\n");
			print_rtc_devs_time();
			time_adjust2(dev, -ADJUST_MIN);
			printf("    After Decreased %d minutes\n", ADJUST_MIN);
			print_rtc_devs_time();

			printf("\n\n");
		}
	}
}

void print_rtc_dev_tz(struct rtc_dev *dev)
{
	long int gmtoff;
	struct rtc_time rtc_tm;
	int fd, ret;
	
	fd = open(dev->dev_path, O_RDONLY);

	/* Read the GMTOFF (Seconds east of UTC) */
	ret = ioctl(fd, RTC_RD_GMTOFF, &gmtoff);
	if (ret == -1)
		printf("        %s(%s): not support\n", dev->name, dev->driver_name);
	else
		printf("        %s(%s): GMTOFF: %ld    ACPI TIMEZONE: %d\n", dev->name, dev->driver_name, gmtoff, gmtoff / 60 * -1);

	close(fd);
}

void print_rtc_devs_tz()
{
	int i;

	for (i = 0; i <= MAX_DEV; i++) {
		if(strlen(rtc_devs[i].sys_path))
			print_rtc_dev_tz(&rtc_devs[i]);
	}
}

long int change_gmtoff(struct rtc_dev *dev, long int gmtoff_in)
{
	long int gmtoff = 122820;
	struct rtc_time rtc_tm;
	int fd, ret;
	
	fd = open(dev->dev_path, O_RDONLY);

	/* Read the GMTOFF (Seconds east of UTC) */
	ret = ioctl(fd, RTC_RD_GMTOFF, &gmtoff);
	if (ret == -1) {
		printf("RTC_RD_GMTOFF fail.\n");
		goto read_err;
	}
	
	ret = ioctl(fd, RTC_SET_GMTOFF, gmtoff_in);
	if (ret == -1)
		printf("RTC_SET_GMTOFF fail.\n");

read_err:
	close(fd);

	return gmtoff;
}

void access_gmtoff_test(void)
{
	int i;

	for (i = 0; i <= MAX_DEV; i++) {
		struct rtc_dev *dev = &rtc_devs[i];

		if(strlen(dev->sys_path) &&
		   dev->caps & RTC_TZ_CAP) {
			long int orig_tz;

			printf("Test Target: %s(%s)\n", dev->name, dev->driver_name);
			printf("Set to Default TZ: %ld\n", DEFAULT_TZ);
			change_gmtoff(dev, DEFAULT_TZ);

			printf("    Before Adjust TZ\n");
			print_rtc_devs_tz();
			orig_tz = change_gmtoff(dev, ADJUST_TZ);
			printf("    After Adjusted TZ\n");
			print_rtc_devs_tz();
			orig_tz = change_gmtoff(dev, orig_tz);
			printf("    Adjusted Back\n");
			print_rtc_devs_tz();

			printf("\n\n");
		}
	}
}

long int change_gmtoff2(struct rtc_dev *dev, long int gmtoff_in)
{
        struct rtc_time2 rtc_tm2;
	long int gmtoff = 122820;
	int fd, ret;
	
	fd = open(dev->dev_path, O_RDONLY);

	/* Read the GMTOFF (Seconds east of UTC) */
	ret = ioctl(fd, RTC_RD_TIME2, &rtc_tm2);
	if (ret == -1) {
		printf("RTC_RD_TIME2 fail.\n");
		goto read_err;
	}

	gmtoff = rtc_tm2.tm_gmtoff;
	rtc_tm2.tm_gmtoff = gmtoff_in;

	/* Set write mask */
	rtc_tm2.writemask |= RTC_TIME2_GMTOFF;
	
	ret = ioctl(fd, RTC_SET_TIME2, &rtc_tm2);
	if (ret == -1)
		printf("RTC_SET_TIME2 fail.\n");

read_err:
	close(fd);

	return gmtoff;
}

void access_gmtoff_test2(void)
{
	int i;

	for (i = 0; i <= MAX_DEV; i++) {
		struct rtc_dev *dev = &rtc_devs[i];

		if(strlen(dev->sys_path) &&
		   dev->caps & RTC_TZ_CAP) {
			long int orig_tz;

			printf("Test Target: %s(%s)\n", dev->name, dev->driver_name);
			printf("Set to Default TZ: %ld\n", DEFAULT_TZ);
			change_gmtoff2(dev, DEFAULT_TZ);

			printf("    Before Adjust TZ\n");
			print_rtc_devs_tz();
			orig_tz = change_gmtoff2(dev, ADJUST_TZ);
			printf("    After Adjusted TZ\n");
			print_rtc_devs_tz();
			orig_tz = change_gmtoff2(dev, orig_tz);
			printf("    Adjusted Back\n");
			print_rtc_devs_tz();

			printf("\n\n");
		}
	}
}

int main(int argc, char **argv)
{
	search_rtc_dev();
	printf("\nThis testing program will access following ioctl interface:\n");
	printf("    RTC_RD_TIME/RTC_SET_TIME: Used to read and set RTC value.\n");
	printf("    RTC_RD_GMTOFF/RTC_SET_GMTOFF: Used to read and set timezone, input/output is \"Seconds east of UTC\".\n");
	printf("    RTC_CAPS_READ: Read the Timzone and Daylight capabilities of RTC interface.\n");

	printf("\n======== Read Time Testing (RTC_RD_TIME/RTC_CAPS_READ) ========\n\n");
	print_rtc_devs();
	
	printf("\n======== Read Time Testing 2 (RTC_RD_TIME2) ========\n\n");
	print_rtc_devs2();

	printf("\n======== Set Time Testing (RTC_SET_TIME/RTC_RD_TIME) ========\n");
	printf("This testing will increase %d minutes of RTC time then decrease it back.\n\n", ADJUST_MIN);
	set_rtc_time_test();
	
	printf("\n======== Set Time Testing 2 (RTC_SET_TIME2/RTC_RD_TIME2) ========\n");
	printf("Only testing the interface supported Timezone.\n");
	printf("This testing will increase %d minutes of RTC time then decrease it back.\n\n", ADJUST_MIN);
	set_rtc_time_test2();
	
	printf("\n======== Access TimeZone Testing (RTC_RD_GMTOFF/RTC_SET_GMTOFF) ========\n");
	printf("Only testing the interface supported Timezone.\n");
	printf("Timezone of ACPI and UEFI spec: Time zone field is the number of minutes that the local time lags behind the UTC time.\n");
	printf("                                -1440 to 1440 or 2047. Localtime = UTC - TimeZone\n");
	printf("Timezone in GNU tm struct: Seconds east of UTC.\n");
	printf("This testing will set time zone to Los Angeles time (-28800 Seconds east of UTC) then set it back.\n\n", ADJUST_MIN);
	access_gmtoff_test();

	printf("\n======== Access TimeZone Testing 2 (RTC_RD_TIME2/RTC_SET_TIME2) ========\n");
	access_gmtoff_test2();

	return 0;
}
