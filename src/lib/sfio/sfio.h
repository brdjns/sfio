#ifndef _SFIO_H
#define _SFIO_H	1

/*	Public header file for the sfio library
**
**	Written by Kiem-Phong Vo (kpv@research.att.com)
*/

/*{KPV: standard definitions */

/* The symbol __STD_C indicates that the language is ANSI-C or C++ */
#ifndef __STD_C
#ifdef __STDC__
#define	__STD_C		1
#else
#if __cplusplus || c_plusplus
#define __STD_C		1
#else
#define __STD_C		0
#endif /*__cplusplus*/
#endif /*__STDC__*/
#endif /*__STD_C*/

/* For C++, extern symbols must be protected against name mangling */
#ifndef _BEGIN_EXTERNS_
#if __cplusplus || c_plusplus
#define _BEGIN_EXTERNS_	extern "C" {
#define _END_EXTERNS_	}
#else
#define _BEGIN_EXTERNS_
#define _END_EXTERNS_
#endif
#endif /*_BEGIN_EXTERNS_*/

/* _ARG_ simplifies function prototypes between K&R-C and more modern Cs */
#ifndef _ARG_
#if __STD_C
#define _ARG_(x)	x
#else
#define _ARG_(x)	()
#endif
#endif /*_ARG_*/

/* The type Void_t is properly defined so that Void_t* can address any type */
#ifndef Void_t
#if __STD_C
#define Void_t		void
#else
#define Void_t		char
#endif
#endif /*Void_t*/

/* The NIL() macro simplifies defining nil pointers to a given type */
#ifndef NIL
#define NIL(type)	((type)0)
#endif /*NIL*/

/* The below are for DLLs on systems such as WINDOWS that only
** allows pointers across client and library code.
*/
#ifndef _PTR_
#if  _DLL_INDIRECT_DATA && !_DLL	/* building client code			*/
#define _ADR_ 		/* cannot export whole structs - data access via ptrs	*/
#define _PTR_	*
#else					/* library code or a normal system	*/
#define _ADR_	&	/* exporting whole struct is ok				*/
#define _PTR_ 
#endif
#endif /*_PTR_*/

/*KPV} */

/* to prevent stupid C++ stdarg.h from including stdio.h */
#if __cplusplus
#ifndef __stdio_h__
#define __stdio_h__	1
#endif
#ifndef _stdio_h_
#define _stdio_h_	1
#endif
#ifndef _stdio_h
#define _stdio_h	1
#endif
#ifndef __h_stdio__
#define __h_stdio__	1
#endif
#ifndef _h_stdio_
#define _h_stdio_	1
#endif
#ifndef _h_stdio
#define _h_stdio	1
#endif
#ifndef __STDIO_H__
#define __STDIO_H__	1
#endif
#ifndef _STDIO_H_
#define _STDIO_H_	1
#endif
#ifndef _STDIO_H
#define _STDIO_H	1
#endif
#ifndef __H_STDIO__
#define __H_STDIO__	1
#endif
#ifndef _H_STDIO_
#define _H_STDIO_	1
#endif
#ifndef _H_STDIO
#define _H_STDIO	1
#endif
#ifndef _stdio_included
#define _stdio_included	1
#endif
#ifndef _included_stdio
#define _included_stdio	1
#endif
#ifndef _INCLUDED_STDIO
#define _INCLUDED_STDIO	1
#endif
#ifndef _STDIO_INCLUDED
#define _STDIO_INCLUDED	1
#endif
#ifndef _SFSTDIO_H
#define _SFSTDIO_H	1
#endif

#ifndef FILE
#define FILE	Sfio_t
#endif
#endif /* __cplusplus */

typedef struct _sfio_	Sfile_t, Sfio_t, SFIO;
typedef struct _sfdc_	Sfdisc_t;
typedef int		(*Sfread_f)_ARG_((Sfio_t*, Void_t*, int, Sfdisc_t*));
typedef int		(*Sfwrite_f)_ARG_((Sfio_t*, const Void_t*, int, Sfdisc_t*));
typedef long		(*Sfseek_f)_ARG_((Sfio_t*, long, int, Sfdisc_t*));
typedef int		(*Sfexcept_f)_ARG_((Sfio_t*, int, Sfdisc_t*));

