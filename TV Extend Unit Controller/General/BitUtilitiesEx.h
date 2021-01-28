#ifndef BITUTILITIESEX_H_
#define BITUTILITIESEX_H_

typedef uint8_t BYTE;
typedef uint16_t WORD;
typedef uint32_t DWORD;

#define MAKEWORD(lowB, highB)	((WORD)(((BYTE)(((BYTE)(lowB)) & 0xff)) | ((WORD)((BYTE)(((BYTE)(highB)) & 0xff))) << 8))
#define MAKEDWORD(lowW, highW)	((DWORD)(((WORD)(((WORD)(lowW)) & 0xffff)) | ((DWORD)((WORD)(((WORD)(highW)) & 0xffff))) << 16))
#define LOWORD(l)				((WORD)(((DWORD)(l)) & 0xffff))
#define HIWORD(l)				((WORD)((((DWORD)(l)) >> 16) & 0xffff))
#define LOBYTE(w)				((BYTE)(((WORD)(w)) & 0xff))
#define HIBYTE(w)				((BYTE)((((WORD)(w)) >> 8) & 0xff))

#define sbi(PORT,bit)		(PORT|=(1<<bit))
#define cbi(PORT,bit)		(PORT&=~(1<<bit))
#define tbi(PORT,bit)		(PORT^=(1<<bit))

#endif /* BITUTILITIESEX_H_ */