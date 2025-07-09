#ifndef __SMDE_H__
#define __SMDE_H__ 1

#include "metaldio.h"

#pragma pack(1)

struct smde {
  union {
    unsigned char  smde_basic[44]; /* START OF BASIC SECTION */
    unsigned char  smde_hdr[16];   /* HEADER                 */
    struct {
      unsigned char  smde_id[8];          /* EYECATCHER IGWSMDE                 */
      int            smde_len;            /* LENGTH OF CONTROL BLOCK. THIS      */
      unsigned int                   : 7,
                     smde_lvl_val    : 1; /* LEVEL CONSTANT                     */
      unsigned char  _filler1[3];         /* RESERVED                           */
      unsigned char  smde_libtype;        /* SOURCE LIBRARY TYPE. POSSIBLE      */
      unsigned int   smde_flag_alias : 1, /* ENTRY IS AN ALIAS.                 */
                     smde_flag_lmod  : 1, /* MEMBER IS A PROGRAM.               */
                     smde_system_dcb : 1, /* DCB IS A SYSTEM DCB, THEREFORE     */
                                     : 5;
      unsigned char  _filler2[2];         /* RESERVED. MUST BE ZERO.            */
      unsigned char  _filler3;            /* RESERVED. MUST BE ZERO.            */
      struct {
        unsigned char  smde_mlt[3]; /* MLT OF MEMBER (ZERO IF HFS)        */
        unsigned char  smde_cnct;   /* CONCATENATION NUMBER               */
        } smde_mltk;                /* MLT AND CONCAT #                   */
      unsigned char  smde_libf;           /* LIBRARY FLAG (Z-BYTE)              */
      short int      smde_name_off;       /* NAME OFFSET                        */
      struct {
        short int      smde_pmar_len; /* SUM OF LENGTHS OF PROGRAM          */
        } smde_usrd_len;              /* USER DATA LENGTH                   */
      struct {
        short int      smde_pmar_off; /* PROGRAM MANAGEMENT                 */
        } smde_usrd_off;              /* USER DATA OFFSET                   */
      struct {
        short int      smde_gene_len; /* GENERATION SECTION LENGTH   @L4A   */
        } smde_token_len;             /* TOKEN SECTION LENGTH        @L4C   */
      struct {
        short int      smde_gene_off; /* GENERATION SECTION OFFSET   @L4A   */
        } smde_token_off;             /* TOKEN DATA OFFSET           @L4C   */
      short int      smde_pname_off;      /* PRIMARY NAME OFFSET.               */
      short int      smde_nlst_cnt;       /* NUMBER OF NOTE LIST                */
      short int      smde_c370_attr_off;  /* OFFSET TO C370LIB ATTRIBUTE @02A   */
      short int      smde_ext_attr_off;   /* OFFSET TO EXTENDED ATTRIBUTES @L3A */
      __extension__ unsigned char  smde_sections[0]; /* START OF ENTRY SECTIONS            */
      };
    };
  };

/* Values for field "smde_libtype" */
#define smde_libtype_c370lib 0x03 /* C370LIB LIBRARY TYPE       @L2A */
#define smde_libtype_hfs     0x02 /* HFS FILE TYPE                   */
#define smde_libtype_pdse    0x01 /* PDSE LIBRARY TYPE               */
#define smde_libtype_pds     0x00 /* PDS LIBRARY TYPE                */

/* Values for field "smde_libf" */
#define smde_libf_tasklib    0x02 /* LIBRARY FOUND FLAG - TASKLIB    */
#define smde_libf_linklib    0x01 /* LIBRARY FOUND FLAG - LNKLST     */
#define smde_libf_private    0x00 /* LIBRARY FOUND FLAG - PRIVATE    */

struct smde_name {
  short int      smde_name_len; /* LENGTH OF ENTRY NAME */
  __extension__ unsigned char  smde_name_val[0]; /* ENTRY NAME           */
  };

struct smde_nlst {
  union {
    unsigned char  smde_nlst_entry[4]; /* NOTE LIST ENTRIES */
    struct {
      unsigned char  smde_nlst_rlt[3]; /* NOTE LIST RECORD LOCATION  */
      unsigned char  smde_nlst_num;    /* NUMBER OF RLT DESCRIBED BY */
      };
    };
  };

