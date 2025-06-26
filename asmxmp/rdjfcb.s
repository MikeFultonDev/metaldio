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
         LHI R0,WOPEN_FAIL_MASK
         LR  R6,R15                put err code in R6
         OR  R6,R0
         B   DONE      
*
WOPEN_SUCCESS DS 0H

* 
* Get storage for Exit List in R11
*
         LA    R1,EXLSTLEN
         BRASL R14,STG_OBTAIN_24    Get 24-bit cleared heap storage
         LR    R11,R1
         USING EXIT_LIST,R11
         MVC   EXIT_LIST(EXLSTLEN),CONST_EXLST
*
* Get storage for ARL List in R1 and initialize it
*
         LA    R1,ARL_LEN
         BRASL R14,STG_OBTAIN_24    Get 24-bit cleared heap storage
         LR    R8,R1
         USING ARL_LIST,R8
         MVC   0(ARL_LEN,R8),CONST_ARLLIST

*
* Request USS Path names and Dataset names
*
         LA    R0,ARLUSS
         ST    R0,ARLOPT1

* Store ARL List pointer into Exit List
         STCM  R1,B'0111',EXIT_ARL_ADDR

*
* Get storage for JFCB DCB and initialize it
*
         LA    R1,JFCB_DCBLEN
         BRASL R14,STG_OBTAIN_24    Get 24-bit cleared heap storage
         LR    R9,R1
         USING IHADCB,R9
         MVC   IHADCB(JFCB_DCBLEN),CONST_JFCB_DCB

* Store Exit List pointer into JFCB DCB
         ST    R11,DCBEXLST

* Store IHADCB pointer into RDJFCB
         ST    R9,RDJFCBP
         MVI   RDJFCBP,X'80'           SET FLAG IN PARMLIST

*
* Example to read PATH info from JFCB:                               *
*   https://tech.mikefulton.ca/DDNameReadDSAndPathEntries            *
* RDJFCB Info: https://tech.mikefulton.ca/RDJFCBOverview             *
* DCB Exit List: https://tech.mikefulton.ca/DCBExitList              *

RDJFCBE  DS  0H
*
* Read the JFCB
*
         RDJFCB MF=(E,RDJFCBP)
         LTR   R15,R15
         BZ    JFCB_OK

INERROR  DS 0H
         LHI R0,JFCB_FAIL_MASK
         LR  R6,R15                put err code in R6
         OR  R6,R0
         B   DONE            

JFCB_OK  DS 0H

PROCESS_JFCB   DS 0H
         ICM   R1,X'F',ARLAREA  GET AND TEST ADDRESS OF ARL
         BZ    DONE_NO_ARL      GO IF SYSTEM DOES NOT SUPPORT ARL
         CLI   ARLRCODE,0       TEST RDJFCB REASON CODE
         BNE   DONE_NO_INFO     BRANCH IF INFORMATION NOT AVAILABLE
*
*  Loop through the JFCBs.
*  Print the Dataset name or Path name as appropriate to output
*
         LH    R5,ARLRTRVD         R5 has # of JFCB's retrieved
         L     R2,ARLAREA          Set up R2 
         USING ARA,R2              R2 points to ARA
LOOP_ARA TM    ARAFLG,ARAXINF      Test if Extended Info
         BZ    USE_DS              If none, branch to use dataset
         SR    R3,R3               Clear R3
         IC    R3,ARAXINOF         Get double-word offset
         SLL   R3,3                Convert to byte offset
         AR    R3,R2               Set up R3
         USING ARAXINLN,R3         R3 points to Extended Info
         SR    R4,R4               Clear R4
         ICM   R4,B'0011',ARAPATHO Test if path available
         BZ    USE_DS              If no path, use dataset
         USING ARAPATHNAME,R4

USE_PATH DS 0H
* Write out path
         LA  R1,ARAPATHLEN
* msf - this 'path' is not being taken - it shows up as 'DS'
         ST  R1,0
         MVC OUTREC(PATHMSGLEN),PATHMSG
         PUT WDCB,OUTREC
         B     NEXT_ARA

USE_DS   DS 0H
* Write out dataset
         LA    R4,ARAJFCB
         USING JFCB,R4
         MVC OUTREC(DSMSGLEN),DSMSG
         PUT WDCB,OUTREC

NEXT_ARA AH    R2,ARALEN        POINT TO NEXT ARA ENTRY
         BCT   R5,LOOP_ARA      DECREMENT JFCB COUNTER, LOOP IF MORE
         B     DONE_JFCB
         
DONE_NO_ARL DS 0H
         LA R2,1
         ST R1,0
DONE_NO_INFO DS 0H
         LA R2,2
         ST R1,0
DONE_JFCB DS   0H

