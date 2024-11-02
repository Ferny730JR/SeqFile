#ifndef SEQFILES_H
#define SEQFILES_H

#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief Get the error number encountered by SeqFile
 * 
 * @return int error number
 */
int *seqfgeterrno(void);


/**
 * @brief Error number encountered by SeqFile
 */
#define seqferrno (*seqfgeterrno())


/**
 * @brief Opaque pointer for SeqFile struct
 */
typedef struct SeqFile *SeqFile;


/**
 * @brief Open an fasta/fastq/sequence files fo reading. Mode is used to specify which type
 * of file is used for reading. "a" for fasta, "q" for fastq, "s" for sequence file, and
 * "b" for binary.
 * 
 * @param path Path to the file you want to open for reading
 * @param mode Type of file being opened
 * @return SeqFile 
 */
SeqFile seqfopen(const char *path, const char *mode);


/**
 * @brief Close an SeqFile handle.
 * 
 * Removes all allocated resources associated with SeqFile.
 * 
 * @param file SeqFile to close
 */
int seqfclose(SeqFile file);


/**
 * @brief Rewind a SeqFile to read from the beginning of the file.
 * 
 * @param file SeqFile to rewind
 * @return int return code: 0 if success, failure otherwise
 */
int seqfrewind(SeqFile file);


/**
 * @brief Test if SeqFile is at the end of file.
 * 
 * @param file SeqFile pointer to test
 * @return true if at end of file
 * @return false if not at end of file
 */
bool seqfeof(SeqFile file);


/**
 * @brief Return an allocated string detailing the error encountered from SeqFile
 * 
 * @param _seqferrno The seqferrno variable
 * @return char* String with error description
 * 
 * @note It is necessary to free the string returned from this function as memory is
 * allocated to store the string.
 */
const char *seqfstrerror(int _seqferrno);


/**
 * @brief Fill buffer with a string detailing the error code from _seqferrno
 * 
 * @param _seqferrno The seqferrno variable
 * @param buffer     Buffer to fill with error description
 * @param bufsize    Size of the buffer
 * @return int 0 on success, 1 when buffer was too small for the error message
 */
int seqfstrerror_r(int _seqferrno, char *buffer, size_t bufsize);


/**
 * @brief Read SeqFile sequences into buffer. Will continue reading until it can no
 * longer fit a full sequence within bufsize characters.
 * 
 * @param file    SeqFile pointer to read from
 * @param buffer  Buffer to write sequences to
 * @param bufsize Size of the buffer being passed
 * @return size_t Number of bytes read into buffer. 0 if end of file, or error encountered.
 */
size_t seqfread(SeqFile file, char *buffer, size_t bufsize);


/**
 * @brief Read SeqFile sequences into a buffer. Will continue reading until it can no
 * longer fit a full sequence within butsize characters.
 * 
 * @param file    SeqFile pointer to read from
 * @param buffer  Buffer to write sequences to
 * @param bufsize Size of the buffer being passed
 * @return size_t Number of bytes read into buffer. 0 if end of file, or error encountered.
 * 
 * @note
 * This function does not use a mutex to lock access to the SeqFile buffer. As such, it is not
 * thread safe. Only use in single-threaded applications
 */
size_t seqfread_unlocked(SeqFile file, char *buffer, size_t bufsize);


/** Undocumented read functions. See `seqfread()` for more information. */
size_t seqfqread(SeqFile file, char *buffer, size_t bufsize);
size_t seqfqread_unlocked(SeqFile file, char *buffer, size_t bufsize);
size_t seqfaread(SeqFile file, char *buffer, size_t bufsize);
size_t seqfaread_unlocked(SeqFile file, char *buffer, size_t bufsize);
size_t seqfsread(SeqFile file, char *buffer, size_t bufsize);
size_t seqfsread_unlocked(SeqFile file, char *buffer, size_t bufsize);


/**
 * @brief Read a record's sequence into `buffer`.
 * 
 * `seqfgets()` reads at most `bufsize - 1` characters and stores them into the `buffer` string. Reading
 * stops as soon as the end of the record it is currently processing has been reached or the end of file
 * has been reached. A `'\0'` is appended to `buffer` to ensure the string is null terminated.
 * 
 * @param file    SeqFile to read from
 * @param buffer  Buffer to fill
 * @param bufsize Size of the buffer being passed
 * @return char* Pointer to the record's sequence, or NULL if unable to find the next record.
 */
char *seqfgets(SeqFile file, char *buffer, size_t bufsize);


/**
 * @brief Read a record's sequence into `buffer`.
 * 
 * `seqfgets()` reads at most `bufsize - 1` characters and stores them into the `buffer` string. Reading
 * stops as soon as the end of the record it is currently processing has been reached or the end of file
 * has been reached. A `'\0'` is appended to `buffer` to ensure the string is null terminated.
 * 
 * @param file    SeqFile to read from
 * @param buffer  Buffer to fill
 * @param bufsize Size of the buffer being passed
 * @return char* Pointer to the record's sequence, or NULL if unable to find the next record.
 * 
 * @note
 * This function does not use a mutex to lock access to the SeqFile internal buffer. As such, it is not
 * thread-safe. Only use in single-threaded applications.
 */
char *seqfgets_unlocked(SeqFile file, char *buffer, size_t bufsize);


/** Undocumented gets functions. See seqfgets() for more information. */
char *seqfagets(SeqFile file, char *buffer, size_t bufsize);
char *seqfagets_unlocked(SeqFile file, char *buffer, size_t bufsize);
char *seqfsgets(SeqFile file, char *buffer, size_t bufsize);
char *seqfsgets_unlocked(SeqFile file, char *buffer, size_t bufsize);
char *seqfqgets(SeqFile file, char *buffer, size_t bufsize);
char *seqfqgets_unlocked(SeqFile file, char *buffer, size_t bufsize);

#ifdef __cplusplus
}
#endif

#endif
