#ifndef __IOB__
#define __IOB__ 1

#include "metaldio.h"

#pragma pack(1)
struct iob {
  union {
    struct {
      unsigned int   iobv6chn : 1, /* -   I/O CHAINED BIT SET BY IGG019V6  @ZA56251      */
                     iobrsv02 : 1, /* RESERVED                                           */
                     iobrsv03 : 1, /* RESERVED                                           */
                     iobrsv04 : 1, /* RESERVED                                           */
                     iobptst  : 1, /* -   NOTE OR POINT OPERATION IS IN PROCESS          */
                     iobabapp : 1, /* -   ERROR HAS BEEN PROCESSED ONCE BY ABNORMAL-END  */
                     iobrstch : 1, /* -   RESTART CHANNEL                                */
                     iobpci   : 1; /* -   SET WHEN A PROGRAM-CONTROLLED INTERRUPTION     */
      unsigned char  iobrsv05;     /* -     RESERVED                                     */
      unsigned char  iobcinop;     /* -     OFFSET OF THE LAST I/O COMMAND FOR INPUT     */
      unsigned char  iobconop;     /* -     OFFSET OF THE LAST I/O COMMAND FOR AN OUTPUT */
      int            iobcecb;      /* -       EVENT CONTROL BLOCK USED BY BSAM OR QSAM.  */
      };
    struct {
      int            iobvidan; /* VIRTUAL IDAW FOR QSAM, BSAM, BPAM NORMAL  @L3A */
      struct {
        int            ioblenrd; /* LENGTH OF BLOCK READ, LBI                 @L5A */
        } iobvida2;              /* VIRTUAL IDAW WHEN NOT BEGINNING OF BUFFER,@L5C */
      };
    struct {
      struct {
        unsigned char  iobflag1;    /* -       FLAG BYTE 1                                  */
        unsigned char  iobflag2;    /* -       FLAG BYTE 2                                  */
        unsigned char  iobsens0;    /* -       FIRST SENSE BYTE                             */
        unsigned int   iobs1b0 : 1, /* -   BIT 0 (DEVICE DEPENDENT)                         */
                       iobs1b1 : 1, /* -   BIT 1 (DEVICE DEPENDENT)                         */
                       iobs1b2 : 1, /* -   BIT 2 (DEVICE DEPENDENT)                         */
                       iobs1b3 : 1, /* -   BIT 3 (DEVICE DEPENDENT)                         */
                       iobs1b4 : 1, /* -   BIT 4 (DEVICE DEPENDENT)                         */
                       iobs1b5 : 1, /* -   BIT 5 (DEVICE DEPENDENT)                         */
                       iobs1b6 : 1, /* -   BIT 6 (DEVICE DEPENDENT)                         */
                       iobs1b7 : 1; /* -   BIT 7 (DEVICE DEPENDENT)                         */
        struct {
          unsigned char  iobecbcc;      /* -     COMPLETION CODE FOR AN I/O REQUEST.  THIS CODE */
          unsigned int   iobecbpb : 24; /* -     ADDRESS OF THE ECB TO BE POSTED UPON THE       */
          } iobecbpt;                   /* -      ADDRESS OF ECB TO BE POSTED ON I/O COMPLETION */
        } iobstdrd;
      };
    struct {
      unsigned char  _filler5[8];
      struct {
        struct {
          struct {
            unsigned char  iobfl3; /* -       FLAG 3 - STATUS ERROR COUNTS FOR MAGNETIC */
            } iobflag3;            /* -      I/O SUPERVISOR ERROR ROUTINE FLAG BYTE     */
          unsigned char  _filler6[3];
          } iobcmd31;                 /* ENDING CCW ADDRESS IF IOBEFMT1 IS ON      @P1A */
        unsigned char  _filler7[4];
        } iobcsw8;                  /* 8 BYTE CSW IF IOBEFMT1 IS ON              @P1A */
      };
    struct {
      unsigned char  _filler8[9];
      struct {
        struct {
          unsigned int   iobcmda : 24; /* -     COMMAND ADDRESS (3890)                  MDC023 */
          struct {
            unsigned int   iobusb0 : 1, /* -   ATTENTION  (MDC312)                   @Z40MP9A   */
                           iobusb1 : 1, /* -   STATUS MODIFIER  (MDC313)             @Z40MP9A   */
                           iobusb2 : 1, /* -   CONTROL UNIT END  (MDC314)            @Z40MP9A   */
                           iobusb3 : 1, /* -   BUSY  (MDC315)                        @Z40MP9A   */
                           iobusb4 : 1, /* -   CHANNEL END  (MDC316)                 @Z40MP9A   */
                           iobusb5 : 1, /* -   DEVICE END  (MDC317)                  @Z40MP9A   */
                           iobusb6 : 1, /* -   UNIT CHECK  (MDC318)                  @Z40MP9A   */
                           iobusb7 : 1; /* -   UNIT EXCEPTION  (MDC319)              @Z40MP9A   */
            unsigned int   iobcsb0 : 1, /* -   PROGRAM CONTROL INTERRUPT  (MDC321)   @Z40MP9A   */
                           iobcsb1 : 1, /* -   INCORRECT LENGTH  (MDC322)            @Z40MP9A   */
                           iobcsb2 : 1, /* -   PROGRAM CHECK  (MDC323)               @Z40MP9A   */
                           iobcsb3 : 1, /* -   PROTECTION CHECK  (MDC324)            @Z40MP9A   */
                           iobcsb4 : 1, /* -   CHANNEL DATA CHECK  (MDC325)          @Z40MP9A   */
                           iobcsb5 : 1, /* -   CHANNEL CONTROL CHECK  (MDC326)       @Z40MP9A   */
                           iobcsb6 : 1, /* -   INTERFACE CONTROL CHECK  (MDC327)     @Z40MP9A   */
                           iobcsb7 : 1; /* -   CHAINING CHECK  (MDC328)              @Z40MP9A   */
            } iobstbyt;                 /* -    STATUS BITS 32-47 (3890)                MDC024  */
          } iobiocsw;                  /* -    LOW-ORDER BYTES OF CSW FOR MAGNETIC DOCUMENT    */
        unsigned short iobresct; /* -     RESIDUAL COUNT                            @L6C */
        } iobcsw;                   /* -    LOW-ORDER SEVEN BYTES OF THE LAST CSW THAT      */
      };
    struct {
      unsigned char  _filler9[16];
      struct {
        unsigned char  iobsiocc;      /* -       SIO CODE.  BITS 2 AND 3 CONTAIN CONDITION CODE */
        unsigned int   iobstrtb : 24; /* -     ADDRESS OF CHANNEL PROGRAM TO BE EXECUTED        */
        } iobstart;                   /* -      ADDRESS OF CHANNEL PROGRAM TO BE EXECUTED IF    */
      };
    struct {
      unsigned char  _filler10[20];
      struct {
        unsigned int   iobgdpol  : 1, /* -   RE-ENTER SIO APPENDAGE FOR OLTEP GUARANTEED    */
                       iobcc3we  : 1, /* -   USER REQUESTS THAT IOS POST A X'6D' FOR A      */
                       iobpmerr  : 1, /* -   VTAM SETS THIS BIT ON TO INDICATE TO IOS THAT  */
                       iobcef    : 1, /* -   IOB COMMON EXTENSION IS AVAILABLE         @02C */
                       iobrsv41  : 1, /* - RESERVED                                         */
                       iobrsv42  : 1, /* - RESERVED                                         */
                       iobjes3i  : 1, /* -   JES3 INTERVENTION REQUIRED NOTIFICATION.       */
                       iobrsv44  : 1; /* - RESERVED                                         */
        unsigned int   iobdcbpb : 24; /* -     ADDRESS OF DCB ASSOCIATED WITH THIS IOB      */
        } iobdcbpt;                   /* -      ADDRESS OF DCB ASSOCIATED WITH THIS IOB     */
      };
    struct {
      unsigned char  _filler11[24];
      struct {
        unsigned char  iobrepos;      /* -     DURING I/O ERROR CORRECTION (MEANINGFUL ONLY */
        unsigned int   iobrstrb : 24; /* -     SAME AS IOBRESTR ABOVE                       */
        } iobrestr;                   /* -      AFTER SVC 16 (PURGE) - QUIESCE - ADDRESS OF */
      };
    struct {
      unsigned char  _filler12[28];
      struct {
        unsigned int   iobprmer : 1, /* -   SAD OR ENABLE ISSUED BY OPEN RESULTED IN A  */
                       iobinuse : 1, /* -   THIS IOB IS CURRENTLY IN USE BY AN I/O      */
                       iobrsv14 : 1, /* RESERVED                                        */
                       iobrsv15 : 1, /* RESERVED                                        */
                       iobrsv16 : 1, /* RESERVED                                        */
                       iobrsv17 : 1, /* RESERVED                                        */
                       iobrftmg : 1, /* -   A REQUEST-FOR-TEST MESSAGE RECEIVED FROM A  */
                       ioboltst : 1; /* -   LINE IS UNDER ON-LINE TEST OPERATION        */
        unsigned char  iobrsv19;     /* -     RESERVED                                  */
        } iobincam;                  /* -      QSAM, BSAM, EXCP ACCESS METHOD -- NORMAL */
      };
    struct {
      unsigned char  _filler13[28];
      unsigned int   iobovr    : 1, /* -   OVERRUN ERROR (3890)  (MDC026)        @G30HP9A */
                     iobrej    : 1, /* -   COMMAND REJECT ERROR (3890) (MDC027)  @G30HP9A */
                     iobdck    : 1, /* -   DATA CHECK ERROR (3890)  (MDC028)     @G30HP9A */
                     iobbus    : 1, /* -   BUS-OUT ERROR (3890)  (MDC029)        @G30HP9A */
                     iobeqp    : 1, /* -   EQUIPMENT CHECK ERROR (3890) (MDC030) @G30HP9A */
                     iobent    : 1, /* -   FIRST TIME ENTRY SWITCH (3890)                 */
                     iobrsv47  : 1, /* - RESERVED FOR 3890                   MDC044       */
                     iobrsv46  : 1; /* - RESERVED FOR 3890                   MDC033       */
      unsigned char  _filler14;
      };
    struct {
      unsigned char  _filler15[28];
      unsigned char  iobcrdcc;      /* -     DATA CHECK ERROR COUNT  (OPTICAL READER)       */
      unsigned char  iobcrilc;      /* -     INCORRECT LENGTH ERROR COUNT  (OPTICAL READER) */
      };
    struct {
      unsigned char  _filler16[28];
      unsigned char  iobamaf;       /* SMS  DIAGNOSTICS BYTE TO DESCRIBE         @L1A         */
      unsigned char  _filler17;
      short int      ioberrct;      /* -       USED BY I/O SUPERVISOR ERROR ROUTINES TO COUNT */
      };
    struct {
      unsigned char  _filler18[32];
      struct {
        struct {
          unsigned char  iobm; /* -     THE NUMBER OF THE DEB EXTENT TO BE USED FOR */
          struct {
            unsigned char  iobbb1;
            unsigned char  iobbb2;
            } iobbb;               /* -    BIN NUMBER(DATA CELL)                        */
          struct {
            unsigned char  iobcc1;
            unsigned char  iobcc2;
            } iobcc;               /* -    CYLINDER NUMBER                              */
          struct {
            unsigned char  iobhh1;
            unsigned char  iobhh2;
            } iobhh;               /* -    TRACK NUMBER                                 */
          unsigned char  iobr; /* -     RECORD NUMBER                               */
          } iobseek;           /* -    A SEEK ADDRESS (IN THE FORMAT MBBCCHHR) USED */
        } iobexten;
      };
    struct {
      unsigned char  _filler19[32];
      struct {
        struct {
          unsigned char  iobrtype; /* -       RECORD TYPE FOR OBR                     MDC002 */
          } iobucbxv;              /* -    UCB INDEX                               MDC050    */
        unsigned int   iobercta : 24; /* -     POINTER TO COUNTERS FOR SIO AND TEMPORARY       */
        } ioberct;                    /* -      POINTER TO COUNTERS FOR SIO AND TEMPORARY      */
      struct {
        char           iobnamsz;      /* -     SIZE OF TERMINAL NAME                   MDC005  */
        unsigned int   iobnamea : 24; /* -     POINTER TO TERMINAL NAME                MDC006  */
        } iobname;                    /* -      POINTER TO TERMINAL NAME                MDC004 */
      };
    struct {
      unsigned char  _filler20[32];
      struct {
        unsigned char  iobskrv; /* -     RESERVED  (MDC301)                    @Z30OP9A */
        unsigned char  iobsktt; /* -     TRACK NUMBER  (MDC302)                @Z30OP9A */
        unsigned char  iobsk0;  /* -     MUST BE ZERO  (MDC303)                @Z30OP9A */
        unsigned char  iobskss; /* -     SECTOR NUMBER  (MDC304)               @Z30OP9A */
        } iobskadr;             /* -    3540 SEEK ADDRESS  (MDC300)           @Z30OP9A  */
      unsigned char  _filler21[4];
      };
    struct {
      unsigned char  _filler22[32];
      unsigned char  iobucbx;       /* -     UCB INDEX.  THE LINE NUMBER IS USED AS AN    */
      unsigned char  iobwork[5];    /* -     WORK AREA USED BY ERROR ROUTINES AND ON-LINE */
      unsigned char  iobrcvpt;      /* -     RECEIVED ACK (ACK-0 OR ACK-1)                */
      unsigned char  iobsndpt;      /* -     SENT ACK (ACK-0 OR ACK-1)                    */
      };
    struct {
      unsigned char  _filler23[32];
      unsigned char  iobucbxg;      /* -     UCB INDEX                                    */
      unsigned char  iobrsv37[3];   /* -     RESERVED                                     */
      struct {
        unsigned int   iobavlfl  : 1, /* -   IF 0, IOB IS AVAILABLE.                        */
                       iobrsv20  : 1, /* RESERVED                                           */
                       iobrsv21  : 1, /* RESERVED                                           */
                       iobrsv22  : 1, /* RESERVED                                           */
                       iobrsv23  : 1, /* RESERVED                                           */
                       iobrsv24  : 1, /* RESERVED                                           */
                       iobrsv25  : 1, /* RESERVED                                           */
                       iobrsv26  : 1; /* RESERVED                                           */
        unsigned int   iobnxtpb : 24; /* -     ADDRESS OF NEXT AVAILABLE IOB.  SET TO ZERO  */
        } iobnxtpt;                   /* -      ADDRESS OF NEXT AVAILABLE IOB.  SET TO ZERO */
      };
    struct {
      unsigned char  _filler24[42];
      struct {
        unsigned char  w1oexten[2]; /* -     SAME AS W1IEXTEN ABOVE                      */
        } w1iexten;                 /* -    APPENDAGE CODES FOR BOTH NORMAL AND ABNORMAL */
      };
    struct {
      unsigned char  _filler25[42];
      struct {
        unsigned char  iobsk2m;     /* -       EXTENT NUMBER                           ICB435 */
        unsigned char  iobsk2bb[2]; /* -     BIN NUMBER                              ICB435   */
        unsigned char  iobsk2cc[2]; /* -     CYLINDER NUMBER                         ICB435   */
        unsigned char  iobsk2hh[2]; /* -     HEAD NUMBER                             ICB435   */
        unsigned char  iobsk2r;     /* -       RECORD NUMBER                           ICB435 */
        } iobseek2;                 /* -    SEEK FIELD 2                            ICB435    */
      };
    struct {
      unsigned char  _filler26[42];
      unsigned char  ioberccw[8];   /* -     CCW AREA USED BY THE BTAM ERROR RECOVERY */
      };
    struct {
      unsigned char  _filler27[42];
      void * PTR32 iobccwad;      /* -       FOR FIXED LENGTH RECORDS, ADDRESS OF FIRST */
      unsigned int   iobdeqcp  : 1, /* -   DEQUEUE CHANNEL PROGRAM FROM QUEUE             */
                     iobunsch  : 1, /* -   UNSCHEDULED QUEUE                              */
                     iobovptr  : 1, /* -   IF 0, DECBAREA + 6 POINTS TO OVERFLOW RECORD   */
                     iobkeyad  : 1, /* -   IF 0, DECBKEY POINTS TO OVERFLOW RECORD KEY.   */
                     iobrsv27  : 1, /* RESERVED                                           */
                     iobrsv28  : 1, /* RESERVED                                           */
                     iobrsv29  : 1, /* RESERVED                                           */
                     iobchnnl  : 1; /* -   IF 0, NORMAL CHANNEL END HAS OCCURRED.         */
      unsigned int   iobcpbsy  : 1, /* -   CHANNEL PROGRAM CP1 OR CP2 BUSY                */
                     iobntav1  : 1, /* -   NO CP4, CP5 OR CP6 AVAILABLE                   */
                     iobntav2  : 1, /* -   NO CP7 AVAILABLE                               */
                     iobknwr   : 1, /* -   WRITE KN IS IN EFFECT (UNSCHEDULED IOB IS FOR  */
                     iobknrwr  : 1, /* -   WRITE KN IS IN EFFECT (UNSCHEDULED IOB IS FOR  */
                     iobrsv30  : 1, /* RESERVED                                           */
                     iobrsv31  : 1, /* RESERVED                                           */
                     iobrsv32  : 1; /* RESERVED                                           */
      unsigned char  iobapp;        /* -     APPENDAGE CODE                               */
      unsigned char  iobasyn;       /* -     ASYNCHRONOUS ROUTINE CODE                    */
      struct {
        unsigned char  iobcount;      /* -     WRITE CHECK COUNTER                          */
        unsigned int   iobfchnb : 24; /* -     FORWARD CHAIN ADDRESS                        */
        } iobfchad;                   /* -      FORWARD CHAIN ADDRESS                       */
      };
    struct {
      unsigned char  _filler28[42];
      unsigned char  iobccw[32];    /* -    LIST OF CHANNEL COMMAND WORDS TO TRANSFER DATA */
      };
    struct {
      unsigned char  _filler29[42];
      short int      iobdbytr;      /* -       NUMBER OF UNUSED BYTES REMAINING ON THE TRACK */
      unsigned char  iobdcap0;      /* LAST BYTE OF CAPACITY RECORD - MUST REMAIN            */
      unsigned char  iobdiobs;      /* OVERALL SIZE OF THE IOB IN WORDS         @04C         */
      struct {
        unsigned char  iobdayli;     /* -       ALL BITS SET TO ZERO INDICATE THE AVAILABILITY */
        unsigned int   iobdplb : 24; /* -     ADDRESS OF THE NEXT IOB IN THE POOL OF IOB'S     */
        } iobdplad;                  /* -      ADDRESS OF THE NEXT IOB IN THE POOL OF IOB'S    */
      unsigned int   iobverfy  : 1, /* -   VERIFY                                            */
                     iobovflo  : 1, /* -   OVERFLOW                                          */
                     iobextsc  : 1, /* -   EXTENDED SEARCH                                   */
                     iobfdbck  : 1, /* -   FEEDBACK                                          */
                     iobactad  : 1, /* -   ACTUAL ADDRESSING                                 */
                     iobdynbf  : 1, /* -   DYNAMIC BUFFERING                                 */
                     iobrdexc  : 1, /* -   READ EXCLUSIVE                                    */
                     iobrelbl  : 1; /* -   RELATIVE BLOCK ADDRESSING                         */
      unsigned int   iobskey   : 1, /* -   KEY ADDRESS CODED AS 'S'                          */
                     iobsblkl  : 1, /* -   BLOCK LENGTH CODED AS 'S'                         */
                     iobsuffx  : 2, /* -   IF BITS 2 AND 3 ARE ONE, RU IS SUFFIXED TO THE    */
                     iobrqust  : 1, /* -   IF 1, READ REQUEST.  IF 0, WRITE REQUEST.         */
                     iobtype   : 1, /* -   IF 1, KEY TYPE.  IF 0, ID TYPE.                   */
                     iobaddty  : 1, /* -   ADD TYPE                                          */
                     iobrelex  : 1; /* -   RELEX MACRO ISSUED                                */
      struct {
        unsigned int   iobabnrm : 1, /* -   ABNORMAL COMPLETION                               */
                       iobnewvl : 1, /* -   ON EXTENDED SEARCH, THE NEXT EXTENT IS ON A       */
                       iobsynch : 1, /* -   MODULE WAS ENTERED VIA SYNCH            MDC037    */
                       iobpass2 : 1, /* -   ON EXTENDED SEARCH, INDICATES TO THE RELATIVE     */
                       iobenque : 1, /* -   FOR EXCLUSIVE CONTROL REQUEST, INDICATES THAT     */
                       iobbuff  : 1, /* -   A BUFFER HAS BEEN ASSIGNED TO THIS IOB            */
                       iobaddvu : 1, /* -   IOB BEING USED TO ADD A VARIABLE (V) OR           */
                       iobsiort : 1; /* -   INDICATES TO THE DYNAMIC BUFFERING ROUTINE        */
        unsigned char  iobstat2;     /* -     ERROR CODE FOR ABNORMAL COMPLETION USED AS      */
        } iobdstat;                  /* -   STATUS OF THE I/O REQUEST                         */
      void * PTR32 iobdcpnd;      /* -       ADDRESS OF LOCATION WHERE CHANNEL END PROGRAM */
      short int      iobdbytn;      /* -       NUMBER OF BYTES NEEDED ON A TRACK TO WRITE A  */
      unsigned int   iobrec31  : 1, /* -   BLOCK REFERENCE ADDR (DECRECPT) IS A      @03A    */
                     iobkey31  : 1, /* -   KEY ADDR (DECKYADR) IS A 31 BIT ADDR      @03A    */
                     iobdat31  : 1, /* -   DATA ADDR (DECAREA) IS A 31 BIT ADDR      @03A    */
                               : 5;
      unsigned char  iobrsv34;      /* -     RESERVED                                  @03C  */
      void * PTR32 iobdqptr;      /* -       ADDRESS OF IOB FOR NEXT I/O OPERATION TO BE   */
      unsigned char  iobrsv35[8];   /* -     RESERVED                                        */
      };
    struct {
      unsigned char  _filler30[42];
      void * PTR32 iobmdrec;      /* -       POINTER TO RECORD BEING PASSED TO              */
      void * PTR32 iobrcd;        /* -       POINTER TO QUEUE OF OBR RECORDS PASSED FROM    */
      unsigned char  iobsensv;      /* -       SENSE BYTE SAVE AREA                    MDC009 */
      unsigned char  iobcswsv[7];   /* -     SAVE AREA FOR LAST 7 BYTES OF CSW       MDC010   */
      unsigned char  _filler31[16];
      };
    struct {
      unsigned char  _filler32[48];
      unsigned char  ioberinf[16];  /* -    ERROR INFORMATION FIELD USED BY THE BTAM ERROR  */
      struct {
        unsigned char  _filler33[8];
        } iobcpa;                    /* -      CHANNEL PROGRAMS AREA.  THE LENGTH DEPENDS ON */
      };
    struct {
      unsigned char  _filler34[48];
      void * PTR32 iobbufc;       /* -       ADDRESS OF ASSOCIATED BUFFER CONTROL BLOCK     */
      void * PTR32 iobreada;      /* -       ADDRESS OF FIRST READ CHANNEL PROGRAM SEGMENT  */
      void * PTR32 iobnexta;      /* -       ADDRESS OF NEXT ACTIVE IOB              ICB435 */
      void * PTR32 iobrdchp;      /* -       ADDRESS OF READ CHANNEL PROGRAM         ICB435 */
      unsigned char  _filler35[8];
      };
    struct {
      unsigned char  _filler36[52];
      void * PTR32 iobbchad;      /* -       BACKWARD CHAIN ADDRESS  */
      unsigned char  _filler37[16];
      unsigned char  iobdncrf[8];   /* -     COUNT FIELD FOR NEW BLOCK */
      };
    struct {
      unsigned char  _filler38[72];
      __extension__ double         iobchnpr[0]; /* -      CHANNEL PROGRAM USED TO TRANSFER DATA AS */
      };
    };
  };

