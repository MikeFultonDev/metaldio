#ifndef __DESERV_H__
#define __DESERV_H__

#include "metaldio.h"

#pragma pack(1)

struct desl_name {
  unsigned short desl_name_len;
  char desl_name[8];
};

struct deslx {
  unsigned char   desl_code;
  unsigned short  desl_errcode;
  unsigned char   _filler1;
  struct desl_name* PTR32   desl_smde_ptr;
  struct desl_name* PTR32   desl_name_ptr;
};

struct desp {
  union {
    struct {
      unsigned char  desp_id[8];      /* eyecatcher (IGWDES_P) */
      int            desp_len;        /* length of buffer      */
      unsigned int               : 7,
                     desp_lev_iv : 1; /* parm list level       */
      unsigned char  _filler1[3];     /* RESERVED              */
    };
    unsigned char  desp_header[16]; /* Standard Header */
  };
  unsigned char  desp_func;                 /* function type (GET_ALL, GET,     */
  unsigned char  _filler2[3];               /* RESERVED                         */
  void * PTR32 desp_member_counts_ptr;    /* pointer to optional output  @08A */
  union {
    unsigned char  desp_data[12]; /* function data                    */
    unsigned char  desp_flags[2]; /* DESP FLAG1                  @P6C */
    struct {
      unsigned int   desp_bypass_lla     : 1, /* 0=USE LLA, 1=BYPASS LLA             */
                     desp_concat_flg     : 1, /* 0=CONCAT PARM NOT SPECIFIED,        */
                     desp_subpool_flg    : 1, /* 0=SUBPOOL PARM NOT SPECIFIED,       */
                     desp_c370lib        : 1, /* 0=C370LIB PARM NOT SPECIFIED, @L3A  */
                     desp_concat_all     : 1, /* ONLY VALID IF DESP_CONCAT_FLG @L3A  */
                     desp_hide           : 1, /* 0=all names are visible      @L3A   */
                     desp_system_dcb     : 1, /* 0=SYSTEM DCB NOT SPECIFIED.  @L3A   */
                     desp_ext_attr       : 1; /* 0=SYSTEM DCB NOT SPECIFIED.    @L5A */
      unsigned int   desp_override_purge : 1, /* 1=Override connect purge       @L7A */
                                         : 7;
      unsigned char  desp_exit_scope;         /* EXIT SCOPE                          */
      unsigned char  desp_concat;             /* concatenation number       @L2A     */
      unsigned char  desp_libtype;            /* function subtype (DCB, DEB)         */
      unsigned char  desp_gettype;            /* function subtype (NAME_LIST or      */
      unsigned char  desp_reltype;            /* function subtype (DE_LIST  @L2A     */
      unsigned char  desp_exit_option;        /* EXIT option                         */
      unsigned char  desp_option;             /* PUT REPLACE option                  */
      unsigned char  desp_subpool;            /* SUBPOOL identifier         @L2A     */
      unsigned char  desp_conn_intent;        /* connect intent                      */
      unsigned char  _filler3;                /* RESERVED                            */
      };
    };
  struct ihadcb * PTR32 desp_dcb_ptr;     /* DCB address                      */
  struct deb * PTR32 desp_deb_ptr;        /* DEB address                      */
  void * PTR32 desp_conn_id_ptr;          /* connect identifier address       */
  struct desb* PTR32 * PTR32 desp_areaptr_ptr;   /* address for buffer address @L2A  */
  struct desb* PTR32 desp_area_ptr;       /* buffer address                   */
  int            desp_area2;              /* buffer length                    */
  void* PTR32 desp_de_list_ptr;           /* DE_LIST address            @L2A  */
  int            desp_de_list2;             /* DE_LIST entry count        @L2A  */
  int            desp_entry_gap;            /* entry gap size                   */
  void * PTR32 desp_mem_data_ptr;         /* MEM_DATA address                 */
  int            desp_mem_data2;            /* MEM_DATA entry count             */
  struct desl * PTR32 desp_name_list_ptr; /* name list address                */
  int            desp_name_list2;           /* input list number of entries     */
  void * PTR32 desp_name_ptr;             /* name address               @L2A  */
  union {
    void * PTR32 desp_pdsde_ptr; /* bldl directory entry address    */
    void * PTR32 desp_smde_ptr;  /* input smde addr for GET    @L1A */
    };
  void * PTR32 desp_exit_dst_ptr;         /* DESERV exit screen table, DST    */
  void * PTR32 desp_exit_prev_dstptr_ptr; /* Address of pointer to previous   */
  };

