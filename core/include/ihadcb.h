#ifndef __IHADCB__
#define __IHADCB__ 1

#include "metaldio.h"

#include <stdio.h>
#include "dcbe.h"
#include "dbgopts.h"

#pragma pack(1)
struct ihadcb {
  union {
    struct dcbe* PTR32 dcbdcbe;     /* DCBE ADDRESS ASSOCIATED WITH THIS    @L5A */
    unsigned char  dcbrelad[4]; /* --- PARTITIONED ORGANIZATION DATA SET -   */
    };
  char           dcbkeycn;     /* KEYED BLOCK OVERHEAD CONSTANT             */
  union {
    unsigned char  dcbfdad[8]; /* FULL DISK ADDRESS IN THE FORM OF MBBCCHHR */
    struct {
      unsigned char  _filler1[7];
      struct {
        unsigned char  _filler2;      /* LAST BYTE OF DCBFDAD                      */
        unsigned int   dcbdvtba : 24; /* ADDRESS OF ENTRY IN I/O DEVICE            */
        } dcbdvtbl;                   /* SAME AS DCBDVTBA BELOW                    */
      };
    struct {
      unsigned char  _filler3[11];
      struct {
        char           dcbkeyle;    /* KEY LENGTH OF DATA SET                    */
        struct {
          unsigned char  _filler4;
          } dcbdevt;                  /* DEVICE TYPE                               */
        unsigned char  _filler5[2];
        } dcbrelb;                  /* SAME AS DCBREL BELOW                      */
      };
    struct {
      unsigned char  _filler6[11];
      char           _filler7;     /* DCBKEYLE - KEY LENGTH OF DATA SET         */
      unsigned char  _filler8;     /* DCBDEVT - DEVICE TYPE                     */
      short int      dcbtrbal;     /* CODE INDICATING CAPACITY REMAINING ON     */
      };
    struct {
      unsigned char  _filler9[12];
      int            dcbrel : 24;  /* NUMBER OF RELATIVE TRACKS OR BLOCKS IN    */
      struct {
        char           dcbbufno;      /* NUMBER OF BUFFERS REQUIRED FOR THIS DATA  */
        unsigned int   dcbbufca : 24; /* ADDRESS OF BUFFER POOL CONTROL BLOCK      */
        } dcbbufcb;                   /* ADDRESS OF BUFFER POOL CONTROL BLOCK      */
      };
    };
  short int      dcbbufl;      /* LENGTH OF BUFFER.  MAY RANGE FROM 0 TO    */
  union {
    unsigned char  dcbdsorg[2]; /* DATA SET ORGANIZATION BEING USED */
    struct {
      unsigned int   dcbdsgis : 1, /* IS - INDEXED SEQUENTIAL ORGANIZATION      */
                      dcbdsgps : 1, /* PS - PHYSICAL SEQUENTIAL ORGANIZATION     */
                      dcbdsgda : 1, /* DA - DIRECT ORGANIZATION                  */
                      dcbdsgcx : 1, /* CX - BTAM OR QTAM LINE GROUP              */
                              : 2,
                      dcbdsgpo : 1, /* PO - PARTITIONED ORGANIZATION             */
                      dcbdsgu  : 1; /* U  - UNMOVABLE, THE DATA CONTAINS         */
      unsigned int   dcbdsggs : 1, /* GS - GRAPHICS ORGANIZATION                */
                      dcbdsgtx : 1, /* TX - TCAM LINE GROUP                      */
                      dcbdsgtq : 1, /* TQ - TCAM MESSAGE QUEUE                   */
                              : 1,
                      dcbacbm  : 1, /* ALWAYS 0 IN DCB. ALWAYS 1 IN ACB     @L5C */
                      dcbdsgtr : 1, /* TR - TCAM 3705                            */
                              : 2;
      struct {
        struct {
          struct {
            struct {
              unsigned char  dcbqslm; /* QSAM LOCATE MODE LOGICAL RECORD INTERFACE */
              } dcblnp;               /* 3525 PRINTER LINE POSITION COUNTER        */
            unsigned int   dcbodeba : 24; /* ADDRESS OF OLD DEB                        */
            } dcbodeb;                    /* ADDRESS OF OLD DEB                        */
          } dcbicqe;       /* ADDRESS OF ICQE                           */
        } dcbiobad;      /* ADDRESS OF IOB WHEN CHAINED SCHEDULING IS */
      };
    struct {
      unsigned char  _filler10[2];
      struct {
        unsigned char  _filler11;     /* RESERVED                         */
        unsigned int   dcbsvcxa : 24; /* POINTER TO EXIT LIST OF JES      */
        } dcbsvcxl;                   /* SAME AS DCBSVCXA BELOW           */
      };
    struct {
      unsigned char  _filler12[6];
      struct {
        struct {
          struct {
            unsigned char  dcbbfaln; /* BUFFER ALIGNMENT BITS                 */
            } dcbbftek;              /* BUFFERING TECHNIQUE BITS              */
          } dcbhiarc;      /* HIERARCHY BITS                        */
        unsigned int   dcbeoda : 24; /* ADDRESS OF A USER-PROVIDED ROUTINE TO */
        } dcbeodad;                  /* SAME AS DCBEODA BELOW                 */
      };
    struct {
      unsigned char  _filler13[10];
      struct {
        unsigned char  dcbrecfm;      /* RECORD FORMAT                          */
        unsigned int   dcbexlsa : 24; /* ADDRESS OF USER-PROVIDED LIST OF EXITS */
        } dcbexlst;                   /* ADDRESS OF USER-PROVIDED LIST OF EXITS */
      };
    };