/* Values for field "iobnflg1" */
#define iobprtov 0x80 /* -   PRTOV HAS OCCURRED (PRINTER DEVICES)           */
#define iobsegmt 0x80 /* -   SEGMENTING OF A SPANNED RECORD IS IN PROCESS   */
#define iobwrite 0x40 /* -   A WRITE OPERATION IS IN PROCESS                */
#define iobread  0x20 /* -   A READ OPERATION IS IN PROCESS                 */
#define iobupdat 0x10 /* -   UPDATE FLAG.  SET ON TOGETHER WITH BIT 1 OF    */
#define iobbkspc 0x08 /* -   IOB BEING USED FOR BACKSPACE, CONTROL OR       */
#define iobspan  0x04 /* -   THE RECORD CURRENTLY BEING PROCESSED HAS MORE  */
#define iobuperr 0x02 /* -   UPDATE CHANNEL PROGRAM HAS BEEN SPLIT INTO     */
#define iobfirst 0x01 /* -   THIS IS THE FIRST IOB ON CHAIN                 */

/* Values for field "iobflag1" */
#define iobdatch 0x80 /* -   DATA CHAINING USED IN CHANNEL PROGRAM          */
#define iobcmdch 0x40 /* -   COMMAND CHAINING USED IN CHANNEL PROGRAM       */
#define ioberrtn 0x20 /* -   ERROR ROUTINE IS IN CONTROL                    */
#define iobrpstn 0x10 /* -   DEVICE IS TO BE REPOSITIONED                   */
#define iobcycck 0x08 /* -   CYCLIC REDUNDANCY CHECK (CRC) NEEDED (TAPE)    */
#define iobfcrex 0x08 /* -   FETCH COMMAND RETRY EXIT (DIRECT ACCESS)       */
#define iobioerr 0x04 /* -   EXCEPTIONAL CONDITION.  AFTER THE ERROR        */
#define iobunrel 0x02 /* -   IOB UNRELATED FLAG (I.E., NONSEQUENTIAL)       */
#define iobrstrt 0x01 /* -   IF 1, RESTART ADDRESS IN IOB TO BE USED.       */
#define iobspsvc 0x01 /* -   FOR SAM/PAM, SET BY SVC IF I/O APPENDAGE       */