/* discipline structure */
struct _sfdc_
{
	Sfread_f	readf;		/* read function		*/
	Sfwrite_f	writef;		/* write function		*/
	Sfseek_f	seekf;		/* seek function		*/
	Sfexcept_f	exceptf;	/* to handle exceptions		*/
	Sfdisc_t*	disc;		/* the continuing discipline	*/
};

/* a file structure */
struct _sfio_
{
	unsigned char*	next;	/* next position to read/write from	*/
	unsigned char*	endw;	/* end of write buffer			*/
	unsigned char*	endr;	/* end of read buffer			*/
	unsigned char*	endb;	/* end of buffer			*/
	struct _sfio_*	push;	/* the stream that was pushed on	*/
	unsigned short	flags;	/* type of stream			*/
	short		file;	/* file descriptor			*/
	unsigned char*	data;	/* base of data buffer			*/
	int		size;	/* buffer size				*/
#ifdef _SFIO_PRIVATE
	_SFIO_PRIVATE
#endif
};

/* various constants */
#ifndef NULL
#define NULL	0
#endif
#ifndef EOF
#define EOF	(-1)
#endif
#ifndef SEEK_SET
#define SEEK_SET	0
#define SEEK_CUR	1
#define SEEK_END	2
#endif

/* bits for various types of files */
#define	SF_READ		0000001	/* open for reading			*/
#define SF_WRITE	0000002	/* open for writing			*/
#define SF_STRING	0000004	/* a string stream			*/
#define SF_APPENDWR	0000010	/* associated file is in append mode	*/
#define SF_APPEND	SF_APPENDWR	/* BSDI messed up the namespace	*/
				/* by using the SF_ prefix in stat.h.	*/
				/* If you run across this, either change*/
				/* to SF_APPENDWR or complain to them.	*/
#define SF_MALLOC	0000020	/* buffered space malloc-ed		*/
#define SF_LINE		0000040	/* line buffering			*/
#define SF_SHARE	0000100	/* file stream that is shared		*/
#define SF_EOF		0000200	/* eof was detected			*/
#define SF_ERROR	0000400	/* an error happened			*/
#define SF_STATIC	0001000	/* a stream that cannot be freed	*/
#define SF_IOCHECK	0002000	/* call exceptf before doing IO		*/
#define SF_PUBLIC	0004000	/* SF_SHARE and follow physical seek	*/

#define SF_FLAGS	0005177	/* PUBLIC FLAGS PASSABLE TO SFNEW()	*/
#define SF_SETS		0007163	/* flags passable to sfset()		*/

/* exception events: SF_NEW(0), SF_READ(1), SF_WRITE(2) and the below 	*/
#define SF_SEEK		3	/* seek error				*/
#define SF_CLOSE	4	/* when stream is being closed		*/
#define SF_DPUSH	5	/* when discipline is being pushed	*/
#define SF_DPOP		6	/* when discipline is being popped	*/
#define SF_DPOLL	7	/* see if stream is ready for I/O	*/
#define SF_DBUFFER	8	/* buffer not empty during push or pop	*/
#define SF_SYNC		9	/* a sfsync() call was issued		*/
#define SF_PURGE	10	/* a sfpurge() call was issued		*/

/* for stack and disciplines */
#define SF_POPSTACK	NIL(Sfio_t*)	/* pop the stream stack		*/
#define SF_POPDISC	NIL(Sfdisc_t*)	/* pop the discipline stack	*/

/* for the notify function and discipline exception */
#define SF_NEW		0	/* new stream				*/
#define SF_SETFD	-1	/* about to set the file descriptor 	*/

#define SF_BUFSIZE	8192	/* suggested default buffer size	*/
#define SF_UNBOUND	(-1)	/* unbounded buffer size		*/

