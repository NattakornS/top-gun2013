/* "dgSIC4310App.h" */
#ifndef __DG_SIC4310_APP_H
#define __DG_SIC4310_APP_H

extern unsigned char sic4310RxBuff[];

void dgSic4310Init(void);
void dgSIC4310IncomingHandler(void);

void set_FlagNFCUpdated(void);
void clr_FlagNFCUpdated(void);
//char* getPtr_NFCRxBuff(void);
#endif	/* __DG_SIC4310_APP_H */