/* Values for field "desp_func" */
#define desp_func_count_all_g      0x0E /* Count all generations      @L7A */
#define desp_func_get_all_g        0x0D /* Get all generations        @L7A */
#define desp_func_get_g            0x0C /* GET generation request     @L7A */
#define desp_func_close            0x0B /* close (igwcdcls)           @P4A */
#define desp_func_defer_release    0x0A /* PROCESS DEFERRED REQUEST   @05A */
#define desp_func_update           0x09 /* UPDATE request             @L2A */
#define desp_func_rename           0x08 /* RENAME request             @L2A */
#define desp_func_delete           0x07 /* DELETE request             @L2A */
#define desp_func_exit             0x06 /* EXIT request                    */
#define desp_func_get_names        0x05 /* GET_NAMES request          @L2A */
#define desp_func_put              0x04 /* PUT request                     */
#define desp_func_release          0x03 /* RELEASE request            @L2A */
#define desp_func_get_all          0x02 /* GET_ALL request            @L2A */
#define desp_func_get              0x01 /* GET request                     */
#define desp_func_omitted          0x00 /* NOT SPECIFIED              @L2A */

/* Values for field "desp_exit_scope" */
#define desp_exit_scope_task       0x01 /* SETUP A TASK EXIT               */
#define desp_exit_scope_global     0x00 /* SETUP A GLOBAL EXIT             */

/* Values for field "desp_libtype" */
#define desp_libtype_dcb           0x02 /* DCB was input                   */
#define desp_libtype_deb           0x01 /* DEB was input                   */
#define desp_libtype_omitted       0x00 /* NOT SPECIFIED                   */

/* Values for field "desp_gettype" */
#define desp_gettype_smde          0x03 /* GET input is SMDE          @L3A */
#define desp_gettype_pdsde         0x02 /* GET input is PDSDE              */
#define desp_gettype_name_list     0x01 /* GET input is name list          */
#define desp_gettype_omitted       0x00 /* NOT SPECIFIED                   */

/* Values for field "desp_reltype" */
#define desp_reltype_de_list       0x02 /* or RELTYPE                 @L2A */
#define desp_reltype_conn_id       0x01 /* @L2A                            */
#define desp_reltype_omitted       0x00 /* NOT SPECIFIED              @L2A */

/* Values for field "desp_exit_option" */
#define desp_exit_option_query     0x03 /* QUERY EXISTING EXIT             */
#define desp_exit_option_delete    0x02 /* DELETE EXISTING EXIT            */
#define desp_exit_option_replace   0x01 /* REPLACE EXISTING EXIT           */
#define desp_exit_option_noreplace 0x00 /* DO NOT REPLACE EXISTING EXIT    */

/* Values for field "desp_option" */
#define desp_option_replace        0x01 /* Replace alias or primary        */
#define desp_option_noreplace      0x00 /* Do not replace                  */

/* Values for field "desp_conn_intent" */
#define desp_conn_intent_input     0x03 /* connect for input               */
#define desp_conn_intent_exec      0x02 /* connect for exec                */
#define desp_conn_intent_hold      0x01 /* connect for hold                */
#define desp_conn_intent_none      0x00 /* do not connect                  */

/* Values for field "desp_entry_gap" */
#define desp_entry_gap_max         2048 /* maximum entry gap               */

/* Values for field "desp_exit_prev_dstptr_ptr" */
#define desp_len_iv                0x68 /* parm list length                */
#define desp_len_list              0x58 /* parm list length w/o header     */

struct desl {
  union {
    unsigned char  desl_entry[16]; /* name entry */
    struct {
      unsigned int   desl_module_buffered_lla : 1, /* module is staged by lla */
                                              : 7;
      unsigned char  desl_code;                    /* result code             */
      unsigned short desl_errcode;                 /* low order halfword of   */
      int            _filler1;                     /* reserved                */
      void * PTR32 desl_smde_ptr;                /* pointer to smde         */
      unsigned char  _filler2[4];
      };
    struct {
      unsigned char  _filler3[8];
      void * PTR32 desl_new_name_ptr; /* pointer to new name,       @L2A */
      struct desl_name* PTR32 desl_name_ptr; /* pointer to name (DESN)     */
      };
    struct {
      unsigned char  _filler4[12];
      void * PTR32 desl_old_name_ptr; /* pointer to old name,       @L2A */
      };
    };
  };