  /*
    * MSF - this is an odd union. The structures are different sizes ranging
    * from the smalest at 8 bytes (dcbddnam for example) up to the largest
    * at 21 bytes
    */
  union {
    unsigned char  dcbddnam[8]; /* NAME ON THE DD STATEMENT WHICH DEFINES */
    struct {
      unsigned short dcbtiot; /* OFFSET FROM TIOT ORIGIN TO TIOELNGH FIELD */
      struct {
        unsigned char  dcbmacf1; /* FIRST BYTE OF DCBMACRF                    */
        unsigned char  dcbmacf2; /* SECOND BYTE OF DCBMACRF                   */
        } dcbmacrf;              /* SAME AS DCBMACR BEFORE OPEN               */
      struct {
        unsigned int   dcbifec  : 2, /* ERROR CORRECTION INDICATOR                */
                        dcbifpct : 2, /* PRINTER CARRIAGE TAPE PUNCH INDICATOR     */
                        dcbifioe : 2, /* IOS ERROR ROUTINE USE INDICATOR           */
                        dcbifldt : 1, /* POSSIBLE LOST DATA CONDITION     @42480LP */
                                : 1;
        unsigned int   dcbdeba : 24; /* ADDRESS OF ASSOCIATED DEB                 */
        } dcbdebad;                  /* ADDRESS OF ASSOCIATED DEB                 */
      };
    struct {
      unsigned char  _filler14[8];
      struct {
        struct {
          unsigned char  dcboflg;       /* SAME AS DCBOFLGS BEFORE OPEN     @ZA11086 */
          unsigned int   dcbwrita : 24; /* ADDRESS OF WRITE MODULE          @ZA11086 */
          } dcbwrite;                   /* ADDRESS OF WRITE MODULE          @ZA11086 */
        } dcbread;       /* ADDRESS OF READ MODULE                    */
      };
    struct {
      unsigned char  _filler15[8];
      unsigned char  dcboflgs;     /* FLAGS USED BY OPEN ROUTINE               */
      unsigned char  dcbiflg;      /* FLAGS USED BY IOS IN COMMUNICATING ERROR */
      struct {
        unsigned char  dcbmacr1; /* FIRST BYTE OF DCBMACR                    */
        unsigned char  dcbmacr2; /* SECOND BYTE OF DCBMACR                   */
        } dcbmacr;               /* MACRO INSTRUCTION REFERENCE              */
      };
    struct {
      unsigned char  _filler16[12];
      struct {
        struct {
          struct {
            unsigned char  dcboptcd;      /* OPTION CODES                             */
            unsigned int   dcbchcka : 24; /* ADDRESS OF CHECK MODULE                  */
            } dcbcheck;                   /* ADDRESS OF CHECK MODULE                  */
          } dcbperr;       /* ADDRESS OF SYNCHRONIZING ROUTINE FOR PUT */
        } dcbgerr;       /* ADDRESS OF SYNCHRONIZING ROUTINE FOR GET */
      };
    struct {
      unsigned char  _filler17[16];
      struct {
        char           dcbiobl;      /* IOB LENGTH IN DOUBLE WORDS             */
        unsigned int   dcbsyna : 24; /* ADDRESS OF USER-PROVIDED SYNAD ROUTINE */
        } dcbsynad;                  /* ADDRESS OF USER-PROVIDED SYNAD ROUTINE */
      };
    struct {
      unsigned char  _filler18[20];
      struct {
        unsigned char  dcbcind1; /* CONDITION INDICATORS                   */
        } dcbflag1;              /* TCAM APPLICATION PROGRAM FLAGS         */
      };
    };
  unsigned int   dcbcnsto : 1, /* PARTITIONED DATA SET - STOW HAS BEEN      */
                  dcbcnwr0 : 1, /* DIRECT ORGANIZATION DATA SET - LAST I/O   */
                  dcbcnclo : 1, /* CLOSE IN PROCESS (QSAM)                   */
                  dcbcnioe : 1, /* PERMANENT I/O ERROR (BSAM, BPAM, QSAM)    */
                  dcbcnbfp : 1, /* OPEN ACQUIRED BUFFER POOL                 */
                  dcbcnchs : 1, /* CHAINED SCHEDULING BEING SUPPORTED        */
                  dcbcnfeo : 1, /* FEOV BIT (BSAM, BPAM, QSAM)               */
                  dcbcnqsm : 1; /* ALWAYS ZERO (BSAM, BPAM)                  */
  short int      dcbblksi;     /* MAXIMUM BLOCK SIZE                        */
  unsigned char  dcbwcpo;      /* OFFSET OF WRITE CHANNEL PROGRAM FROM THE  */
  char           dcbwcpl;      /* LENGTH OF WRITE CHANNEL PROGRAM           */
  unsigned char  dcboffsr;     /* OFFSET OF READ CCW FROM BSAM/BPAM PREFIX  */
  unsigned char  dcboffsw;     /* OFFSET OF WRITE CCW FROM BSAM/BPAM PREFIX */
  union {
    void * PTR32 dcbcicb; /* SAME AS DCBCICBA BELOW                    */
    void * PTR32 dcbioba; /* FOR NORMAL SCHEDULING, ADDRESS OF QSAM OR */
    struct {
      unsigned char  _filler19;    /* @L4A                                      */
      unsigned int   dcbiobb : 24; /* SAME AS DCBIOBA ABOVE                @L4A */
      };
    struct {
      unsigned char  _filler20;     /* INTERNAL ACCESS METHOD USE                */
      unsigned int   dcbcicba : 24; /* POINTER TO JES C.I.                       */
      unsigned char  _filler21[8];
      struct {
        struct {
          unsigned char  dcbusasi; /* FLAG BYTE FOR ASCII TAPES                 */
          } dcbqsws;               /* FLAG BYTE                                 */
        struct {
          char           dcbdircq; /* NUMBER OF BYTES USED IN LAST DIRECTORY    */
          } dcbbufof;              /* BLOCK PREFIX LENGTH (0-99), SPECIFIED BY  */
        } dcbdirct;      /* NUMBER OF BYTES USED IN LAST DIRECTORY    */
      };
    struct {
      unsigned char  _filler22[4];
      struct {
        char           dcbncp;        /* NUMBER OF CHANNEL PROGRAMS.               */
        unsigned int   dcbeobra : 24; /* ADDRESS OF END-OF-BLOCK MODULE FOR READ   */
        } dcbeobr;                    /* ADDRESS OF END-OF-BLOCK MODULE FOR READ   */
      void * PTR32 dcbeobw;      /* ADDRESS OF END-OF-BLOCK MODULE FOR WRITE. */
      short int      _filler23;    /* DCBDIRCT - NUMBER OF BYTES USED IN LAST   */
      };
    };
  short int      dcblrecl;     /* LOGICAL RECORD LENGTH                     */
  union {
    void * PTR32 dcbcntrl; /* ADDRESS OF CNTRL MODULE      */
    void * PTR32 dcbnote;  /* ADDRESS OF NOTE/POINT MODULE */
    void * PTR32 dcbpoint; /* ADDRESS OF NOTE/POINT MODULE */
    };
};
#pragma pack(pop)