#if __STD_C
#include		<stdarg.h>
#endif

_BEGIN_EXTERNS_

#if _DLL_INDIRECT_DATA && _DLL
#define sfstdin		((Sfio_t*)_ast_dll->_ast_stdin)
#define sfstdout	((Sfio_t*)_ast_dll->_ast_stdout)
#define sfstderr	((Sfio_t*)_ast_dll->_ast_stderr)
#else
#define	sfstdin		(&_Sfstdin)	/* standard input stream	*/
#define	sfstdout	(&_Sfstdout)	/* standard output stream	*/
#define	sfstderr	(&_Sfstderr)	/* standard error stream	*/
extern Sfio_t		_Sfstdin;
extern Sfio_t		_Sfstdout;
extern Sfio_t		_Sfstderr;
#endif

extern int		_Sfi;

extern Sfio_t*		sfnew _ARG_((Sfio_t*, Void_t*, int, int, int));
extern Sfio_t*		sfopen _ARG_((Sfio_t*, const char*, const char*));
extern Sfio_t*		sfpopen _ARG_((Sfio_t*, const char*, const char*));
extern Sfio_t*		sfstack _ARG_((Sfio_t*, Sfio_t*));
extern Sfio_t*		sfswap _ARG_((Sfio_t*, Sfio_t*));
extern Sfio_t*		sftmp _ARG_((int));
extern int		_sfflsbuf _ARG_((Sfio_t*, int));
extern int		_sffilbuf _ARG_((Sfio_t*, int));
extern int		sfpurge _ARG_((Sfio_t*));
extern int		sfpoll _ARG_((Sfio_t**, int, int));
extern int		sfpeek _ARG_((Sfio_t*, Void_t**, int));
extern Void_t*		sfreserve _ARG_((Sfio_t*, int, int));
extern int		sfsync _ARG_((Sfio_t*));
extern int		sfclrlock _ARG_((Sfio_t*));
extern Void_t*		sfsetbuf _ARG_((Sfio_t*, Void_t*, int));
extern Sfdisc_t*	sfdisc _ARG_((Sfio_t*,Sfdisc_t*));
extern int		sfnotify _ARG_((void(*)(Sfio_t*, int, int)));
extern int		sfset _ARG_((Sfio_t*, int, int));
extern int		sfsetfd _ARG_((Sfio_t*, int));
extern Sfio_t*		sfpool _ARG_((Sfio_t*, Sfio_t*, int));
extern int		sfread _ARG_((Sfio_t*, Void_t*, int));
extern int		sfwrite _ARG_((Sfio_t*, const Void_t*, int));
extern long		sfmove _ARG_((Sfio_t*, Sfio_t*, long, int));
extern int		sfclose _ARG_((Sfio_t*));
extern long		sftell _ARG_((Sfio_t*));
extern long		sfseek _ARG_((Sfio_t*, long, int));
extern int		sfllen _ARG_((long));
extern int		sfdlen _ARG_((double));
extern int		sfputr _ARG_((Sfio_t*, const char*, int));
extern char*		sfgetr _ARG_((Sfio_t*, int, int));
extern int		sfnputc _ARG_((Sfio_t*, int, int));
extern int		_sfputu _ARG_((Sfio_t*, unsigned long));
extern int		_sfputl _ARG_((Sfio_t*, long));
extern long		_sfgetl _ARG_((Sfio_t*));
extern unsigned long	_sfgetu _ARG_((Sfio_t*));
extern long		_sfgetl _ARG_((Sfio_t*));
extern int		_sfputd _ARG_((Sfio_t*, double));
extern double		sfgetd _ARG_((Sfio_t*));
extern int		sfungetc _ARG_((Sfio_t*, int));
extern char*		sfprints _ARG_((const char*, ...));
extern int		sfprintf _ARG_((Sfio_t*, const char*, ...));
extern int		sfsprintf _ARG_((char*, int, const char*, ...));
extern int		sfscanf _ARG_((Sfio_t*, const char*, ...));
extern int		sfsscanf _ARG_((const char*, const char*, ...));
extern int		sfvprintf _ARG_((Sfio_t*, const char*, va_list));
extern int		sfvscanf _ARG_((Sfio_t*, const char*, va_list));
extern char*		sfecvt _ARG_((double,int,int*,int*));
extern char*		sffcvt _ARG_((double,int,int*,int*));

