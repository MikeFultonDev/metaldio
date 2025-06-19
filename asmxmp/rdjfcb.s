         START , 
         YREGS ,                  register equates, syslib SYS1.MACLIB 
         IGWDES                   DSECTs for DESERV
         IGWSMDE
RDJFCB   CSECT , 
*
* DSECTs for USING
*

RDJFCB   AMODE 31 
RDJFCB   RMODE ANY
        SYSSTATE AMODE64=NO,ARCHLVL=OSREL,OSREL=SYSSTATE 
        IEABRCX  DEFINE    convert based branches to relative
*------------------------------------------------------------------- 
* Linkage and storage obtain
*------------------------------------------------------------------- 
         BAKR  R14,0                use linkage stack 
         LARL  R12,DATCONST         setup base for CONSTANTS
         USING DATCONST,R12         "baseless" CSECT

         LA    R1,WALEN
         BRASL R14,STG_OBTAIN_24    Get 24-bit cleared heap storage
         LR    R10,R1
         USING WAREA,R10
*
         MVC   SAVEA+4(4),=C'F1SA'  linkage stack convention 
         LAE   R13,SAVEA            ADDRESS OF OUR SA IN R13 

*------------------------------------------------------------------- 
* application logic                                                - 
*------------------------------------------------------------------- 

*
* DCB has to be below the line
*
         LA    R1,DCBLEN
         BRASL R14,STG_OBTAIN_24    Get 24-bit cleared heap storage
         LR    R8,R1
         USING DCBAREA,R8

*
*
* Copy the DCB template into 24-bit storage
* The OPEN_PARMS and DCBE is 31-bit to minimize below-line stg
*
         XR R6,R6                   Clear R6 (error code)
LIB_OPEN  DS  0H
         MVC LIB_DCB(DCBLEN),CONST_DCB
         MVC OPEN_PARMS(OPENLEN),CONST_OPEN
        OPEN (LIB_DCB,INPUT),MF=(E,OPEN_PARMS),MODE=31
         CIJE R15,0,OPEN_SUCCESS
*
OPEN_FAIL DS  0H
         LHI R8,OPEN_FAIL_MASK
         LR  R6,R15                put err code in R6
         OR  R6,R8
         B   DONE

EOM       DS  0H
          LA  R2,1
          BR  R14
*
OPEN_SUCCESS DS 0H

*
* Write DCB has to be below the line
*
         LA    R1,WDCBLEN
         BRASL R14,STG_OBTAIN_24    Get 24-bit cleared heap storage
         LR    R7,R1
         USING WDCBAREA,R7

*
*
* Copy the DCB template into 24-bit storage
* The OPEN_PARMS and DCBE is 31-bit to minimize below-line stg
*
         XR R6,R6                   Clear R6 (error code)
WOPEN    DS  0H
         MVC WDCB(WDCBLEN),CONST_WDCB
         MVC WOPEN_PARMS(WOPENLEN),CONST_WOPEN
        OPEN (WDCB,OUTPUT),MF=(E,WOPEN_PARMS),MODE=31
         CIJE R15,0,WOPEN_SUCCESS
*
WOPEN_FAIL DS  0H
         LHI R8,WOPEN_FAIL_MASK
         LR  R6,R15                put err code in R6
         OR  R6,R8
         B   DONE      
*
WOPEN_SUCCESS DS 0H

*
* JFCB DCB has to be below the line
*
         LA    R1,JFCB_DCBLEN
         BRASL R14,STG_OBTAIN_24    Get 24-bit cleared heap storage
         LR    R9,R1
         USING JFCB_DCB,R9

*
*
* Copy the DCB template into 24-bit storage
* Perform the RDJFCB to get a member
*
* Example to read PATH info from JFCB:                               *
*   https://tech.mikefulton.ca/DDNameReadDSAndPathEntries            *
* RDJFCB Info: https://tech.mikefulton.ca/RDJFCBOverview             *
* DCB Exit List: https://tech.mikefulton.ca/DCBExitList              *

RDJFCBE  DS  0H
         MVC JFCB_DCB(JFCB_DCBLEN),CONST_JFCB_DCB
*         
         LA    R6,0
         B     JFCB_OK
*
         RDJFCB MF=(E,JFCB_DCB)
         LTR   R15,R15
         BZ    JFCB_OK
*
         LHI R8,JFCB_FAIL_MASK
         LR  R6,R15                put err code in R6
         OR  R6,R8
         B   DONE            

INERROR  DS 0H

JFCB_OK  DS 0H
*
* Write result to OUTDD
*
WRITE_RESULT DS 0H
         MVC OUTREC(OUTMSGLEN),OUTMSG
         PUT WDCB,OUTREC      
*
LIB_CLOSE  DS 0H
         MVC CLOSE_PARMS(CLOSELEN),CONST_CLOSE
         CLOSE (LIB_DCB),MF=(E,CLOSE_PARMS),MODE=31
         CIJE R15,0,CLOSE_SUCCESS
*
CLOSE_FAIL DS  0H
         LHI R8,CLOSE_FAIL_MASK
         LR  R6,R15                put err code in R6
         OR  R6,R8
         B   DONE
*
CLOSE_SUCCESS DS 0H

*
WCLOSE  DS 0H
         MVC WCLOSE_PARMS(WCLOSELEN),CONST_WCLOSE
         CLOSE (WDCB),MF=(E,WCLOSE_PARMS),MODE=31
         CIJE R15,0,WCLOSE_SUCCESS
*
WCLOSE_FAIL DS  0H
         LHI R8,WCLOSE_FAIL_MASK
         LR  R6,R15                put err code in R6
         OR  R6,R8
         B   DONE
*
WCLOSE_SUCCESS DS 0H