/* Values for field "desl_code" */
#define desl_code_newname_exists 0x03 /* for func=rename, indicates a */
#define desl_code_error          0x02 /* an unexpected error has      */
#define desl_code_notfound       0x01 /* entry not found or entry not */
#define desl_code_succ           0x00 /* entry successfully processed */

#define SMDE_NAME_MAXLEN (1024)
struct desb {
  union {
    unsigned char  desb_fixed[40];
    unsigned char  desb_header[16];
    struct {
      unsigned char  desb_id[8];      /* EYECATCHER (IGWDESB )     @03C */
      int            desb_len;        /* LENGTH OF BUFFER               */
      unsigned int               : 7,
                     desb_lev_iv : 1; /* buffer level                   */
      unsigned char  _filler1[3];     /* reserved                       */
      struct desb* PTR32 desb_next;   /* Next Buffer Pointer            */
      void * PTR32 _filler2;        /* RESERVED                       */
      int            desb_count;      /* count of entries in this       */
      void * PTR32 desb_avail;      /* Start of free space in buffer  */
      unsigned char  _filler3;        /* RESERVED                       */
      unsigned char  desb_subpool;    /* subpool number                 */
      short int      desb_gap_len;    /* length of user requested gap   */
      void * PTR32 _filler4;        /* RESERVED                       */
      __extension__ unsigned char  desb_data[0]; /* start of data area             */
      };
    };
  };

struct desn {
  short int      desn_len; /* Length of name that follows */
  __extension__ unsigned char  desn_val[0]; /* name data                   */
  };

struct desd {
  union {
    unsigned char  desd_entry[16]; /* entry descriptor */
    struct {
      unsigned int   desd_flag_alias : 1, /* alias entry                     */
                                     : 7;
      unsigned char  desd_code;           /* processing code                 */
      unsigned char  desd_errcode[2];     /* low order half word of          */
      short int      desd_index;          /* Index number for name. required */
      short int      desd_data_len;       /* length of data area             */
      void * PTR32 desd_data_ptr;       /* address of data                 */
      void * PTR32 desd_name_ptr;       /* address of varying length name  */
      };
    };
  };

/* Values for field "desd_code" */
#define desd_code_error 0x02 /* an unexpected error has    */
#define desd_code_nogo  0x01 /* the entry has not yet been */
#define desd_code_succ  0x00 /* successful processing      */

struct desx {
  union {
    unsigned char  desx_header[16]; /* Standard Header */
    struct {
      unsigned char  desx_id[8];      /* eyecatcher (IGWDESX) */
      int            desx_len;        /* length of buffer     */
      unsigned int               : 7,
                     desx_lev_iv : 1; /* parm list level      */
      unsigned char  _filler1[3];     /* RESERVED             */
      };
    };
  void * PTR32 desx_desp_ptr;     /* Address of DESP               */
  void * PTR32 desx_dst_ptr;      /* DESERV Screen Table, DST      */
  unsigned char  desx_caller_key;   /* High order nibble is DESERV's */
  unsigned int   desx_bldl_bit : 1, /* DESERV BLDL path              */
                 desx_pre_bit  : 1, /* Exit invoked prior to DESERV  */
                 desx_post_bit : 1, /* Exit invoked after DESERV     */
                               : 5;
  short int      _filler2;          /* RESERVED                      */
  int            desx_return_code;  /* User Exit rtn return code     */
  int            desx_reason_code;  /* User Exit rtn reason code     */
  };

/* Values for field "desx_reason_code" */
#define desx_len_iv 0x24 /* parm list length */

struct dst {
  union {
    unsigned char  dst_header[15]; /* Standard Header */
    struct {
      unsigned char  dst_id[8];          /* eyecatcher (IGWDST) */
      int            dst_len;            /* length of buffer    */
      unsigned int                  : 7,
                     dst_lev_iv     : 1; /* parm list level     */
      unsigned int                  : 7,
                     dst_flags_prop : 1; /* Propagate           */
      unsigned char  dst_res[2];         /* RESERVED            */
      };
    };
  void * PTR32 dst_exit_ptr; /* Address of exit routine */
  };

