/* seqf_read.h - Header to access seqf's internal definition and read functions
 * 
 * Copyright (c) Francisco F. Cavazos 2024
 * Subject to the MIT License
 * 
 * This file should not be used in applications. It is used to implement the
 * seqf library and is subject to change.
 */

#include <string.h> // For mem* functions

#include "seqf_core.h"


/**
 * @brief Get the minimum of two comparable values
 * 
 * Used in several *read functions
 */
#define MIN2(A, B)      ((A) < (B) ? (A) : (B))


/**
 * @brief Loads `buffer' with `bufsize' decompressed bytes and store the number
 * of decompressed bytes read into `read'. Returns 0 on success, 1 on a file
 * read error, 2 when encountered end of file (no bytes left to read), and 3 
 * when there was en error decompressing the file stream.
 * 
 * If the file is not of a compressed format, it simply tries to load the buffer
 * with the requested number of bytes.
 * 
 * The function sets the seqferrno and eof flag as appropiate when encountering
 * errors.
 * 
 * @param state   internal state pointer for the SeqFile
 * @param buffer  Buffer in which decompressed bytes will be stored in
 * @param bufsize Requested number of decompressed bytes
 * @param read    Actual number of decompressed bytes read into buffer
 * @return int 
 */
extern int seqf_load(seqf_statep state, unsigned char *buffer, size_t bufsize, size_t *read);


/**
 * @brief Fills the internal output buffer with decompressed bytes. Assumes that
 * state->have is 0 since it will overwrite everything in the output buffer. If
 * state->compression is PLAIN, then it will just copy the data from the file
 * into the output buffer. Updates state->next to point to the first byte of the
 * output buffer and sets state->have to the size of the internal output buffer.
 * 
 * On success return 0, otherwise return 1.
 * 
 * @param state Internal state pointer for the SeqFile
 * @return int  
 */
extern int seqf_fetch(seqf_statep state);


/**
 * @brief Fills `buffer' with `bufsize' decompressed bytes. First, it will fill
 * buffer with the next available bytes from the internal output buffer. If not
 * enough bytes were available to fill the buffer, then it will decompress and
 * load the remaining number of requested bytes directly into the buffer.
 * 
 * @param state   Internal state pointer for the SeqFile
 * @param buffer  Buffer in which decompressed bytes will be stored in
 * @param bufsize Requested number of bytes
 * @return size_t Number of bytes read into `buffer'
 */
extern size_t seqf_fill(seqf_statep state, unsigned char *buffer, size_t bufsize);


/**
 * @brief Skip to the start of sequence data by searching for and skipping a 
 * header line.
 * 
 * This function searches within the internal buffer of a `SeqFile` state for
 * the character specified by `skip` (typically used to identify headers in
 * fasta/fastq files). It advances the reading position past this line to allow
 * direct access to the sequence data.
 * 
 * @param state Pointer to the internal `SeqFile` state
 * @param skip  Character to identify the start of a header line.
 * 
 * @return unsigned char* Pointer to the next position in the buffer after the 
 *         header line. Returns `NULL` if the header line is not found or if an
 *         error occurs.
 */
extern unsigned char *seqf_skipheader(seqf_statep state, char skip);


/**
 * @brief Skip the current line in the internal buffer of a SeqFile state.
 * 
 * This function advances the reading position within the internal buffer of the 
 * `SeqFile` state, moving past the current line to the start of the next line.
 * It is typically used to bypass unwanted lines, allowing the next read
 * operation to begin from the following line.
 * 
 * @param state Pointer to the internal `SeqFile` state
 * 
 * @return unsigned char* Pointer to the start of the next line in the buffer 
 *         after the skipped line. Returns `NULL` if the end of the buffer is 
 *         reached or an error occurs.
 */
extern unsigned char *seqf_skipline(seqf_statep state);


/* Undocumented functions. Used for file-specific reading */

size_t seqf_qread(seqf_statep state, unsigned char *buffer, size_t bufsize);
size_t seqf_aread(seqf_statep state, unsigned char *buffer, size_t bufsize);
size_t seqf_sread(seqf_statep state, unsigned char *buffer, size_t bufsize);

char *seqf_qgets(seqf_statep state, unsigned char *buffer, size_t bufsize);
char *seqf_agets(seqf_statep state, unsigned char *buffer, size_t bufsize);
char *seqf_sgets(seqf_statep state, unsigned char *buffer, size_t bufsize);
