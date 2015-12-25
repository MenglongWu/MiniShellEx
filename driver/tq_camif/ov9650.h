#ifndef _TQ2440_OV9650_H_
#define _TQ2440_OV9650_H_

#define CHIP_DELAY 0xFF

#define BUSHI_DEF 0

static struct OV9650_t {
	int subaddr;
	int value;
} ov9650_reg[] = 
{
	{0x12, 0x80},	// Camera Soft reset. Self cleared after reset.
	{CHIP_DELAY, 10},
//change 0x11[0x81->0x80], 0x15[0x02->0x10], hzh
//{0x12,0x80},
{0x11,0x80},{0x6a,0x3e},{0x3b,0x09},{0x13,0xe0},{0x01,0x80},{0x02,0x80},{0x00,0x00},{0x10,0x00},
{0x13,0xe5},{0x39,0x43},{0x38,0x12},{0x37,0x00},{0x35,0x91},{0x0e,0xa0},{0x1e,0x04},{0xA8,0x80},
{0x12,0x40},{0x04,0x00},{0x0c,0x04},{0x0d,0x80},{0x18,0xc6},{0x17,0x26},{0x32,0xad},{0x03,0x00},
{0x1a,0x3d},{0x19,0x01},{0x3f,0xa6},{0x14,0x2e},{0x15,0x10},{0x41,0x02},{0x42,0x08},{0x1b,0x00},
{0x16,0x06},{0x33,0xe2},{0x34,0xbf},{0x96,0x04},{0x3a,0x00},{0x8e,0x00},{0x3c,0x77},{0x8B,0x06},
{0x94,0x88},{0x95,0x88},{0x40,0xc1},{0x29,0x3f},{0x0f,0x42},{0x3d,0x92},{0x69,0x40},{0x5C,0xb9},
{0x5D,0x96},{0x5E,0x10},{0x59,0xc0},{0x5A,0xaf},{0x5B,0x55},{0x43,0xf0},{0x44,0x10},{0x45,0x68},
{0x46,0x96},{0x47,0x60},{0x48,0x80},{0x5F,0xe0},{0x60,0x8c},{0x61,0x20},{0xa5,0xd9},{0xa4,0x74},
{0x8d,0x02},{0x13,0xe7},{0x4f,0x3a},{0x50,0x3d},{0x51,0x03},{0x52,0x12},{0x53,0x26},{0x54,0x38},
{0x55,0x40},{0x56,0x40},{0x57,0x40},{0x58,0x0d},{0x8C,0x23},{0x3E,0x02},{0xa9,0xb8},{0xaa,0x92},
{0xab,0x0a},{0x8f,0xdf},{0x90,0x00},{0x91,0x00},{0x9f,0x00},{0xa0,0x00},{0x3A,0x01},{0x24,0x70},
{0x25,0x64},{0x26,0xc3},{0x2a,0x00},{0x2b,0x00},{0x6c,0x40},{0x6d,0x30},{0x6e,0x4b},{0x6f,0x60},
{0x70,0x70},{0x71,0x70},{0x72,0x70},{0x73,0x70},{0x74,0x60},{0x75,0x60},{0x76,0x50},{0x77,0x48},
{0x78,0x3a},{0x79,0x2e},{0x7a,0x28},{0x7b,0x22},{0x7c,0x04},{0x7d,0x07},{0x7e,0x10},{0x7f,0x28},
{0x80,0x36},{0x81,0x44},{0x82,0x52},{0x83,0x60},{0x84,0x6c},{0x85,0x78},{0x86,0x8c},{0x87,0x9e},
{0x88,0xbb},{0x89,0xd2},{0x8a,0xe6},
	//{0x15, 0x12},	// PCLK reverse
};

#define OV9650_REGS (sizeof(ov9650_reg)/sizeof(ov9650_reg[0]))


#endif /* _TQ2440_OV9650_H_ */