/* Values for field "dst_exit_ptr" */
#define dst_len_iv 0x14 /* parm list length */

struct desrcs {
  unsigned char  desrc[4]; /* return code */
  };

/* Values for field "desrc" */
#define desrc_succ 0x00 /* successful processing     */
#define desrc_info 0x04 /* not completely successful */
#define desrc_warn 0x08 /* Results questionable      */
#define desrc_parm 0x0C /* Missing/invalid parms     */
#define desrc_calr 0x10 /* Caller has a problem      */
#define desrc_envr 0x14 /* Resources unavailable     */
#define desrc_ioer 0x18 /* I/O error                 */
#define desrc_mede 0x1C /* Media error               */
#define desrc_dsle 0x20 /* Data Set logical error    */
#define desrc_seve 0x24 /* Severe error              */

struct desr {
  unsigned char  desr_compid; /* DFP component id           */
  unsigned char  desr_modid;  /* module id within component */
  unsigned char  desrs[2];    /* reason code                */
  };

/* Values for field "desrs" */
#define desrs_dataset_mismatch         0x46C /* A file token for a            */
#define desrs_name_list_length_not12   0x46B /* Name list length not 12@L7A   */
#define desrs_concat_not_one           0x46A /* More than one PDSE in  @L7A   */
#define desrs_dst_exit_ptr_not_common  0x469 /* GLOBAL EXIT ADDRESS IS @12A   */
#define desrs_exec_only_concat         0x468 /* Attempt to connect to  @11A   */
#define desrs_uss_not_supported        0x467 /* USS directories not supp.@L5A */
#define desrs_bad_dcii                 0x466 /* DCII EYE CATCHER BAD   @09A   */
#define desrs_igwlock_failed           0x465 /* A call to IGWLOCK to get a    */
#define desrs_mem_counts_concat_all    0x464 /* MEMBER_COUNTS and CONCAT(ALL) */
#define desrs_pacb_ptr_zero            0x463 /* An igwftokm extract returned  */
#define desrs_early_eof                0x462 /* A C370LIB DIRECTORY IS BAD.   */
#define desrs_bad_esd_name             0x461 /* A C370LIB DIRECTORY IS BAD.   */
#define desrs_igwlsrxl_failed          0x460 /* A call to RELEASE             */
#define desrs_igwlsoxl_failed          0x45F /* A call to OBTAIN  the         */
#define desrs_igwlsixl_failed          0x45E /* A call to initialize          */
#define desrs_igwlsnxl_failed          0x45D /* A call to nullify the         */
#define desrs_defer_anchor_obtain      0x45C /* AN ERROR OCCURED PROCESSING   */
#define desrs_defer_anchor_release     0x45B /* AN ERROR OCCURED PROCESSING   */
#define desrs_defer_anchor_purge       0x45A /* AN ERROR OCCURED PROCESSING   */
#define desrs_defer_element_purge      0x459 /* AN ERROR OCCURED PROCESSING   */
#define desrs_member_pend_delete       0x458 /* A FUNC=PUT request specified  */
#define desrs_long_name_not_original   0x457 /* A FUNC=RENAME request         */
#define desrs_new_name_exists          0x456 /* A FUNC=RENAME request         */
#define desrs_both_names_big           0x455 /* A FUNC=RENAME request         */
#define desrs_both_names_same          0x454 /* A FUNC=RENAME request         */
#define desrs_inval_prevdst_header     0x453 /* The DST header is invalid     */
#define desrs_prev_dstptr_ptr_zero     0x452 /* The pointer to the previous   */
#define desrs_dst_comp_swap_failed     0x451 /* AN EXIT_OPTION=DELETE         */
#define desrs_exit_scope_invalid       0x450 /* The EXIT_SCOPE specified      */
#define desrs_exit_option_invalid      0x44F /* The EXIT_OPTION specified     */
#define desrs_exit_prev_dstptr_zero    0x44E /* The EXIT_PREV_DSTPTR parm     */
#define desrs_inval_dst_header         0x44D /* The DST header is not         */
#define desrs_exit_dst_ptr_zero        0x44C /* A ZERO DESP_EXIT_DST_PTR      */
#define desrs_dst_already_exists       0x44B /* An EXIT exists and DESERV     */
#define desrs_exit_error               0x44A /* Invalid return code           */
#define desrs_auth_error               0x449 /* Caller not supervisor state,  */
#define desrs_pacbloc_error            0x448 /* A call to the pacb_locate     */
#define desrs_unknown                  0x447 /* Issued by DESERV recovery     */
#define desrs_getmain_error            0x446 /* A call to GETMAIN failed      */
#define desrs_cant_change_recovery     0x445 /* A call to IGWFECHG failed     */
#define desrs_igwcdfnc_error           0x444 /* A call to IGWCDFNC failed@L1C */
#define desrs_no_pnar_for_name         0x443 /* A name record was obtained    */
#define desrs_unexpected_sar_code      0x442 /* An unexpected Standard        */
#define desrs_two_mlt_names            0x441 /* Two mlt names were found for  */
#define desrs_pdsde_merge_err          0x440 /* PDSDE merge into PMAR failed  */
#define desrs_unknown_exit_error       0x43F /* Issued by the DESERV recovery */
#define desrs_ecb_posted_error         0x43E /* An I/O error occurred,        */
#define desrs_connid_overflow          0x43D /* The maximum allowable         */
#define desrs_name_is_primary_name     0x43C /* Alias name specified is a     */
#define desrs_clock_error              0x43B /* An STCK instruction failed    */
#define desrs_data_length_error        0x43A /* Desd data length is invalid,  */
#define desrs_anchor_in_use            0x439 /* An obtain for a DCLA was      */
#define desrs_anchor_free              0x438 /* A purge for a DCLA was        */
#define desrs_add_stack_failed         0x437 /* Non-zero return code from     */
#define desrs_invalid_conn_intent      0x436 /* The connect intent            */
#define desrs_name_list___invalid      0x435 /* The address of the NAME_LIST  */
#define desrs_name_list_count_invalid  0x434 /* The count of entries in       */
#define desrs_connid_invalid           0x433 /* The connect identifier in     */
#define desrs_invalid_gettype          0x432 /* The GET function accepts      */
#define desrs_invalid_area_ptr         0x431 /* The address of a DESB         */
#define desrs_invalid_areaptr_ptr      0x430 /* The address of the AREAPTR    */
#define desrs_area_length_too_small    0x42F /* The length of the area        */
#define desrs_invalid_entry_gap        0x42E /* Gap specfied was too large.   */
#define desrs_invalid_de_list_ptr      0x42D /* The address of the DE_LIST    */
#define desrs_invalid_de_list_cnt      0x42C /* The number of entries in the  */
#define desrs_invalid_conn_id_ptr      0x42B /* The address of the CONN_ID    */
#define desrs_invalid_release_type     0x42A /* The RELEASE function          */
#define desrs_invalid_put_option       0x429 /* PUT function requires         */
#define desrs_invalid_mem_data_ptr     0x428 /* The address of the MEM_DATA   */
#define desrs_invalid_mem_data_cnt     0x427 /* The count of entries in       */
#define desrs_invalid_name_ptr         0x426 /* The address of the NAME       */
#define desrs_invalid_name_length      0x425 /* The length of a name          */
#define desrs_unsupported_func         0x424 /* The FUNC value is incorrect   */
#define desrs_deb_requires_auth        0x423 /* To pass the DEB the caller    */
#define desrs_invalid_dcb_ptr          0x422 /* The address of the DCB is 0   */
#define desrs_dcb_not_open             0x421 /* The passed DCB is not opened  */
#define desrs_comp_name_bad            0x420 /* A compressed name is          */
#define desrs_dcb_not_open_output      0x41F /* With function PUT the DCB     */
#define desrs_invalid_deb_ptr          0x41E /* Address of the DEB is 0 or    */
#define desrs_debchk_failed            0x41D /* The DEBCHK macro failed.      */
#define desrs_invalid_concat           0x41C /* The concatenation number      */
#define desrs_pdsde_ptr_invalid        0x41B /* Address of the PDSDE is 0     */
#define desrs_invalid_ft               0x41A /* File token is invalid         */
#define desrs_lib_not_buffered         0x419 /* ** INTERNAL CODE **           */
#define desrs_invalid_dfnc_func        0x418 /* An internal function call     */
#define desrs_disconnect_file_failed   0x417 /* A call to disconnect files    */
#define desrs_insuf_buffer_size        0x416 /* Area provided is too small    */
#define desrs_pds_not_supported        0x415 /* This function requires a      */
#define desrs_invalid_ct               0x414 /* Connect token invalid.        */
#define desrs_invalid_mlt              0x413 /* MLT is not valid (PUT func).  */
#define desrs_more_than_1_primary      0x412 /* The MEM_DATA must have only   */
#define desrs_invalid_parm_list_header 0x411 /* The id, length, or level      */
#define desrs_invalid_name_prefix      0x410 /* The first 8 bytes of a name   */
#define desrs_no_primary_name          0x40F /* The MEM_DATA must have one    */
#define desrs_name_already_exists      0x40E /* The PUT failed because of a   */
#define desrs_long_name_and_lla        0x40D /* LLA does not support long     */
#define desrs_program_object_only      0x40C /* Function supports PDSE        */
#define desrs_freemain_error           0x40B /* FREEMAIN failure              */
#define desrs_convert_error            0x40A /* Error converting TTR          */
#define desrs_unexpected_jcdm_error    0x409 /* JCDM returned an unexpected   */
#define desrs_pdsde_convrt_err         0x408 /* Error converting PDSDE        */
#define desrs_setlock_err              0x407 /* Bad return code from SETLOCK  */
#define desrs_extract_error            0x406 /* IGWFTOKM EXTRACT failed       */
#define desrs_set_error                0x405 /* IGWFTOKM SET failed           */
#define desrs_anchor_create_failed     0x404 /* Number of CONNIDs exceeded    */
#define desrs_element_create_failed    0x403 /* No space to store connections */
#define desrs_pmar_merge_err           0x402 /* PMARA to PMAR merge failed    */
#define desrs_iewlcnvt_error           0x401 /* IEWLCNVT macro call error@L1C */
#define desrs_name_not_defined         0x400 /* name to be replaced did       */
#define desrs_changed_pmar_length      0x3FF /* For an UPDATE function, the   */
#define desrs_smde_token_bad           0x3FE /* The smde input defined a      */
#define desrs_pri_nm_this_file         0x3FD /* alias name is same name as    */
#define desrs_cant_get_filelock        0x3FC /* file lock unavailable, pos-   */
#define desrs_connection_not_found     0x3FB /* The connection specified in   */
#define desrs_desl_smde_ptr            0x3FA /* The SMDE for the release      */
#define desrs_reltype_invalid          0x3F9 /* The release function was      */
#define desrs_bad_pmar_field_updated   0x3F8 /* For the DESERV function       */
#define desrs_iewbxilo_error           0x3F7 /* a call to iewbxilo returned   */
#define desrs_multiple_errors          0x3F6 /* MORE THAN ONE ERROR HAS @L1C  */
#define desrs_auth_subpool             0x3F5 /* Caller specified an           */
#define desrs_directory_empty          0x3F4 /* No members in directory       */
#define desrs_connect_auth             0x3F3 /* A DESERV GET_ALL call was     */
#define desrs_c370lib_pdsde_me         0x3F2 /* C370LIB(YES) and PDSDE are    */
#define desrs_bad_blksize              0x3F1 /* DCBBLKSI is too small   @L3A  */
#define desrs_bad_txt_card             0x3F0 /* while processing a C370LIB    */
#define desrs_bad_c370lib_dir          0x3EF /* the C370LIB directory         */
#define desrs_dcb_not_open_po          0x3EE /* the DCB was not opened with   */
#define desrs_area_areaptr_me          0x3ED /* AREA and AREAPTR are mutually */
#define desrs_smde_ptr_invalid         0x3EC /* For GETTYPE=SMDE, the input   */
#define desrs_c370lib_smde_me          0x3EB /* The SMDE parameter is         */
#define desrs_notfound                 0x3EA /* Some members not found        */
#define desrs_module_buffered_lla      0x3E9 /* The module is buffered        */
#define desrs_succ                     0x00  /* successful processing         */

#pragma pack(pop)

#endif // __DESERV__H