struct smde_token {
  union {
    struct {
      int            smde_token_connid; /* CONNECT_IDENTIFIER */
      int            smde_token_itemno; /* ITEM NUMBER        */
      unsigned char  smde_token_ft[24]; /* FILE TOKEN         */
      };
    struct {
      unsigned char  _filler1[16];         /* RESERVED                    @L2A */
      unsigned char  smde_token_bmf_ct[8]; /* BMF CONNECT TOKEN           @L2A */
      unsigned char  smde_token_cdm_ct[8]; /* JCDM CONNECT TOKEN          @L2A */
      __extension__ unsigned char  smde_token_end[0];
      };
    };
  };

struct smde_fd {
  int            smde_fd_token; /* FILE DESCRIPTOR */
  __extension__ unsigned char  smde_fd_end[0];
  };

struct smde_pname {
  short int      smde_pname_len; /* LENGTH OF PRIMARY NAME */
  __extension__ unsigned char  smde_pname_val[0]; /* PRIMARY NAME           */
  };

struct smde_gene {
  unsigned char  smde_gene_name[8];         /* Generation Name              */
  unsigned int   smde_is_dummy         : 1, /* Entry is dummy               */
                                       : 7;
  unsigned char  smde_gene_flgs2;           /* Flags                        */
  unsigned char  smde_gene_flgs3;           /* Flags                        */
  unsigned char  smde_gene_flgs4;           /* Flags                        */
  int            smde_gene_number;          /* Absolute Generation number   */
  union {
    unsigned char  smde_gene_mltk[4]; /* MLT AND CONCAT # */
    struct {
      unsigned char  smde_gene_mlt[3]; /* MLT OF MEMBER        */
      unsigned char  smde_gene_cnct;   /* CONCATENATION NUMBER */
      };
    };
  short int      smde_gene_ccsid;           /* CCSID or '0000'x if not      */
  unsigned char  _filler1[2];               /* Reserved                     */
  unsigned char  smde_gene_userid[8];       /* Userid of creator or last    */
  unsigned char  smde_gene_timestamp[8];    /* Last member update or create */
  unsigned char  smde_gene_gentimestamp[8]; /* Last generation update       */
  __extension__ unsigned char  smde_gene_end[0]; /* @03A                         */
  };

/* Values for field "smde_gene_end" */
#define smde_po1_name_maxlen 63    /* Maximum length of names in a   */
#define smde_hfs_name_maxlen 256   /* Maximum length of names in a   */
#define smde_po2_name_maxlen 1024  /* Maximum length of names in a   */
#define smde_name_maxlen     1024  /* Maximum length of names   @L1A */
#define smde_pname_maxlen    8     /* Maximum length of primary      */
#define smde_maxlen          0x11F
#define smde_hfs_maxlen      0x4BA
#define smde_po2_maxlen      0x4E0
#define smde_all_maxlen      0x4E0 /* Maximum length of SMDE for all */
#define smde_pds_maxlen      0x42

struct smde_ext_attr {
  short int      smde_ext_attr_len;          /* length of this section        @L3a */
  unsigned char  smde_ccsid[2];              /* CCSID, or x'0000' if CCSID was     */
  unsigned char  smde_type_descriptor[16];   /* type descriptor               @L3a */
  unsigned char  smde_userid_last_change[8]; /* userid of creator or last updater  */
  unsigned char  smde_change_timestamp[8];   /* last member update or creation@L3a */
  };