/* Values for field "iobflag2" */
#define iobhalt  0x80 /* -   HALT I/O HAS BEEN ISSUED BY SVC PURGE ROUTINE  */
#define iobsense 0x40 /* -   SENSE WILL NOT BE PERFORMED UNTIL THE DEVICE   */
#define iobpurge 0x20 /* -   IOB HAS BEEN PURGED TO ALLOW I/O ACTIVITY TO   */
#define iobrrt3  0x20 /* -   TYPE 3 RELATED REQUEST (OS/VS2)         MDC048 */
#define iobrdha0 0x10 /* -   HOME ADDRESS (R0) RECORD IS TO BE READ.  SEEK  */
#define iobrrt2  0x10 /* -   TYPE 2 RELATED REQUEST (OS/VS2)         MDC049 */
#define iobalttr 0x08 /* -   NO TEST FOR OUT-OF-EXTENT.  AN ALTERNATE TRACK */
#define iobskupd 0x04 /* -   SEEK ADDRESS IS BEING UPDATED.  CYLINDER END   */
#define iobstato 0x02 /* -   DEVICE END STATUS HAS BEEN OR'ED WITH CHANNEL  */
#define iobpnch  0x01 /* -   ERROR RECOVERY IN CONTROL FOR A 2540 CARD      */