/* Values for field "_filler8" */
#define dcbdv311 0x21 /* 2311 DISK STORAGE                @ZA46311        */
#define dcbdv301 0x22 /* 2301 PARALLEL DRUM                               */
#define dcbdv303 0x23 /* 2303 SERIAL DRUM                                 */
#define dcbdv345 0x24 /* 9345 DISK STORAGE FACILITY(NO LONGER @11C        */
#define dcbdv321 0x25 /* 2321 DATA CELL STORAGE           @ZA46311        */
#define dcbd1305 0x26 /* 2305 DRUM MODEL-1                @ZA46311        */
#define dcbdv305 0x27 /* 2305 DRUM MODEL-2                @ZA46311        */
#define dcbdv314 0x28 /* 2314/2319 DISK STORAGE FACILITY  @ZA46311        */
#define dcbdv330 0x29 /* 3330 DISK STORAGE FACILITY       @ZA46311        */
#define dcbdv340 0x2A /* 3340/3344 DISK STORAGE FACILITY  @ZA46311        */
#define dcbdv350 0x2B /* 3350 DISK STORAGE FACILITY       @ZA46311        */
#define dcbdv375 0x2C /* 3375 DISK STORAGE FACILITY           @01A        */
#define dcbdv331 0x2D /* 3330 MODEL-11 OR 3333 MODEL-11   @ZA46311        */
#define dcbdv380 0x2E /* 3380 DISK STORAGE FACILITY           @01A        */
#define dcbdv390 0x2F /* 3390 DISK STORAGE FACILITY           @08A        */