/* io functions with discipline continuation */
extern int		sfrd _ARG_((Sfio_t*, Void_t*, int, Sfdisc_t*));
extern int		sfwr _ARG_((Sfio_t*, const Void_t*, int, Sfdisc_t*));
extern long		sfsk _ARG_((Sfio_t*, long, int, Sfdisc_t*));
extern int		sfpkrd _ARG_((int, Void_t*, int, int, long, int));

/* function analogues of fast in-line functions */
extern int		sfgetc _ARG_((Sfio_t*));
extern long		sfgetl _ARG_((Sfio_t*));
extern unsigned long	sfgetu _ARG_((Sfio_t*));
extern int		sfputc _ARG_((Sfio_t*,int));
extern int		sfputd _ARG_((Sfio_t*,double));
extern int		sfputl _ARG_((Sfio_t*,long));
extern int		sfputu _ARG_((Sfio_t*,unsigned long));
extern int		sfslen _ARG_((void));
extern int		sfulen _ARG_((unsigned long));
extern long		sfsize _ARG_((Sfio_t*));
extern int		sfclrerr _ARG_((Sfio_t*));
extern int		sfeof _ARG_((Sfio_t*));
extern int		sferror _ARG_((Sfio_t*));
extern int		sffileno _ARG_((Sfio_t*));
extern int		sfstacked _ARG_((Sfio_t*));

_END_EXTERNS_

/* fast in-line functions */
#define sfputc(f,c)	((f)->next >= (f)->endw ? \
				_sfflsbuf(f,(int)((unsigned char)(c))) : \
				(int)(*(f)->next++ = (unsigned char)(c)))
#define sfgetc(f)	((f)->next >= (f)->endr ? _sffilbuf(f,0) : (int)(*(f)->next++))
#define sffileno(f)	((f)->file)
#define sfeof(f)	((f)->flags&SF_EOF)
#define sferror(f)	((f)->flags&SF_ERROR)
#define sfclrerr(f)	((f)->flags &= ~(SF_ERROR|SF_EOF))
#define sfstacked(f)	((f)->push != NIL(Sfio_t*))

#if !_DLL_INDIRECT_DATA || _DLL
#define sfslen()	(_Sfi)
#endif

/* coding long integers in a portable and compact fashion */
#define SF_SBITS	6
#define SF_UBITS	7
#define SF_SIGN		(1 << SF_SBITS)
#define SF_MORE		(1 << SF_UBITS)
#define SF_U1		SF_MORE
#define SF_U2		(SF_U1*SF_U1)
#define SF_U3		(SF_U2*SF_U1)
#define SF_U4		(SF_U3*SF_U1)
#define sfulen(v)	((v) < SF_U1 ? 1 : (v) < SF_U2 ? 2 : \
			 (v) < SF_U3 ? 3 : (v) < SF_U4 ? 4 : 5)
#define sfgetu(f)	((_Sfi = sfgetc(f)) < 0 ? -1 : \
				((_Sfi&SF_MORE) ? _sfgetu(f) : (unsigned long)_Sfi))
#define sfgetl(f)	((_Sfi = sfgetc(f)) < 0 ? -1 : \
				((_Sfi&(SF_MORE|SF_SIGN)) ? _sfgetl(f) : (long)_Sfi))
#define sfputu(f,v)	_sfputu((f),(unsigned long)(v))
#define sfputl(f,v)	_sfputl((f),(long)(v))
#define sfputd(f,v)	_sfputd((f),(double)(v))

#endif /* _SFIO_H */