/* Values for field "iobsens0" */
#define iobs0b0  0x80 /* -   BIT 0 (DEVICE DEPENDENT)                       */
#define iobs0b1  0x40 /* -   BIT 1 (DEVICE DEPENDENT)                       */
#define iobs0b2  0x20 /* -   BIT 2 (DEVICE DEPENDENT)                       */
#define iobs0b3  0x10 /* -   BIT 3 (DEVICE DEPENDENT)                       */
#define iobs0b4  0x08 /* -   BIT 4 (DEVICE DEPENDENT)                       */
#define iobs0b5  0x04 /* -   BIT 5 (DEVICE DEPENDENT)                       */
#define iobs0b6  0x02 /* -   BIT 6 (DEVICE DEPENDENT)                       */
#define iobs0b7  0x01 /* -   BIT 7 (DEVICE DEPENDENT)                       */
#define iobsnsc9 0x01 /* -   CHANNEL 9 SENSED IN CARRIAGE TAPE              */

/* Values for field "iobfl3" */
#define iobccc   0x80 /* -   CHANNEL CONTROL CHECK ERROR COUNT (3890)       */
#define iobicc   0x40 /* -   INTERFACE CONTROL CHECK ERROR COUNT (3890)     */
#define iobcdc   0x20 /* -   CHANNEL DATA CHECK ERROR (3890)         MDC040 */
#define iobacu   0x10 /* -   ATTENTION/CONTROL UNIT ERROR (3890)     MDC041 */
#define iobcnc   0x08 /* -   CHAIN CHECK ERROR (3890)                MDC042 */
#define iobsdr   0x08 /* -   STATISTICS ONLY FLAG (3800) (MDC306)  @X50AD9A */
#define iobmsg   0x04 /* -   MESSAGE FLAG (3890 OR 3800) (MDC308)  @X50AD9A */
#define iobicl   0x02 /* -   INCORRECT LENGTH ERROR (3890)           MDC020 */
#define iobjam   0x02 /* -   SET ON WHEN JES SUBSYSTEM HAS DETECTED A       */
#define ioblog   0x01 /* -   LOG OUT FLAG (3890 OR 3800) (MDC309)  @X50AD9A */

