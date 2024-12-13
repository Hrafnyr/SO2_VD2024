/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _LINUX_UTS_H
#define _LINUX_UTS_H

/*
 * Defines for what uname() should return - Como dice es para definir lo que el comando uname retorna/muestra,
    en este caso es el nombre con uname -s
 */
#ifndef UTS_SYSNAME
#define UTS_SYSNAME "Linux-Vladek-v1"
#endif

#ifndef UTS_NODENAME
#define UTS_NODENAME CONFIG_DEFAULT_HOSTNAME /* set by sethostname() */
#endif

#ifndef UTS_DOMAINNAME
#define UTS_DOMAINNAME "(none)"	/* set by setdomainname() */
#endif

#endif
