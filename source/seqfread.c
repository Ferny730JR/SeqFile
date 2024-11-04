#include "seqf_read.h"

static size_t
seqf_read(seqf_statep state, unsigned char *buffer, size_t bufsize)
{
	switch(state->type) {
	case 'a': return seqf_aread(state, buffer, bufsize);
	case 'q': return seqf_qread(state, buffer, bufsize);
	case 's': return seqf_sread(state, buffer, bufsize);
	case 'b': return seqf_fill(state, buffer, bufsize);
	default: 
		seqferrno_ = 4;
		return 0;
	}
}

size_t
seqfread(SeqFile file, char *buffer, size_t bufsize)
{
	seqf_statep state = (seqf_statep)file;

	mtx_lock(&state->mutex);
	size_t bytes_read = seqf_read(state, (unsigned char *)buffer, bufsize);
	mtx_unlock(&state->mutex);

	return bytes_read;
}

size_t
seqfread_unlocked(SeqFile file, char *buffer, size_t bufsize)
{
	return seqf_read((seqf_statep)file, (unsigned char *)buffer, bufsize);
}

static char *
seqf_line(seqf_statep state, unsigned char *buffer, size_t bufsize)
{
	/* Sanity checks */
	if(state == NULL || buffer == NULL || bufsize == 0)
		return NULL;
	if(state->eof)
		return NULL;

	/* Declare variables */
	register size_t n, left = bufsize - 1;
	register unsigned char *str = buffer;
	register unsigned char *eol;

	/* Begin filling buffer */
	if(left) do {
		if(state->have == 0 && seqf_fetch(state) != 0)
			return NULL; // fetch encountered error
		if(state->have == 0)
			break;

		n = MIN2(state->have, left);
		eol = (unsigned char *)memchr(state->next, '\n', n);
		if(eol != NULL)
			n = (size_t)(eol - state->next) + 1;

		/* Copy line into buffer */
		memcpy(buffer, state->next, n);
		left -= n;
		buffer += n;
		state->have -= n;
		state->next += n;
	} while(left && eol == NULL);

	buffer[0] = '\0';
	return (char *)str;
}

static char *
seqf_gets(seqf_statep state, unsigned char *buffer, size_t bufsize)
{
	switch(state->type) {
	case 'a': return seqf_agets(state, buffer, bufsize);
	case 'q': return seqf_qgets(state, buffer, bufsize);
	case 's': return seqf_sgets(state, buffer, bufsize);
	case 'b': return seqf_line(state, buffer, bufsize);
	default:
		seqferrno_ = 5;
		return NULL;
	}
}

char *
seqfgets(SeqFile file, char *buffer, size_t bufsize)
{
	seqf_statep state = (seqf_statep)file;

	mtx_lock(&state->mutex);
	char *ret = seqf_gets(state, (unsigned char *)buffer, bufsize);
	mtx_unlock(&state->mutex);

	return ret;
}

char *
seqfgets_unlocked(SeqFile file, char *buffer, size_t bufsize)
{
	return seqf_gets((seqf_statep)file, (unsigned char *)buffer, bufsize);
}