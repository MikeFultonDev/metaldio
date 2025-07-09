#ifndef __BPAMIO_H__
  #define __BPAMIO_H__ 1

  #include "dbgopts.h"

  #ifndef MEM_MAX
    #define MEM_MAX (8)
  #endif
  #ifndef DD_MAX
    #define DD_MAX (8)
  #endif
  #ifndef DS_MAX
    #define DS_MAX (44) 
  #endif 
  
  struct mstat;

  #ifndef PTR32
    #ifdef VSCODE
      #define PTR32
    #else
      #define PTR32 __ptr32
    #endif
  #endif
  
  typedef struct FM_BPAMHandle_InternalStruct FM_BPAMHandle;

  /*
   * open_pds_for_write: given a fully qualified dataset, return a FM_BPAMHandle to the caller
   * to use for subsequent BPAM I/O activities.
   * Call close_pds to free the underlying resources associated with the dataset.
   */
  FM_BPAMHandle* open_pds_for_write(const char* dataset, const DBG_Opts* opts);

  /*
   * open_pds_for_read: given a fully qualified dataset, return a FM_BPAMHandle to the caller
   * to use for subsequent BPAM I/O activities, restricted to read.
   * Call close_pds to free the underlying resources associated with the dataset.
   */
  FM_BPAMHandle* open_pds_for_read(const char* dataset, const DBG_Opts* opts);

  /*
   * find_member: given an FM_BPAMHandle returned from open_pds_for_read or open_pds_for_write,
   * and a corresponding member name of this PDS, return non-zero if the member name could not
   * be located. 
   * Zero if successful.
   */  
  int find_member(FM_BPAMHandle* bh, const char* mem, const DBG_Opts* opts);

  /*
   * read_record: Read a record up to max_rec_len bytes in size and store the results into the passed in rec
   * buffer.
   * On success, returns the number of bytes read, which could be 0 for VB records of length 0.
   * If nothing could be read, a negative value is returned.
   */
  ssize_t read_record(FM_BPAMHandle* bh, size_t max_rec_len, char* rec, const DBG_Opts* opts);

  /*
   * read_record_direct: Read a record and return an internal pointer to the underlying
   * block in memory, along with a length indicating how long the record is.
   * On success, returns the number of bytes read, which could be 0 for VB records of length 0.
   * If nothing could be read, a negative value is returned.
   */
  ssize_t read_record_direct(FM_BPAMHandle* bh, char** rec, size_t* rec_len, const DBG_Opts* opts);

  /*
   * write_record: Write a record of rec_len bytes in size from the buffer pointed to by rec.
   * On success, returns the number of bytes written, which could be 0 for VB records of length 0.
   * If a write error occurred, a negative value is returned.
   */
  ssize_t write_record(FM_BPAMHandle* bh, size_t rec_len, const char* rec, const DBG_Opts* opts);

  /*
   * flush: When calling write_record, the records are only written to disk when the block is full.
   * When all records are written, call flush to write the remaining partial block to disk.
   * Returns zero if successful, non-zero otherwise.
   */
  int flush(FM_BPAMHandle* bh, const DBG_Opts* opts);

  /*
   * record_length: Return the record length that members of this dataset have. In the case of
   * variable-length records, this is the number of bytes of data in the record not including
   * the internal header at the start of the record on disk.
   * For fixed-length records, there is no internal header at the start of a record, so there
   * is no ambiguity.
   */
  size_t record_length(FM_BPAMHandle* bh, const DBG_Opts* opts);

  /*
   * close_pds: Close the PDS that was opened by open_pds_for_read or open_pds_for_write
   * and free any allocated storage associated with the handle.
   * Returns 0 if successful, non-zero otherwise.
   */
  int close_pds(FM_BPAMHandle* bh, const DBG_Opts* opts);

  /*
   * enq_dataset_member: Acquire the ENQ for the dataset and member specified.
   * This will ensure that you can co-operate with other tools such as ISPF that may be
   * actively editing a dataset. When you are done with the ENQ you should release it 
   * by calling DEQ.
   * Returns 0 if successful, non-zero otherwise
   */
  int enq_dataset_member(const char* dataset, const char* member, const DBG_Opts* opts);

  /*
   * deq_dataset_member: Release the ENQ for the dataset and member specified.
   * See enq_dataset_member for details.
   * Returns 0 if successful, non-zero otherwise
   */
  int deq_dataset_member(const char* dataset, const char* member, const DBG_Opts* opts);

  typedef enum {
    RECORD_FORMAT_F,
    RECORD_FORMAT_FB,
    RECORD_FORMAT_V,
    RECORD_FORMAT_VB,
    RECORD_FORMAT_U,
    RECORD_FORMAT_UNKNOWN
  } record_format_t;

  /*
   * record_format: Return the record format.
   * Returns the record format as a record_format_t enum.
   */
  record_format_t record_format(const FM_BPAMHandle* bh, const DBG_Opts* opts);
    
#endif
