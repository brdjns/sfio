#include	"sfhdr.h"

/*	Obtain/release exclusive use of a stream.
**
**	Written by Kiem-Phong Vo.
*/

/* the main locking/unlocking interface */
#if __STD_C
int sfmutex(Sfio_t* f, int type)
#else
int sfmutex(f, type)
Sfio_t*	f;
int	type;
#endif
{
#if !vt_threaded
	NOTUSED(f); NOTUSED(type);
	return 0;
#else

	SFONCE();

	if(!f)
		return -1;

	if(!f->mutex)
	{	if(f->bits&SF_PRIVATE)
			return 0;

		vtmtxlock(_Sfmutex);
		f->mutex = vtmtxopen(NIL(Vtmutex_t*), VT_INIT);
		vtmtxunlock(_Sfmutex);
		if(!f->mutex)
			return -1;
	}

	if(type == SFMTX_LOCK)
		return vtmtxlock(f->mutex);
	else if(type == SFMTX_TRYLOCK)
		return vtmtxtrylock(f->mutex);
	else if(type == SFMTX_UNLOCK)
		return vtmtxunlock(f->mutex);
	else if(type == SFMTX_CLRLOCK)
		return vtmtxclrlock(f->mutex);
	else	return -1;
#endif /*vt_threaded*/
}