struct pmar {
  union {
    unsigned char  pmar_entry[30]; /* Alternative name for the PMAR section */
    struct {
      short int      pmar_slen;                /* Section length.                       */
      unsigned char  pmar_lvl;                 /* PMAR format level                     */
      unsigned char  pmar_plvl;                /* Bind processor creating object        */
      struct {
        unsigned int   pmar_rent : 1, /* Reenterable                           */
                       pmar_reus : 1, /* Reusable                              */
                       pmar_ovly : 1, /* Overlay structure                     */
                       pmar_test : 1, /* Module to be tested - TESTRAN         */
                       pmar_load : 1, /* Only loadable                         */
                       pmar_sctr : 1, /* Scatter format                        */
                       pmar_exec : 1, /* Executable                            */
                       pmar_1blk : 1; /* Load module contains only one         */
        unsigned int   pmar_flvl : 1, /* If on, the program cannot be          */
                       pmar_org0 : 1, /* Linkage editor assigned origin        */
                                 : 1,
                       pmar_nrld : 1, /* Program contains no RLD items         */
                       pmar_nrep : 1, /* Module cannot be reprocessed          */
                       pmar_tstn : 1, /* Module contains TESTRAN symbol        */
                                 : 1,
                       pmar_refr : 1; /* Refreshable program                   */
        struct {
          unsigned int               : 1,
                         pmar_big    : 1, /* This program requires 16M bytes       */
                         pmar_paga   : 1, /* Page alignment is required            */
                         pmar_xssi   : 1, /* SSI information present               */
                         pmar_xapf   : 1, /* APF information present               */
                         pmar_lfmt   : 1, /* PMARL follows PMAR.                   */
                         pmar_signed : 1, /* Program is signed. Verified on        */
                                     : 1;
          } pmar_atr3;                    /* Third attribute byte.                 */
        struct {
          unsigned char  pmar_ftb2; /* Alternative name for flags byte       */
          } pmar_atr4;              /* Fourth attribute byte                 */
        } pmar_atr;                   /* Attribute bytes.                      */
      unsigned int                        : 7,
                     pmar_longparm        : 1; /* Parm >100 chars allowed     @LFA      */
      unsigned char  pmar_ac;                  /* APF authorization code                */
      unsigned char  pmar_stor[4];             /* Virtual storage required              */
      unsigned char  pmar_epm[4];              /* Main entry point offset               */
      unsigned char  pmar_epa[4];              /* This entry point offset               */
      struct {
        unsigned char  pmar_chlv;    /* Change level of member                */
        unsigned char  pmar_ssfb;    /* SSI flag byte                         */
        unsigned char  pmar_mser[2]; /* Member serial number                  */
        } pmar_ssi;                  /* SSI information                       */
      unsigned int   pmar_system_le       : 1, /* @L7A                                  */
                     pmar_lightweight_le  : 1, /* @L7A                                  */
                                         : 14;
      unsigned char  _filler1[2];              /* Reserved                    @L7A      */
      __extension__ unsigned char  pmar_end[0]; /* END OF BASIC SECTION                  */
      };
    };
  };

/* Values for field "pmar_lvl" */
#define pmar_pm1_val      0x01 /* level constant for PO1      @L2A */
#define pmar_pm2_val      0x02 /* level constant for PO2      @L2A */
#define pmar_pm3_val      0x03 /* level constant for PO3      @L3A */
#define pmar_pm4_val      0x04 /* level constant for PO4      @L7A */
#define pmar_pm5_val      0x05 /* level constant for PO5      @LAA */
#define pmar_lvl_val      0x05 /* level constant              @LAC */

/* Values for field "pmar_plvl" */
#define pmar_plvl_e_val   0x01 /* E-level constant                 */
#define pmar_plvl_f_val   0x02 /* F-level constant                 */
#define pmar_plvl_aos_val 0x03 /* AOS-level constant               */
#define pmar_plvl_xa_val  0x04 /* XA-level constant                */
#define pmar_plvl_b1_val  0x05 /* Binder version 1                 */
#define pmar_plvl_b2_val  0x06 /* Binder version 2            @L2A */
#define pmar_plvl_b3_val  0x07 /* Binder version 3            @L3A */
#define pmar_plvl_b4_val  0x08 /* Binder version 4            @L7A */
#define pmar_plvl_b5_val  0x09 /* Binder version 5            @L8A */

/* Values for field "pmar_ftb2" */
#define pmar_altp         0x80 /* Alternate primary flag. If on    */
#define pmar_rmode64      0x20 /* RMODE 64                    @LGC */
#define pmar_rmod         0x10 /* RMODE is 31 or 64                */
#define pmar_aamd         0x0C /* Alias entry point addressing     */
#define pmar_aamd_maskoff 0xF3 /* Mask for AMODE flags             */
#define pmar_mamd         0x03 /* Main entry point addressing      */