/* Values for field "dcbqslm" */
#define dcb1dvds 0x80 /* ONLY ONE DEVICE IS ALLOCATED TO THIS             */
#define dcbupdcm 0x40 /* UPDATE COMPLETE, FREE OLD DEB                    */
#define dcbupdbt 0x30 /* UPDATE BITS                                      */
#define dcbupdt  0x20 /* UPDATE TO TAKE PLACE                             */
#define dcbnupd  0x30 /* NO UPDATE TO TAKE PLACE                          */
#define dcbsvdeb 0x10 /* OLD DEB ADDRESS MUST BE SAVED                    */

/* Values for field "dcbbfaln" */
#define dcbh1    0x80 /* HIERARCHY 1 MAIN STORAGE IF BIT5 IS ZERO.        */
#define dcbbft   0x70 /* BUFFERING TECHNIQUE                              */
#define dcbbfta  0x60 /* QSAM LOCATE MODE PROCESSING OF SPANNED           */
#define dcbbftr  0x20 /* FOR BSAM CREATE BDAM PROCESSING OF               */
#define dcbbfts  0x40 /* SIMPLE BUFFERING - BIT 3 IS ZERO                 */
#define dcbbftkr 0x20 /* UNBLOCKED SPANNED RECORDS - SOFTWARE             */
#define dcbbfte  0x10 /* EXCHANGE BUFFERING - BIT 1 IS ZERO               */
#define dcbbftkd 0x08 /* DYNAMIC BUFFERING (BTAM)                         */
#define dcbbftk  0x08 /* LRECL  IN 'K' UNITS FOR XLRI         @L2A        */
#define dcbbxlri 0x68 /* EXTENDED LOGICAL RECORD   @L2A                   */
#define dcbh0    0x04 /* HIERARCHY 0 MAIN STORAGE IF BIT0 IS ZERO         */
#define dcbbfa   0x03 /* BUFFER ALIGNMENT                                 */
#define dcbbfad  0x02 /* DOUBLEWORD BOUNDARY                              */
#define dcbbfaf1 0x01 /* FULLWORD NOT A DOUBLEWORD BOUNDARY,              */
#define dcbbfaf2 0x03 /* FULLWORD NOT A DOUBLEWORD BOUNDARY,              */

