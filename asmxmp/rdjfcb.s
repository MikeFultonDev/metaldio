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
        STORAGE OBTAIN,LENGTH=WALEN,EXECUTABLE=NO,LOC=24,CHECKZERO=YES
         LR    R10,R1               R10 points to Working Storage 
         USING WAREA,R10            BASE FOR DSECT 
*
* Clear storage
*
         CHI   R15,X'0014'           X'14': storage zeroed
         BE    STG_WA_CLEAR
         LR    R2,R1                 system did not clear, do ourselves
         LA    R3,WALEN
         XR    R5,R5
         MVCL  R2,R4                 clear storage (pad byte zero)

STG_WA_CLEAR DS 0H
*
         MVC   SAVEA+4(4),=C'F1SA'  linkage stack convention 
         LAE   R13,SAVEA            ADDRESS OF OUR SA IN R13 

*------------------------------------------------------------------- 
* application logic                                                - 
*------------------------------------------------------------------- 

*
* DCB has to be below the line
*
        STORAGE OBTAIN,LENGTH=DCBLEN,EXECUTABLE=NO,LOC=24,CHECKZERO=YES
         LR R8,R1                   R8 points to Input DCB
         USING DCBAREA,R8
*
* Clear storage
*
         CHI   R15,X'0014'           X'14': storage zeroed
         BE    STG_DCB_CLEAR
         LR    R2,R1                 system did not clear, do ourselves
         LA    R3,DCBLEN
         XR    R5,R5
         MVCL  R2,R4                 clear storage (pad byte zero)

STG_DCB_CLEAR DS 0H
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
        STORAGE OBTAIN,LENGTH=WDCBLEN,EXECUTABLE=NO,                   x
               LOC=24,CHECKZERO=YES
         LR R7,R1                   R7 points to Output DCB
         USING WDCBAREA,R7
*
* Clear storage
*
         CHI   R15,X'0014'           X'14': storage zeroed
         BE    STG_WDCB_CLEAR
         LR    R2,R1                 system did not clear, do ourselves
         LA    R3,WDCBLEN
         XR    R5,R5
         MVCL  R2,R4                 clear storage (pad byte zero)

STG_WDCB_CLEAR DS 0H
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
        STORAGE OBTAIN,LENGTH=JFCB_DCBLEN,EXECUTABLE=NO,               x
               LOC=24,CHECKZERO=YES
         LR R9,R1                   R9 points to JFCB DCB
         USING JFCB_DCBAREA,R9
*
* Clear storage
*
         CHI   R15,X'0014'           X'14': storage zeroed
         BE    STG_JFCB_DCB_CLEAR
         LR    R2,R1                 system did not clear, do ourselves
         LA    R3,JFCB_DCBLEN
         XR    R5,R5
         MVCL  R2,R4                 clear storage (pad byte zero)

STG_JFCB_DCB_CLEAR DS 0H

*
*
* Copy the DCB template into 24-bit storage
* Perform the RDJFCB to get a member
*
RDJFCBE  DS  0H
         MVC JFCB_DCB(JFCB_DCBLEN),CONST_JFCB_DCB 

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
* Free DCB storage
*
RLSE_STG DS 0H
        STORAGE RELEASE,ADDR=(R8),LENGTH=DCBLEN,EXECUTABLE=NO 
         DROP R8
        STORAGE RELEASE,ADDR=(R9),LENGTH=JFCB_DCBLEN,EXECUTABLE=NO 
         DROP R9          
RLSE_WA  DS 0H
         STORAGE RELEASE,ADDR=(R10),LENGTH=WALEN,EXECUTABLE=NO 
         LR    R15,R6               get saved rc into R15
         PR    ,                    return to caller 

*------------------------------------------------------------------- 
* constants and literal pool                                       - 
*------------------------------------------------------------------- 
DATCONST   DS    0D                 Doubleword alignment for LARL
CONST_DCB  DCB   DSORG=PO,MACRF=(R),DDNAME=INDD,DCBE=CONST_DCBE
DCBLEN    EQU   *-CONST_DCB
CONST_WDCB  DCB   DSORG=PS,MACRF=(PM),DDNAME=OUTDD,DCBE=CONST_WDCBE
WDCBLEN   EQU   *-CONST_WDCB

CONST_JFCB_DCB  DCB   DSORG=PS,MACRF=(R),DDNAME=INDD
JFCB_DCBLEN    EQU   *-CONST_JFCB_DCB
CONST_DCBE DCBE  RMODE31=BUFF,EODAD=EOM
CONST_WDCBE DCBE  RMODE31=BUFF

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
JFCB_DCBAREA DSECT
JFCB_DCB    DS   CL(JFCB_DCBLEN)
WDCBAREA DSECT
WDCB        DS   CL(WDCBLEN)

*------------------------------------------------------------------- 
* Equates                                                            - 
*------------------------------------------------------------------- 
OPEN_FAIL_MASK EQU   16
RDJFCB_FAIL_MASK EQU 32
CLOSE_FAIL_MASK EQU  64
WOPEN_FAIL_MASK EQU   7
WCLOSE_FAIL_MASK EQU 15



         END   RDJFCB    