struct pmarl {
  short int      pmarl_slen; /* Section length */
  union {
    unsigned char  pmarl_data[48]; /* Section Data    */
    unsigned char  pmarl_atr[4];   /* Attribute bytes */
    struct {
      unsigned char  pmarl_atr1;      /* 6th attribute byte               */
      unsigned int   pmarl_cmpr  : 1, /* Compressed format module         */
                     pmarl_1rmod : 1, /* 1st segment is RMODE 31,    @L2A */
                     pmarl_2rmod : 1, /* 2nd segment is RMODE 31,    @L2A */
                                 : 1,
                     pmarl_1alin : 1, /* 1st segment is page-aligned,@L2A */
                     pmarl_2alin : 1, /* 2nd segment is page-aligned,@L2A */
                     pmarl_fill  : 1, /* FILL option specified       @L2A */
                                 : 1;
      unsigned char  pmarl_fillval;   /* FILL character value        @L2A */
      unsigned char  pmarl_po_sublvl; /* Program object sublevel     @L7A */
      unsigned char  pmarl_mpgs[4];   /* Total length of program on       */
      struct {
        unsigned char  pmarl_txtl[4]; /* Length of initial load text on   */
        void * PTR32 pmarl_txto;    /* Offset to text                   */
        unsigned char  pmarl_bdrl[4]; /* Length of Binder index           */
        void * PTR32 pmarl_bdro;    /* Offset to Binder index           */
        unsigned char  pmarl_rdtl[4]; /* Length of PRDT                   */
        void * PTR32 pmarl_rdto;    /* Offset to PRDT                   */
        unsigned char  pmarl_ratl[4]; /* Length of PRAT                   */
        void * PTR32 pmarl_rato;    /* Offset to PRAT                   */
        struct {
          unsigned char  pmarl_lmdl[4]; /* Length of LSLoader data,         */
          } pmarl_nvspgs;               /* Number of virtual storage   @L2A */
        void * PTR32 pmarl_lmdo;    /* Offset to LSLoader data          */
        } pmarl_mdat;                 /* DASD program descriptors         */
      };
    struct {
      unsigned char  _filler1[48];
      struct {
        unsigned char  pmarl_nseg[2];  /* Number of loadable segments @L2A */
        unsigned char  pmarl_ngas[2];  /* Count of entries in Gas     @L2A */
        unsigned char  pmarl_1stor[4]; /* Virtual storage required    @L2A */
        unsigned char  pmarl_2stor[4]; /* Virtual storage required    @L2A */
        unsigned char  pmarl_2txto[4]; /* Offset to second txt segment@L2A */
        unsigned char  _filler2[8];
        } pmarl_pm2;                   /* New fields for PM2-Level    @L2A */
      };
    struct {
      unsigned char  _filler3[64];
      struct {
        unsigned char  pmarl_date[4]; /* Date saved                  @L2A */
        unsigned char  pmarl_time[4]; /* Time saved                  @L2A */
        unsigned char  pmarl_user[8]; /* User or job identification  @L2A */
        } pmarl_trace;                /* Audit trace data            @L2A */
      };
    struct {
      unsigned char  _filler4[80];
      struct {
        unsigned int   pmarl_hide        : 1, /* Name is an alias that can   @L3A          */
                       pmarl_dllena      : 1, /* PO is DLL-enabled           @L3A          */
                       pmarl_mustdelet   : 1, /* If on and directed LOAD     @L3A          */
                       pmarl_iewblitp    : 1, /* If on, PMARL_IEWBLITO is    @L3A          */
                       pmarl_mangled     : 1, /* If on, name is mangled.     @L3A          */
                                         : 3;
        unsigned int   pmarl_cms_system  : 1, /* SYSTEM module bit                    @L6A */
                       pmarl_cms_noclean : 1, /* Do not cleanup at end of service     @L6A */
                       pmarl_cms_strinit : 1, /* STRINIT bit                          @L6A */
                       pmarl_cms_moddos  : 1, /* Gen'd with DOS                       @L6A */
                       pmarl_cms_modall  : 1, /* Gen'd with ALL                       @L6A */
                       pmarl_cms_invalxa : 1, /* XA-mode invalid                      @L6A */
                       pmarl_cms_invalxc : 1, /* XC-mode invalid                      @L6A */
                                         : 1;
        unsigned char  pmarl_ndefer[2];       /* Number of deferred classes  @L3A          */
        unsigned char  pmarl_dtempl[4];       /* Total length of deferred    @L3A          */
        unsigned char  pmarl_1dtxto[4];       /* Offset of 1st deferred      @L3A          */
        unsigned char  pmarl_iewblito[4];     /* Byte offset of IEWBLIT      @L3A          */
        } pmarl_pm3;                          /* New fields for PM3-Level    @L3A          */
      };
    struct {
      unsigned char  _filler5[96];
      union {
        unsigned char  pmarl_pm4[8]; /* New fields for PM4-Level    @L7A */
        struct {
          unsigned int   pmarl_1rmod64 : 1, /* 1st segment is RMODE 64     @L8A */
                         pmarl_2rmod64 : 1, /* 2nd segment is RMODE 64     @L8A */
                                       : 6;
          unsigned char  _filler6[7];       /* Reserved                    @L7A */
          __extension__ unsigned char  pmarl_pm5[0]; /* New fields for PM5-Level    @LAA */
          __extension__ unsigned char  pmarl_end[0]; /* END OF LSLOADER SECTION          */
          };
        };
      };
    };
  };

