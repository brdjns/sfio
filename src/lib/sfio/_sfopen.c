#include	"sfhdr.h"

/*	Open a file/string for IO.
**	If f is not nil, it is taken as an existing stream that should be
**	closed and its structure reused for the new stream.
**
**	Written by Kiem-Phong Vo.
*/

#if _BLD_sfio && defined(__EXPORT__)
#define extern  __EXPORT__
#endif
extern
#undef  extern

#if __STD_C
Sfio_t* _sfopen(reg Sfio_t* f, const char* file, const char* mode)
#else
Sfio_t* _sfopen(f,file,mode)
reg Sfio_t*	f;		/* old stream structure */
char*		file;		/* file/string to be opened */
reg char*	mode;		/* mode of the stream */
#endif
{
	int	fd, oldfd, oflags, sflags;

	/* get the control flags */
	if((sflags = _sftype(mode,&oflags,NIL(int*))) == 0)
		return NIL(Sfio_t*);

	/* changing the control flags */
	if(f && !file && !((f->flags|sflags)&SF_STRING) )
	{	SFMTXSTART(f, NIL(Sfio_t*));

		if(f->mode&SF_INIT ) /* stream uninitialized, ok to set flags */
		{	f->flags |= (sflags & (SF_FLAGS & ~SF_RDWR));

			if((sflags &= SF_RDWR) != 0) /* reset read/write modes */
			{	f->flags = (f->flags & ~SF_RDWR) | sflags;

				if((f->flags&SF_RDWR) == SF_RDWR)
					f->bits |= SF_BOTH;
				else	f->bits &= ~SF_BOTH;

				if(f->flags&SF_READ)
					f->mode = (f->mode&~SF_WRITE)|SF_READ;
				else	f->mode = (f->mode&~SF_READ)|SF_WRITE;
			}
		}
		else /* make sure there is no buffered data */
		{	if(sfsync(f) < 0)
				SFMTXRETURN(f,NIL(Sfio_t*));
		}

		if(f->file >= 0 && (oflags &= (O_TEXT|O_BINARY|O_APPEND)) != 0 )
		{	/* set file access control */
			int ctl = sysfcntlf(f->file, F_GETFL, 0);
			ctl = (ctl & ~(O_TEXT|O_BINARY|O_APPEND)) | oflags;
			sysfcntlf(f->file, F_SETFL, ctl);
		}

		SFMTXRETURN(f,f);
	}

	if(sflags&SF_STRING)
	{	f = sfnew(f,(char*)file,
		  	  file ? (size_t)strlen((char*)file) : (size_t)SF_UNBOUND,
		  	  -1,sflags);
	}
	else
	{	if(!file)
			return NIL(Sfio_t*);

#if _has_oflags /* open the file */
		while((fd = sysopenf((char*)file,oflags,SF_CREATMODE)) < 0 && errno == EINTR)
			errno = 0;
#else
		while((fd = sysopenf(file,oflags&O_ACCMODE)) < 0 && errno == EINTR)
			errno = 0;
		if(fd >= 0)
		{	if((oflags&(O_CREAT|O_EXCL)) == (O_CREAT|O_EXCL) )
			{	CLOSE(fd);	/* error: file already exists */
				return NIL(Sfio_t*);
			}
			if(oflags&O_TRUNC )	/* truncate file */
			{	reg int	tf;
				while((tf = syscreatf(file,SF_CREATMODE)) < 0 &&
				      errno == EINTR)
					errno = 0;
				CLOSE(tf);
			}
		}
		else if(oflags&O_CREAT)
		{	while((fd = syscreatf(file,SF_CREATMODE)) < 0 && errno == EINTR)
				errno = 0;
			if((oflags&O_ACCMODE) != O_WRONLY)
			{	/* the file now exists, reopen it for read/write */
				CLOSE(fd);
				while((fd = sysopenf(file,oflags&O_ACCMODE)) < 0 &&
				      errno == EINTR)
					errno = 0;
			}
		}
#endif
		if(fd < 0)
			return NIL(Sfio_t*);

		/* we may have to reset the file descriptor to its old value */
		oldfd = f ? f->file : -1;
		if((f = sfnew(f,NIL(char*),(size_t)SF_UNBOUND,fd,sflags)) && oldfd >= 0)
			(void)sfsetfd(f,oldfd);
	}

	return f;
}

#if __STD_C
int _sftype(reg const char* mode, int* oflagsp, int* uflagp)
#else
int _sftype(mode, oflagsp, uflagp)
reg char*	mode;
int*		oflagsp;
int*		uflagp;
#endif
{
	reg int	sflags, oflags, uflag;

	if(!mode)
		return 0;

	/* construct the open flags */
	sflags = oflags = uflag = 0;
	while(1) switch(*mode++)
	{
	case 'w' :
		sflags |= SF_WRITE;
		oflags |= O_WRONLY | O_CREAT;
		if(!(sflags&SF_READ))
			oflags |= O_TRUNC;
		continue;
	case 'a' :
		sflags |= SF_WRITE | SF_APPENDWR;
		oflags |= O_WRONLY | O_APPEND | O_CREAT;
		continue;
	case 'r' :
		sflags |= SF_READ;
		oflags |= O_RDONLY;
		continue;
	case 's' :
		sflags |= SF_STRING;
		continue;
	case 'b' :
		oflags |= O_BINARY;
		continue;
	case 't' :
		oflags |= O_TEXT;
		continue;
	case 'x' :
		oflags |= O_EXCL;
		continue;
	case '+' :
		if(sflags)
			sflags |= SF_READ|SF_WRITE;
		continue;
	case 'm' :
		sflags |= SF_MTSAFE;
		uflag = 0;
		continue;
	case 'u' :
		sflags &= ~SF_MTSAFE;
		uflag = 1;
		continue;
	default :
		if(!(oflags&O_CREAT) )
			oflags &= ~O_EXCL;
#if _WIN32 && !_WINIX
		if(!(oflags&(O_BINARY|O_TEXT)))
			oflags |= O_BINARY;
#endif
		if((sflags&SF_RDWR) == SF_RDWR)
			oflags = (oflags&~O_ACCMODE)|O_RDWR;
		if(oflagsp)
			*oflagsp = oflags;
		if(uflagp)
			*uflagp = uflag;
		if((sflags&(SF_STRING|SF_RDWR)) == SF_STRING)
			sflags |= SF_READ;
		return sflags;
	}
}