/* Values for field "dcbrecfm" */
#define dcbrecla 0xE0 /* RECORD LENGTH INDICATOR - ASCII                  */
#define dcbrecd  0x20 /* ASCII VARIABLE RECORD LENGTH                     */
#define dcbrecl  0xC0 /* RECORD LENGTH INDICATOR                          */
#define dcbrecf  0x80 /* FIXED RECORD LENGTH                              */
#define dcbrecv  0x40 /* VARIABLE RECORD LENGTH                           */
#define dcbrecu  0xC0 /* UNDEFINED RECORD LENGTH                          */
#define dcbrecto 0x20 /* TRACK OVERFLOW                                   */
#define dcbrecbr 0x10 /* BLOCKED RECORDS                                  */
#define dcbrecsb 0x08 /* FOR FIXED LENGTH RECORD FORMAT - STANDARD        */
#define dcbreccc 0x06 /* CONTROL CHARACTER INDICATOR                      */
#define dcbrecca 0x04 /* ASA CONTROL CHARACTER                            */
#define dcbreccm 0x02 /* MACHINE CONTROL CHARACTER                        */
#define dcbrecc  0x00 /* NO CONTROL CHARACTER                             */
#define dcbreckl 0x01 /* KEY LENGTH (KEYLEN) WAS SPECIFIED IN DCB         */

/* Values for field "dcboflgs" */
#define dcboflwr 0x80 /* IF ZERO, LAST I/O OPERATION WAS READ OR          */
#define dcbofiod 0x80 /* DATA SET IS BEING OPENED FOR INPUT OR            */
#define dcboflrb 0x40 /* LAST I/O OPERATION WAS IN READ BACKWARD          */
#define dcbofeov 0x20 /* SET TO 1 BY EOV WHEN IT CALLS CLOSE              */
#define dcbofopn 0x10 /* AN OPEN HAS BEEN SUCCESSFULLY COMPLETED          */
#define dcbofppc 0x08 /* SET TO 1 BY PROBLEM PROGRAM TO INDICATE A        */
#define dcboftm  0x04 /* TAPE MARK HAS BEEN READ                          */
#define dcbofuex 0x02 /* SET TO 0 BY AN I/O SUPPORT FUNCTION WHEN         */
#define dcbolock 0x02 /* SAME USE AS DCBOFUEX                 @LAA        */
#define dcbofiof 0x01 /* SET TO 1 BY AN I/O SUPPORT FUNCTION IF           */
#define dcbobusy 0x01 /* SAME USE AS DCBOFIOF                 @LAA        */

/* Values for field "dcbiflg" */
#define dcbibec  0xC0 /* ERROR CORRECTION INDICATOR                       */
#define dcbifnep 0x00 /* NOT IN ERROR PROCEDURE                           */
#define dcbex    0x40 /* ERROR CORRECTION OR IOS PAGE FIX IN              */
#define dcbifpec 0xC0 /* PERMANENT ERROR CORRECTION                       */
#define dcbibpct 0x30 /* PRINTER CARRIAGE TAPE PUNCH INDICATOR            */
#define dcbifc9  0x20 /* CHANNEL 9 PRINTER CARRIAGE TAPE PUNCH            */
#define dcbifc12 0x10 /* CHANNEL 12 PRINTER CARRIAGE TAPE PUNCH           */
#define dcbibioe 0x0C /* IOS ERROR ROUTINE USE INDICATOR                  */
#define dcbifer  0x00 /* ALWAYS USE I/O SUPERVISOR ERROR ROUTINE          */
#define dcbifne1 0x04 /* NEVER USE I/O SUPERVISOR ERROR ROUTINE           */
#define dcbiftim 0x04 /* TEST IOS MASK (IMSK) FOR ERROR PROCEDURE         */
#define dcbifne2 0x08 /* NEVER USE I/O SUPERVISOR ERROR ROUTINE           */
#define dcbifne3 0x0C /* NEVER USE I/O SUPERVISOR ERROR ROUTINE           */