/* Values for field "pmarl_atr1" */
#define pmarl_nmig               0x80 /* This program object cannot be    */
#define pmarl_prim               0x40 /* FETCHOPT PRIME option            */
#define pmarl_pack               0x20 /* FETCHOPT PACK option             */
#define pmarl_xpl                0x10 /* Module requires XPLINK      @L4A */
#define pmarl_hpl                0x10 /* Module requires XPLINK      @L4A */

/* Values for field "pmarl_po_sublvl" */
#define pmarl_po_sublvl_zosv1r3  1    /* Value for z/OS V1 R3 / PO4  @L7A */
#define pmarl_po_sublvl_zosv1r5  2    /* Value for z/OS V1 R5 / PO4  @L8A */
#define pmarl_po_sublvl_zosv1r7  3    /* Value for z/OS V1 R7 / PO4  @L9A */
#define pmarl_po_sublvl_zosv1r8  1    /* Value for z/OS V1 R8 / PO5  @LAA */
#define pmarl_po_sublvl_zosv1r10 2    /* Value for z/OS V1 R10 / PO5 @LBA */
#define pmarl_po_sublvl_zosv1r13 3    /* Value for z/OS V1 R13 / PO5 @LDA */
#define pmarl_po_sublvl_zosv2r1  4    /* Value for z/OS V2 R1 / PO5  @LEA */

struct pmarr {
  short int      pmarr_slen; /* Section length */
  union {
    unsigned char  pmarr_data[21]; /* Section data */
    unsigned char  pmarr_ttrs[8];  /* TTR fields   */
    struct {
      unsigned char  pmarr_ttrt[3]; /* TTR of first block of text     */
      unsigned char  pmarr_zero;    /* Zero                           */
      unsigned char  pmarr_ttrn[3]; /* TTR of note list or scatter    */
      unsigned char  pmarr_nl;      /* Number of entries in note list */
      unsigned char  pmarr_ftbl[2]; /* Length of first block of text. */
      struct {
        unsigned char  _filler1[2]; /* Reserved                        */
        unsigned char  pmarr_rlds;  /* Number of RLD/CTL records which */
        } pmarr_org;                /* Load module origin if ^0        */
      union {
        unsigned char  pmarr_scat[8]; /* Scatter load information       */
        struct {
          unsigned char  pmarr_slsz[2]; /* Scatter list length            */
          unsigned char  pmarr_ttsz[2]; /* Translation table length       */
          unsigned char  pmarr_esdt[2]; /* ESDID of first text block      */
          unsigned char  pmarr_esdc[2]; /* ESDID of EP control section    */
          __extension__ unsigned char  pmarr_end[0]; /* END OF LOAD MODULE ATTRIBUTES  */
          };
        };
      };
    };
  };

struct pmara {
  short int      pmara_len; /* Section length */
  union {
    unsigned char  pmara_data;   /* Section data       */
    unsigned char  pmara_epa[4]; /* Entry point offset */
    struct {
      unsigned char  _filler1[4];
      union {
        unsigned char  pmara_atr;  /* Attribute bytes      */
        unsigned char  pmara_atr1; /* First attribute byte */
        struct {
          unsigned int   pmara_altp    : 1, /* Alternate Primary flag.          */
                         pmara_hide    : 1, /* Alias name can be hidden    @L3A */
                         pmara_nexec   : 1, /* Entry point is non-executable    */
                         pmara_mangled : 1, /* Alias is a mangled name     @L3A */
                         pmara_amd     : 2, /* Alias entry addressing mode      */
                                       : 2;
          __extension__ unsigned char  pmara_end[0]; /* END OF ALIAS ENTRY SECTION       */
          };
        };
      };
    };
  };

/* Values for field "pmara_end" */
#define pmar_maxlen         0x88
#define pmar_maxlen_progobj 0x88
#define pmar_maxlen_po1     0x50
#define pmar_maxlen_po2     0x70 /* @L3C */
#define pmar_maxlen_po3     0x80 /* @L7C */
#define pmar_maxlen_po4     0x88 /* @LAC */
#define pmar_maxlen_po5     0x88 /* @LAA */
#define pmar_maxlen_loadmod 0x35
#define pmarl_lvl1len       0x32 /* @L2A */
#define pmarl_lvl2len       0x52 /* @L3A */
#define pmarl_lvl3len       0x62 /* @L7A */
#define pmarl_lvl4len       0x6A /* @LAA */
#define pmarl_lvl5len       0x6A /* @LAA */

#pragma pack(pop)

#endif // __SMDE_H__

