#include "seqf_read.h"

size_t
seqf_qread(seqf_statep state, unsigned char *buffer, size_t bufsize)
{
	/* Fill buffer with data, return 0 if nothing was filled */
	size_t buffer_end;
	if((buffer_end = seqf_fill(state, buffer, bufsize)) == 0)
		return 0;

	/* Trim sequence that was not fully read */
	if(buffer_end == bufsize) {
		register bool not_validated = true;
		do {
			/* If at fastq could not be validated, return error */
			if(buffer_end == 0) {
				seqferrno_ = 5;
				return 0;
			}

			/* Validate by searching for '@', which must be followed by a '+' */
			while(--buffer_end && buffer[buffer_end] != '@');
			register int validate = buffer_end, count = 0;
			while(validate && count < 3) if(buffer[--validate] == '\n') count++;
			if(++validate && buffer[validate] == '+') not_validated = false;
		} while(not_validated);

		/* Copy seq that wasn't fully read into internal buffer */
		size_t offset = bufsize - buffer_end;
		if(offset > SEQF_CHUNK) {
			seqferrno_ = 5;
			return 0;
		}
		memcpy(state->out_buf, buffer+buffer_end, offset);
		state->have = offset;
		state->next = state->out_buf;
	} else {
		state->have = 0;
		memset(state->out_buf, 0, SEQF_CHUNK);
	}
	buffer[buffer_end] = 0;
	return buffer_end;
}

size_t
seqfqread(SeqFile file, char *buffer, size_t bufsize)
{
	seqf_statep state = (seqf_statep)file;

	mtx_lock(&state->mutex);
	size_t bytes_read = seqf_qread(state, (unsigned char *)buffer, bufsize);
	mtx_unlock(&state->mutex);

	return bytes_read;
}

size_t
seqfqread_unlocked(SeqFile file, char *buffer, size_t bufsize)
{
	return seqf_qread((seqf_statep)file, (unsigned char *)buffer, bufsize);
}

char *
seqf_qgets(seqf_statep state, unsigned char *buffer, size_t bufsize)
{
	if(state == NULL || buffer == NULL || bufsize == 0)
		return NULL;
	if(state->eof)
		return NULL;
	
	/* Find start of next sequence */
	if(seqf_skipheader(state, '@') == NULL)
		return NULL;

	/* Declare variables */
	register unsigned char *eos;
	register char *str = (char *)buffer;
	register size_t n, left = bufsize - 1;

	/* Fill buffer with fastq sequence */
	if(left) do {
		if(state->have == 0 && seqf_fetch(state) != 0)
			return NULL; // error in seqf_fetch
		if(state->have == 0 || *state->next == '+')
			break;

		/* Look for end of line in internal buffer */
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

	seqf_skipline(state); /* Skip '+' line */
	seqf_skipline(state); /* Skip quality scores */

	/* Null terminate and return str */
	buffer[0] = '\0';
	return str;
}

char *
seqfqgets(SeqFile file, char *buffer, size_t bufsize)
{
	seqf_statep state = (seqf_statep)file;

	mtx_lock(&state->mutex);
	char *ret = seqf_qgets(state, (unsigned char *)buffer, bufsize);
	mtx_unlock(&state->mutex);

	return ret;
}

char *
seqfqgets_unlocked(SeqFile file, char *buffer, size_t bufsize)
{
	return seqf_qgets((seqf_statep)file, (unsigned char *)buffer, bufsize);
}
