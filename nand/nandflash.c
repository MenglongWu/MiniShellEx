// #include "nandflash.h"

// //硬件无关层
// /**
//  * @brief	向Nand写入1页数据
//  * @param	page_addr 写入页的起始地址
//  * @param	form 待写入页的内容
//  * @param	size 写入的大小
//  * @retval	待完善
//  * @remarks	调用底层驱动__low_nand_write_page
//  */
// // char nand_write_page(unsigned int page_addr,unsigned char *from,unsigned int size)
// // {
// // 	return __low_nand_write_page(page_addr,from,size);
// // }
 

// /**
//  * @brief	向Nand读出1页数据
//  * @param	page_addr 写入页的起始地址
//  * @param	to 待读出页的内容
//  * @param	size 读出的大小
//  * @retval	待完善
//  * @see	调用底层驱动__low_nand_read_page
//  */
//  void nand_read_page(unsigned int page_addr,unsigned char *to,unsigned int size)
// {
// 	__low_nand_read_page(page_addr,to,size);
// }


// /**
//  * @brief	擦除Nand 1块
//  * @param	block_addr 块地址
//  * @retval	待完善
//  * @remarks	调用底层驱动__low_nand_erase_block
//  */
// char nand_erase_block(unsigned int block_addr)
// {
// 	return __low_nand_erase_block(block_addr);
// }


// /**
//  * @brief	初始化Nand硬件接口
//  * @retval\n	NULL
//  * @remarks	调用底层驱动__low_nand_init
//  */
// void nand_init()
// {
// 	__low_nand_init();
// }

// /**
//  * @brief	读取Nand芯片设备号
//  * @retval\n	NULL
//  * @remarks	调用底层驱动__low_nand_readid，建议调试Nand驱动时首先测试该函数，
//  该函数接口目的在于测试驱动时序是否正确
//  * @see __low_nand_readid
//  */
// char nand_readid(unsigned int *part1,unsigned *part2)
// {
// 	return __low_nand_readid(part1,part2);
// }