/* Values for field "dcbmacr1" */
#define dcbmrecp 0x80 /* EXECUTE CHANNEL PROGRAM (EXCP) ---               */
#define dcbmrfe  0x40 /* FOUNDATION EXTENSION IS PRESENT (EXCP)           */
#define dcbmrget 0x40 /* GET (QSAM, QISAM, TCAM)                          */
#define dcbmrptq 0x40 /* PUT FOR MESSAGE GROUP (QTAM) ---                 */
#define dcbmrapg 0x20 /* APPENDAGES ARE REQUIRED (EXCP)                   */
#define dcbmrrd  0x20 /* READ (BSAM, BPAM, BISAM, BDAM, BTAM)             */
#define dcbmrwrq 0x20 /* WRITE FOR LINE GROUP (QTAM) ---                  */
#define dcbmrci  0x10 /* COMMON INTERFACE (EXCP)                          */
#define dcbmrmvg 0x10 /* MOVE MODE OF GET (QSAM, QISAM)                   */
#define dcbmrrdk 0x10 /* KEY SEGMENT WITH READ (BDAM) ---                 */
#define dcbmrlcg 0x08 /* LOCATE MODE OF GET (QSAM, QISAM)                 */
#define dcbmrrdi 0x08 /* ID ARGUMENT WITH READ (BDAM) ---                 */
#define dcbmrabc 0x04 /* USER'S PROGRAM MAINTAINS ACCURATE BLOCK          */
#define dcbmrpt1 0x04 /* POINT (WHICH IMPLIES NOTE) (BSAM, BPAM)          */
#define dcbmrsbg 0x04 /* SUBSTITUTE MODE OF GET (QSAM)                    */
#define dcbmrdbf 0x04 /* DYNAMIC BUFFERING (BISAM, BDAM) ---              */
#define dcbpgfxa 0x02 /* PAGE FIX APPENDAGE IS SPECIFIED (EXCP)           */
#define dcbmrcrl 0x02 /* CNTRL (BSAM, QSAM)                               */
#define dcbmrchk 0x02 /* CHECK (BISAM)                                    */
#define dcbmrrdx 0x02 /* READ EXCLUSIVE (BDAM) ---                        */
#define dcbmrdmg 0x01 /* DATA MODE OF GET (QSAM)                          */
#define dcbmrck  0x01 /* CHECK (BDAM) --- RESERVED (EXCP, BSAM,           */

/* Values for field "dcbmacr2" */
#define dcbmrstl 0x80 /* SETL (QISAM) --- ALWAYS ZERO (BSAM, QSAM,        */
#define dcbmrput 0x40 /* PUT (QSAM, TCAM) - PUT OR PUTX (QISAM)           */
#define dcbmrgtq 0x40 /* GET FOR MESSAGE GROUP (QTAM) ---                 */
#define dcbmrwrt 0x20 /* WRITE (BSAM, BPAM, BISAM, BDAM, BTAM)            */
#define dcbmrrdq 0x20 /* READ FOR LINE GROUP (QTAM) ---                   */
#define dcbmrmvp 0x10 /* MOVE MODE OF PUT (QSAM, QISAM)                   */
#define dcbmrwrk 0x10 /* KEY SEGMENT WITH WRITE (BDAM) ---                */
#define dcbmr5wd 0x08 /* FIVE-WORD DEVICE INTERFACE (EXCP)                */
#define dcbmrldm 0x08 /* LOAD MODE BSAM (CREATE BDAM DATA SET)            */
#define dcbmrlcp 0x08 /* LOCATE MODE OF PUT (QSAM, QISAM)                 */
#define dcbmridw 0x08 /* ID ARGUMENT WITH WRITE (BDAM) ---                */
#define dcbmr4wd 0x04 /* FOUR-WORD DEVICE INTERFACE (EXCP)                */
#define dcbmrpt2 0x04 /* POINT (WHICH IMPLIES NOTE) (BSAM, BPAM)          */
#define dcbmrtmd 0x04 /* SUBSTITUTE MODE (QSAM)                           */
#define dcbmruip 0x04 /* UPDATE IN PLACE (PUTX) (QISAM) ---               */
#define dcbmr3wd 0x02 /* THREE-WORD DEVICE INTERFACE (EXCP)               */
#define dcbmrctl 0x02 /* CNTRL (BSAM, QSAM)                               */
#define dcbmrstk 0x02 /* SETL BY KEY (QISAM)                              */
#define dcbmrawr 0x02 /* ADD TYPE OF WRITE (BDAM) ---                     */
#define dcbmr1wd 0x01 /* ONE-WORD DEVICE INTERFACE (EXCP)                 */
#define dcbmrswa 0x01 /* USER'S PROGRAM HAS PROVIDED A SEGMENT            */
#define dcbmrdmd 0x01 /* DATA MODE (QSAM)                                 */
#define dcbmrsti 0x01 /* SETL BY ID (QISAM) ---                           */
#define dcblngxe 0x34 /* LENGTH OF DCB FOR EXCP WITH          @L1A        */

