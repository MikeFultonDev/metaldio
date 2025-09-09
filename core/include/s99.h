#ifndef __S99X__
#define __S99X__ 1

#include "metaldio.h"
#include "dbgopts.h"

#include <stdlib.h>
#include <stdio.h>

#pragma pack(1)

typedef enum s99_verb {
	S99VRBAL = 1,
	S99VRBUN = 2,
	S99VRBCC = 3,
	S99VRBDC = 4,
	S99VRBRI = 5,
	S99VRBDN = 6,
	S99VRBIN = 7
};

struct s99_flag1 {
	int s99oncnv:1;
	int s99nocnv:1;
	int s99nomnt:1;
	int s99jbsys:1;
	int s99cnenq:1;
	int s99gdgnt:1;
	int s99msglo:1;
	int s99nomig:1;
	int s99nosym:1;
	int s99acucb:1;
	int s99dsaba:1;
	int s99dxacu:1;
	int s99rsrv:4;
};

struct s99_flag2 {
	int s99wtvol:1;
	int s99wtdsn:1;
	int s99nores:1;
	int s99wtunt:1;
	int s99offln:1;
	int s99tionq:1;
	int s99catlg:1;
	int s99mount:1;
	int s99udevt:1;
	int s99rsrv1:1;
	int s99dyndi:1;
	int s99tioex:1;
	int s99dasup:1;
	int s99rsrv2:19;
};

struct s99_unit_entry {
	unsigned short s99tulng;
	char s99tupar[1];
};

struct s99_basic_text_unit {
	unsigned short s99tukey;
	unsigned short s99tunum;
	struct s99_unit_entry entry[1];
};

struct s99_common_text_unit {
	unsigned short s99tukey;
	unsigned short s99tunum;
	unsigned short s99tulng;
	char s99tupar[256];
};

#define DALEROPT_SKIP 0x40

#define BTOKLEN(member) (sizeof((SVC99BrowseTokenTextUnit_T*) 0)->member)
#define BTOKIDLEN (BTOKLEN(btokid))
#define BTOKIOTPLEN (BTOKLEN(btokiotp))
#define BTOKJKEYLEN (BTOKLEN(btokjkey))
#define BTOKASIDLEN (BTOKLEN(btokasid))
#define BTOKRCIDLEN (BTOKLEN(btokrcid))

#define BTOKACTBUF    0xFFFF
	#define BTOKBRWS 0
	#define BTOKSTKN 3
	#define BTOKVRNM 3

struct s99_browse_token_text_unit {
	unsigned short s99tukey;
	unsigned short s99tunum;
	unsigned short btokpl1;
	char btokid[4];
	unsigned short btokpl2;
	unsigned char btoktype;
	unsigned char btokvers;
	unsigned short btokpl3;
	char * PTR32 btokiotp;
	unsigned short btokpl4;
	unsigned int btokjkey;
	unsigned short btokpl5;
	unsigned short btokasid;
	unsigned short btokpl6;
	char btokrcid[8];
	unsigned short btokpl7;
	unsigned char btoklsdl;
	char btoklsda[254];
	char rsrv;
};

struct s99_text_unit {
	union {
		struct s99_common_text_unit ctu;
		struct s99_basic_text_unit btu;
		struct s99_browse_token_text_unit bttu;
	};
};

#define DALDDNAM 0x01
#define DALDSNAM 0x02
#define DALSTATS 0x04
#define DALNDISP 0x05
#define DALEROPT 0x3D
#define DALRTDDN 0x55
#define DALBRTKN 0x6E
#define DALSSREQ 0x5C

#define DUNDDNAM 0x01

#define DALSTATS_OLD   0x1
#define DALSTATS_MOD   0x2
#define DALSTATS_NEW   0x4
#define DALSTATS_SHR   0x8

struct s99_eopts {
	int s99eimsg:1;
	int s99ermsg:1;
	int s99elsto:1;
	int s99emkey:1;
	int s99emsub:1;
	int s99ewtp:1;
	int s99ersrv:2;
};

struct s99_emgsv {
	int s99xrsrv1:4;
	int s99xseve:1;
	int s99xwarn:1;
	int s99xrsrv2:2;
};

#define S99RBXVR 1
struct s99_rbx {
	char s99eid[6];
	char s99ever;
	struct s99_eopts s99eopts;
	char s99esubp;
	char s99ekey;
	struct s99_emgsv s99emgsv;
	char s99enmsg;
	void* PTR32 s99ecppl;
	char s99ercr;
	char s99ercm;
	char s99erco;
	char s99ercf;
	unsigned int s99ewrc;
	void* PTR32 s99emsgp;
	unsigned short s99eerr;
	unsigned short s99einfo;
	unsigned int s99ersn;
};

struct s99rb {
	unsigned char s99rbln;   /* length of request block-20*/
	enum s99_verb s99verb;
struct s99_flag1 s99flag1;
unsigned short s99error;
unsigned short s99info;
	struct s99_text_unit* PTR32 * PTR32 s99txtpp;
	struct s99_rbx* PTR32 s99s99x;
struct s99_flag2 s99flag2;
};

struct s99_em_bufs {
	unsigned short embufl1;
	unsigned short embufo1;
	char embuft1[251];
	char emrsrv1;
	unsigned short embufl2;
	unsigned short embufo2;
	char embuft2[251];
	char emrsrv2;
};

struct s99_em_wtdert {
	unsigned short emwtdesc;
	char emwtrtcd[16];
	unsigned short emrsrv;
};

struct s99_em {
	int emputlin:1;
	int emwtp:1;
	int emreturn:1;
	int emkeep:1;
	int emwtpcde:1;
	int emrsrv1:3;

	char emidnum;
	char emnmsgbk;
	char emsrsrv2;
	struct s99rb * PTR32 ems99rbp;
	unsigned int emretcod;
	void* PTR32 emcpplp;
	void* PTR32 embufp;
	unsigned int emsrsrv3;
	void* PTR32 emwtpcdp;

	struct s99_em_wtdert emwtdert;

	struct s99_em_bufs embuf;
};

#pragma pack(pop)

#define EMDAIR  1
#define EMFREE  51
#define EMSVC99 50

struct s99rb* PTR32 s99_init(enum s99_verb verb, struct s99_flag1 flag1, struct s99_flag2 flag2, struct s99_rbx* rbxin, size_t num_text_units, ...);
void s99_free(struct s99rb* PTR32 parms);
int s99_prt_msg(const DBG_Opts* opts, struct s99rb* PTR32 svc99parms, int svc99rc);
void s99_fmt_dmp(const DBG_Opts* opts, struct s99rb* PTR32 parms);

int S99(struct s99rb* PTR32 parms);
int S99MSG(struct s99_em* PTR32 parms);

#endif
