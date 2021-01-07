/* <dirent.h> includes <sys/dirent.h>, which is this file.  On a
   system which supports <dirent.h>, this file is overridden by
   dirent.h in the libc/sys/.../sys directory.  On a system which does
   not support <dirent.h>, we will get this file which uses #error to force
   an error.  */

#ifndef _dirent_h_
#define _dirent_h_

#include <sys/types.h>
#include <sys/syslimits.h>

#define _DIRENT_HAVE_D_TYPE

#define	DT_UNKNOWN	 0
#define	DT_FIFO		 1
#define	DT_CHR		 2
#define	DT_DIR		 4
#define	DT_BLK		 6
#define	DT_REG		 8
#define	DT_LNK		10
#define	DT_SOCK		12
#define	DT_WHT		14

#ifdef __cplusplus
extern "C" {
#endif
	
	struct dirent {
		ino_t	d_ino;
		unsigned char  d_type;
		size_t  d_reclen;
		char	d_name[NAME_MAX+1];
	};

	struct __dirstream;
	typedef struct __dirstream DIR;
	

#ifdef __cplusplus
}
#endif

#endif // _dirent_h_