/* Values for field "dcboptcd" */
#define dcboptw  0x80 /* WRITE VALIDITY CHECK (DASD)                      */
#define dcboptu  0x40 /* ALLOW DATA CHECK CAUSED BY INVALID               */
#define dcboptc  0x20 /* CHAINED SCHEDULING                               */
#define dcbopth  0x10 /* 1287/1288 OPTICAL READER - HOPPER EMPTY          */
#define dcbopto  0x10 /* 1285/1287 OPTICAL READER - ON-LINE               */
#define dcbbckpt 0x10 /* CHANNEL-END APPENDAGE IS TO BYPASS DOS           */
#define dcboptq  0x08 /* TRANSLATION TO OR FROM ASCII                     */
#define dcboptz  0x04 /* MAGNETIC TAPE DEVICES - USE REDUCED ERROR        */
#define dcbsrchd 0x04 /* USE SEARCH DIRECT, INSTEAD OF SEARCH             */
#define dcboptt  0x02 /* USER TOTALING (BSAM, QSAM)                       */
#define dcboptj  0x01 /* 3800 PRINTER, OPTCD=J; (DYNAMIC  @Z40MSRZ        */

/* Values for field "dcbcind1" */
#define dcbcntov 0x80 /* DIRECT ACCESS - TRACK OVERFLOW IN USE            */
#define dcbstqck 0x80 /* STOP EQUAL QUICK WAS SPECIFIED FOR               */
#define dcbstfls 0x40 /* STOP EQUAL FLUSH WAS SPECIFIED FOR               */
#define dcbcnsrd 0x40 /* SEARCH DIRECT (BSAM, BPAM, QSAM)                 */
#define dcbcnevb 0x20 /* END OF VOLUME - USED BY EOB ROUTINES             */
#define dcbcneva 0x10 /* END OF VOLUME - USED BY CHANNEL-END              */
#define dcbcnci  0x08 /* SAM-SI COMPATABILITY INTERFACE (CI)              */
#define dcbcnbrm 0x04 /* BLOCKED RECORD BIT MODIFIED (BSAM,BPAM,          */
#define dcbcbndf 0x02 /* -      OPEN DEFAULTED BUFNO (QSAM)          @L7A */
#define dcbcnexb 0x01 /* EXCHANGE BUFFERING SUPPORTED (QSAM)              */
#define dcbcdwdo 0x01 /* INTERNAL USE ONLY, DCBOFFSW IS       @LBA        */

/* Values for field "dcbusasi" */
#define dcbblbp  0x40 /* BLOCK PREFIX IS FOUR BYTE FIELD                  */
#define dcbqadfs 0x38 /* USED TO PERFORM SEQUENCE                         */
#define dcbqadf1 0x20 /* FIRST BIT OF DCBQADFS                            */
#define dcbqadf2 0x10 /* SECOND BIT OF DCBQADFS                           */
#define dcbqadf3 0x08 /* THIRD BIT OF DCBQADFS                            */
#define dcb3525a 0x02 /* DCB IS 3525 - ASSOCIATED DATA                    */
#define dcbqstru 0x01 /* TRUNC ENTRY POINT ENTERED (QSAM)                 */

/* Values for field "dcbpoint" */
#define dcblngbs 0x58 /* LENGTH OF DCB FOR BSAM INTERFACE     @L1A        */
#define dcblngpo 0x58 /* LENGTH OF DCB FOR BPAM INTERFACE     @L1A        */

struct ihadcb* PTR32 dcb_init(const char* ddname);
void dcb_free(struct ihadcb* PTR32 dcb);
void dcb_fmt_dmp(const DBG_Opts* opts, struct ihadcb* PTR32 dcb);

#define DCB_ADDR_UNSET   ((void*)(1))
#define DCB_ADDR24_UNSET (1)
#endif
