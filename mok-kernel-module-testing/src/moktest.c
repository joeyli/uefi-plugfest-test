/* Test MOK
 *
 * Copyright (C) 2013 SUSE Linux Products GmbH. All rights reserved.
 * Written by Chun-Yi Lee (jlee@suse.com)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public Licence
 * as published by the Free Software Foundation; either version
 * 2 of the Licence, or (at your option) any later version.
 */

#include <linux/module.h>

static int __init moktest_init(void)
{
	printk(KERN_INFO "moktest: driver v0.1 successfully loaded.\n");

	return 0;
}

static void __exit moktest_cleanup(void)
{
	printk(KERN_INFO "moktest: driver unloaded.\n");
}

module_init(moktest_init);
module_exit(moktest_cleanup);

MODULE_AUTHOR("Lee Chun-Yi");
MODULE_DESCRIPTION("Test MOK");
MODULE_VERSION("0.1");
MODULE_LICENSE("GPL");