*
* Write result to OUTDD
*
WRITE_RESULT DS 0H
         MVC OUTREC(OUTMSGLEN),OUTMSG
         PUT WDCB,OUTREC      

*
WCLOSE  DS 0H
         MVC WCLOSE_PARMS(WCLOSELEN),CONST_WCLOSE
         CLOSE (WDCB),MF=(E,WCLOSE_PARMS),MODE=31
         CIJE R15,0,WCLOSE_SUCCESS
*
WCLOSE_FAIL DS  0H
         LHI R0,WCLOSE_FAIL_MASK
         LR  R6,R15                put err code in R6
         OR  R6,R0
         B   DONE
*
WCLOSE_SUCCESS DS 0H

*------------------------------------------------------------------- 
* Linkage and storage release. set RC (reg 6)                      -
*------------------------------------------------------------------- 
DONE     DS 0H
         LA R6,0
*
* Free storage
*
RLSE_STG DS 0H
        STORAGE RELEASE,ADDR=(R7),LENGTH=WDCBLEN,EXECUTABLE=NO 
         DROP R7
*
* There are more control blocks to free here... msf...
*
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
CONST_WDCB  DCB   DSORG=PS,MACRF=(PM),DDNAME=OUTDD,DCBE=CONST_WDCBE
WDCBLEN   EQU   *-CONST_WDCB

CONST_WDCBE    DCBE  RMODE31=BUFF

CONST_WOPEN OPEN (*-*,(OUTPUT)),MODE=31,MF=L
WOPENLEN   EQU   *-CONST_WOPEN
CONST_WCLOSE CLOSE (*-*),MODE=31,MF=L
WCLOSELEN  EQU   *-CONST_WCLOSE

OUTMSG     DS 0H
OUTMSG_LEN  DC H'06'
OUTMSG_SPAN DC H'00'
OUTMSG_TXT  DC C'xx'
OUTMSGLEN EQU *-OUTMSG

PATHMSG     DS 0H
PATHMSG_LEN  DC H'06'
PATHMSG_SPAN DC H'00'
PATHMSG_TXT  DC C'Pa'
PATHMSGLEN EQU *-PATHMSG

DSMSG     DS 0H
DSMSG_LEN  DC H'06'
DSMSG_SPAN DC H'00'
DSMSG_TXT  DC C'Ds'
DSMSGLEN EQU *-DSMSG

CONST_JFCB_DCB DCB MACRF=E,DDNAME=INDD,EXLST=(0)              
JFCB_DCBLEN    EQU   *-CONST_JFCB_DCB

CONST_EXLST DC 0F'0'
*         DC    AL1(EXLRJFCB)          ENTRY CODE FOR FIRST JFCB
*         DC    AL3(0)                 ADDR OF JFCB FOR FIRST DATA SET
         DC    AL1(EXLLASTE+EXLARL)   ENTRY CODE TO RETRIEVE
*                                     AND INDICATE LAST ENTRY IN LIST
*                                     ALLOCATION INFORMATION
         DC    AL3(0)                 ADDR OF ALLOCATION RETRIEVAL LIST
EXLSTLEN EQU *-CONST_EXLST
*
*  AN ALLOCATION RETRIEVAL LIST FOLLOWS, POINTED TO BY DCB EXIT LIST.
*
CONST_ARLLIST  IHAARL DSECT=NO,PREFIX=CAR
ARL_LEN  EQU *-CONST_ARLLIST
         LTORG ,

*------------------------------------------------------------------- 
* DSECT                                                            - 
*------------------------------------------------------------------- 
WAREA       DSECT 
SAVEA       DS    18F 
WOPEN_PARMS  DS CL(WOPENLEN)
WCLOSE_PARMS DS CL(WCLOSELEN)
OUTREC       DS CL(2048+4)
RDJFCBP      DS A
WALEN       EQU  *-SAVEA
 
WDCBAREA DSECT
WDCB        DS  CL(WDCBLEN)

         DS    0D
JFCB_DCB DSECT
         DS    CL(JFCB_DCBLEN)

         DCBD

ARL_LIST  IHAARL DSECT=YES,PREFIX=ARL

JFCB     DSECT
         IEFJFCBN LIST=YES

         IHAARA ,
         IHAEXLST ,             DCB exit list mapping

EXIT_LIST      DSECT
EXIT_EC_ARL    DS AL1
EXIT_ARL_ADDR  DS AL3
EXIT_EC_JFCB   DS AL1
EXIT_JFCB_ADDR DS AL3

*------------------------------------------------------------------- 
* Equates                                                            - 
*------------------------------------------------------------------- 
JFCB_FAIL_MASK EQU    16
RDJFCB_FAIL_MASK EQU  32
WOPEN_FAIL_MASK EQU   64
WCLOSE_FAIL_MASK EQU 128

         END   RDJFCB    
