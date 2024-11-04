/* readfasta.c - Functions for parsing fasta files
 * 
 * Copyright (c) Francisco F. Cavazos 2024
 * Subject to the MIT License
 * 
 * This file should not be used in applications. It is used to implement the
 * seqf library and is subject to change.
 */

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

int
seqfagetnt_unlocked(SeqFile file)
{
	seqf_statep state = (seqf_statep)file;
	if(state == NULL || state->eof)
		return EOF;
	if(state->have == 0 && seqf_fetch(state) != 0)
		return EOF;
	
	/* Skip past newline characters, we're interested in what comes next */
	if(*state->next == '\n') {
		state->have--;
		state->next++;
	}

	/* If start of fasta header, skip it to get to nt, or return on error */
	if(*state->next == '>' && seqf_skipline(state) == NULL)
		return EOF;

	state->have--;
	return *state->next++;
}

int
seqfagetnt(SeqFile file)
{
	seqf_statep state = (seqf_statep)file;

	mtx_lock(&state->mutex);
	int ret = seqfagetnt_unlocked(file);
	mtx_unlock(&state->mutex);

	return ret;
}
