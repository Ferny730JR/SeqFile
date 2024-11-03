#include "seqf_read.h"

size_t
seqf_aread(seqf_statep state, unsigned char *buffer, size_t bufsize)
{
	/* Fill buffer with data */
	size_t buffer_end;
	if((buffer_end = seqf_fill(state, buffer, --bufsize)) == 0)
		return 0;

	/* Trim sequence that was not fully read */
	if(buffer_end == bufsize) {
		while(--buffer_end && buffer[buffer_end] != '>');
		size_t offset = bufsize - buffer_end;
		if(offset > SEQF_CHUNK) {
			seqferrno_ = 5;
			return 0;
		}
		memcpy(state->out_buf, buffer+buffer_end, offset);
		state->next = state->out_buf;
		state->have = offset;
	} else {
		state->have = 0;
		memset(state->out_buf, 0, SEQF_CHUNK);
	}

	buffer[buffer_end] = 0;
	return buffer_end;
}

size_t
seqfaread(SeqFile file, char *buffer, size_t bufsize)
{
	seqf_statep state = (seqf_statep)file;

	mtx_lock(&state->mutex);
	size_t bytes_read = seqf_aread(state, (unsigned char *)buffer, bufsize);
	mtx_unlock(&state->mutex);

	return bytes_read;
}

size_t
seqfaread_unlocked(SeqFile file, char *buffer, size_t bufsize)
{
	return seqf_aread((seqf_statep)file, (unsigned char *)buffer, bufsize);
}

char *
seqf_agets(seqf_statep state, unsigned char *buffer, size_t bufsize)
{
	/* Skip header information, in other words find the start of next sequence */
	if(seqf_skipheader(state, '>') == NULL)
		return NULL;

	/* Declare variables */
	register unsigned char *eos;
	register char *str = (char *)buffer;
	register size_t n, left = bufsize - 1;

	/* Fill buffer with seq */
	if(left) do {
		if(state->have == 0 && seqf_fetch(state) != 0)
			return NULL; // error in seqf_fetch
		if(state->have == 0 || *state->next == '>')
			break;
		
		/* Look for end of line in inteseql buffer */
		n = MIN2(state->have, left);
		eos = (unsigned char *)memchr(state->next, '\n', n);
		if(eos != NULL)
			n = (size_t)(eos - state->next);

		/* Copy to end of seq, DONT include newline */
		memcpy(buffer, state->next, n);
		left -= n;
		buffer += n;

		/* Skip past newline if found */
		if(eos != NULL)
			n++;
		state->have -= n;
		state->next += n;
	} while(left);

	buffer[0] = '\0';
	return str;
}

char *
seqfagets(SeqFile file, char *buffer, size_t bufsize)
{
	seqf_statep state = (seqf_statep)file;

	mtx_lock(&state->mutex);
	char *ret = seqf_agets(state, (unsigned char *)buffer, bufsize);
	mtx_unlock(&state->mutex);

	return ret;
}

char *
seqfagets_unlocked(SeqFile file, char *buffer, size_t bufsize)
{
	return seqf_agets((seqf_statep)file, (unsigned char *)buffer, bufsize);
}
