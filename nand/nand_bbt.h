#ifndef _NAND_BBT_H_
#define _NAND_BBT_H_ 

struct a
{
	int ad;
	int csd;
	int dfl;
};
extern int nbbt_init();
extern int nbbt_scan();
extern int nbbt_writeSector(unsigned int sector,unsigned char *buf,unsigned int count);
extern int nbbt_readSector(unsigned int sector,unsigned char *buf,unsigned int count);
#endif
