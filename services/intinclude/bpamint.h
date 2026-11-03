#ifndef __BPAM_INT__
  #define __BPAM_INT__ 1

  struct FM_BPAMHandle_InternalStruct {
    char ddname[DD_MAX+1];
    struct ihadcb* PTR32 dcb;
    struct dcbe* PTR32 dcbe;
    struct opencb* PTR32 opencb;
    struct decb* PTR32 decb;
    void* PTR32 block;
    char* PTR32 next_record_start;
    size_t next_record_len;
    size_t block_size;
    size_t bytes_used;
    unsigned int memstart_ttr;
    unsigned int pdsstart_ttr;
    int memstart_ttr_known:1;
    int pdsstart_ttr_known:1;
    size_t line_num;
  };

  struct desp* PTR32 get_desp_all(const FM_BPAMHandle* bh, const DBG_Opts* opts);
  struct desp* PTR32 find_desp(FM_BPAMHandle* bh, const char* memname, const DBG_Opts* opts);
  void free_desp(struct desp* PTR32, const DBG_Opts* opts);
  int write_member_dir_entry(const struct mstat* mstat, FM_BPAMHandle* bh, const DBG_Opts* opts);

#endif