/* Values for field "iobamaf" */
#define iobbdfpl 0x01 /* -   BAD FPL PASSED TO ASYNC COMP RTN          @L1A */
#define iobpgmck 0x02 /* -   PROGRAM CHECK ENCOUNTERED IN IGG019SY     @L1A */
#define iobbadrc 0x03 /* -   BAD RETURN CODE ENCOUNTERED FROM SMS -    @L1A */
#define iobbadlt 0x04 /* -   INVALID LOCATOR TOKEN DETECTED BY POINT   @L1A */
#define iobncrlt 0x05 /* -   RLT WAS INPUT BUT NO MEMBER WAS CONNECTED @L1A */
#define iobpout  0x06 /* -   DCB OPEN FOR OUTPUT & AN MLT FOR OTHER    @L1A */
#define ioboutio 0x07 /* -   OUTSTANDING I/O IN PROGRESS DETECTED      @L1A */
#define iobrlttb 0x08 /* -   RLT TOO BIG (>= HWM) DETECTED BY POINT    @L1A */
#define iobflock 0x09 /* -   CONNECT OR RECONNECT UNABLE TO GET FILE   @01A */
#define iobnfile 0x0A /* -   MLT INPUT TO POINT BUT NO SUCH FILE       @01A */
#define iobpad   0x0B /* -   PADDING ERROR DETECTED BY MEDIA MANAGER   @L2A */
#define iobmmerr 0x0C /* -   I/O ERROR DETECTED BY MEDIA MANAGER FOR   @L2A */
#define iobshdw  0x0D /* -   AN I/O ERROR ENCOUNTERED IN A SHADOW IOB. @L4A */
#define iobrbnnp 0x0E /* -   READ/WRITE REQUEST IS FOR A USER RBN WHICH@L4A */

#define iobgam   0x0C
#define iobqisam 0x0C

#pragma pack(pop)

#endif