*------------------------------------------------------------------- 
* Linkage and storage release. set RC (reg 15)                     -
*------------------------------------------------------------------- 
DONE     DS 0H
*
* Free storage
*
RLSE_STG DS 0H
        STORAGE RELEASE,ADDR=(R7),LENGTH=WDCBLEN,EXECUTABLE=NO 
         DROP R7
        STORAGE RELEASE,ADDR=(R8),LENGTH=DCBLEN,EXECUTABLE=NO 
         DROP R8
        STORAGE RELEASE,ADDR=(R9),LENGTH=JFCB_DCBLEN,EXECUTABLE=NO 
         DROP R9          
RLSE_WA  DS 0H
         STORAGE RELEASE,ADDR=(R10),LENGTH=WALEN,EXECUTABLE=NO 
         LR    R15,R6               get saved rc into R15
         PR    ,                    return to caller 

*
* STG_OBTAIN_24: 
* Acquire 24-bit storage and clear to 0
* - Linkage: Relative branch to here, Return to R14
* - Input R1: length of storage
* - Clobbers R 2,3,4,5,6,14,15
* - Output R1: address of allocated storage
*
STG_OBTAIN_24 DS 0D
         LR    R2,R1               R2 is copy of length 
         LR    R6,R14              R6 is copy of return address
        STORAGE OBTAIN,LENGTH=(R1),EXECUTABLE=NO,LOC=24,CHECKZERO=YES
*
* Clear storage
*
         CHI   R15,X'0014'           X'14': storage zeroed
         BE    STG_CLEAR
         LR    R3,R2                 copy length into R3
         LR    R2,R1                 system did not clear, do ourselves
         XR    R5,R5
         MVCL  R2,R4                 clear storage (pad byte zero)

STG_CLEAR DS 0H
          BR R6                      Return

*------------------------------------------------------------------- 
* constants and literal pool                                       - 
*------------------------------------------------------------------- 
DATCONST   DS    0D                 Doubleword alignment for LARL
CONST_DCB  DCB   DSORG=PO,MACRF=(R),DDNAME=INDD,DCBE=CONST_DCBE
DCBLEN    EQU   *-CONST_DCB
CONST_WDCB  DCB   DSORG=PS,MACRF=(PM),DDNAME=OUTDD,DCBE=CONST_WDCBE
WDCBLEN   EQU   *-CONST_WDCB

CONST_JFCB_DCB DCB DSORG=PO,MACRF=R,DDNAME=INDD,                       x
               EXLST=(0)
JFCB_DCBLEN    EQU   *-CONST_JFCB_DCB

INEXLST  DC    0F'0',AL1(EXLARL)      ENTRY CODE TO RETRIEVE
*                                     ALLOCATION INFORMATION
         DC    AL3(0)                 ADDR OF ALLOCATION RETRIEVAL LIST
         DC    AL1(EXLLASTE+EXLRJFCB) ENTRY CODE TO RETRIEVE FIRST JFCB
*                                     AND INDICATE LAST ENTRY IN LIST
         DC    AL3(0)                 ADDR OF JFCB FOR FIRST DATA SET

CONST_DCBE     DCBE  RMODE31=BUFF,EODAD=EOM
CONST_WDCBE    DCBE  RMODE31=BUFF

CONST_OPEN OPEN (*-*,(INPUT)),MODE=31,MF=L
OPENLEN   EQU   *-CONST_OPEN
CONST_CLOSE CLOSE (*-*),MODE=31,MF=L
CLOSELEN  EQU   *-CONST_CLOSE

CONST_WOPEN OPEN (*-*,(OUTPUT)),MODE=31,MF=L
WOPENLEN   EQU   *-CONST_WOPEN
CONST_WCLOSE CLOSE (*-*),MODE=31,MF=L
WCLOSELEN  EQU   *-CONST_WCLOSE

OUTMSG     DS 0H
OUTMSG_LEN  DC H'06'
OUTMSG_SPAN DC H'00'
OUTMSG_TXT  DC C'Hi'
OUTMSGLEN EQU *-OUTMSG

         LTORG ,

*------------------------------------------------------------------- 
* DSECT                                                            - 
*------------------------------------------------------------------- 
WAREA       DSECT 
SAVEA       DS    18F 
OPEN_PARMS  DS CL(OPENLEN)
CLOSE_PARMS DS CL(CLOSELEN)
WOPEN_PARMS  DS CL(WOPENLEN)
WCLOSE_PARMS DS CL(WCLOSELEN)
OUTREC       DS CL(2048+4)
WALEN       EQU  *-SAVEA

DCBAREA     DSECT
LIB_DCB     DS   CL(DCBLEN)
JFCB_AREA DSECT
JFCB_DCB    DS   CL(JFCB_DCBLEN)
WDCBAREA DSECT
WDCB        DS   CL(WDCBLEN)

*
*  AN ALLOCATION RETRIEVAL LIST FOLLOWS, POINTED TO BY DCB EXIT LIST.
*
SLBSTRT  IHAARL DSECT=YES,PREFIX=SLB
SLB_LEN  EQU *-SLBSTRT
LIBJFCB  DS     CL176' '         FIRST JFCB
LIBJFCBLEN EQU *-LIBJFCB
         IHAARA ,
         IHAEXLST ,             DCB exit list mapping

*------------------------------------------------------------------- 
* Equates                                                            - 
*------------------------------------------------------------------- 
OPEN_FAIL_MASK EQU   16
RDJFCB_FAIL_MASK EQU 32
CLOSE_FAIL_MASK EQU  64
WOPEN_FAIL_MASK EQU  48
WCLOSE_FAIL_MASK EQU 80
JFCB_FAIL_MASK EQU   96

         END   RDJFCB    
