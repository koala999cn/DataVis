/* NUMfft_d.c
 *
 * Copyright (C) 1997-2011 David Weenink
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/* djmw 20020813 GPL header
	djmw 20040511 Added n>1 test for compatibility with old behaviour.
	djmw 20110308 struct renaming
 */

#include <math.h>

#define my me ->

#include "NUM2.h"
#include "NUMfft_core.h"

void NUMforwardRealFastFourierTransform (FFT_DATA_TYPE *data, long n) {
	autoNUMfft_Table table;
	NUMfft_Table_init (& table, n);
	NUMfft_forward (& table, data);

	if (n > 1) {
		// To be compatible with old behaviour
		FFT_DATA_TYPE tmp = data[n-1];
		for (long i = n-1; i > 1; i--) {
			data[i] = data[i - 1];
		}
		data[1] = tmp;
	}
}

void NUMreverseRealFastFourierTransform (FFT_DATA_TYPE *data, long n) {
	autoNUMfft_Table table;

	if (n > 1) {
		// To be compatible with old behaviour
		FFT_DATA_TYPE tmp = data[1];
		for (long i = 1; i < n-1; i++) {
			data[i] = data[i + 1];
		}
		data[n-1] = tmp;
	}

	NUMfft_Table_init (& table, n);
	NUMfft_backward (& table, data);
}

void NUMfft_forward (NUMfft_Table me, FFT_DATA_TYPE *data) {
	if (my n == 1) {
		return;
	}
	drftf1 (my n, data, my trigcache, my trigcache + my n, my splitcache);
}

void NUMfft_backward (NUMfft_Table me, FFT_DATA_TYPE *data) {
	if (my n == 1) {
		return;
	}
	drftb1 (my n, data, my trigcache, my trigcache + my n, my splitcache);
}

void NUMfft_Table_init (NUMfft_Table me, long n) {
	my n = n;
	my trigcache = new FFT_DATA_TYPE[3 * n - 1];
	my splitcache = new long[31];
	NUMrffti (n, my trigcache, my splitcache);
}


/* End of file NUMfft.c */
