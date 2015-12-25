# TQ2440开发板摄像头驱动移植代码
####2015-11-15 Menglong Woo

该文件夹内所有内容属于从TQ2440移植过来的摄像头驱动，由于TQ2440原始LCD驱动是16bpp的，而我的驱动是24bpp，显示效果不同，将来我会抛开他自己写摄像头驱动，该驱动仅用于查看测试效果

## 测试方法
- 进入控制台后输入 **ckcam** 测试摄像头是否存在
- 进入控制台后输入 **camtest** 打开摄像头并捕捉视频显示到LCD上，按下 **ESC** 结束

## 存在问题
1. 所以显示时候存在色差
1. LCD显存是unsigned long lcd_buffer[x][y],比unsigned short lcd_buffer[x][y]大上2倍，显示存在分屏效果  
如下写法  

* 测试时能看到屏幕被划分成2个小窗口，LCD下半部分没有视频

```
	rCIPRCLRSA1 = (unsigned long)(&lcd_buffer[0][0]);
	rCIPRCLRSA2 = (unsigned long)(&lcd_buffer[0][0]);
	rCIPRCLRSA3 = (unsigned long)(&lcd_buffer[0][0]);
	rCIPRCLRSA4 = (unsigned long)(&lcd_buffer[0][0]);
	|--------------|--------------|
	|              |              |
	|    图象      |   图象       |
	|--------------|--------------|
	|              |              |
	|     LCD空白  |   LCD空白    |
	|--------------|--------------|
```

* 测试时能看到4个小窗口，LCD全屏显示4副画面

```
	
	rCIPRCLRSA1 = (unsigned long)(&lcd_buffer[0][0]);
	rCIPRCLRSA2 = (unsigned long)(&lcd_buffer[LCD_YSIZE_TFT/2][0]);
	rCIPRCLRSA3 = (unsigned long)(&lcd_buffer[0][0]);
	rCIPRCLRSA4 = (unsigned long)(&lcd_buffer[LCD_YSIZE_TFT/2][0]);
	|--------------|--------------|
	|              |              |
	|    图象      |   图象       |
	|--------------|--------------|
	|              |              |
	|    图象      |   图象       |
	|--------------|--------------|
```

于是我简单的将lcd驱动改成16bpp的（并没有完全改完），测试LCD时自己的测试程序候颜色仍旧不对，摄像头现实能全屏了，只不过画面仍旧闪动，也就是2440LCD硬件里关于4帧的处理没弄好，LCD有蓝色的闪烁


##将摄像头改成24bpp后显示一切正常，
####2015-11-16 Menglong Woo

下面是运行正常时CAM寄存器
```
--------- 2440 camif reg ---------
4f000000 rCISRCFMT              828001e0
4f000004 rCIWDOFST              80640064
4f000008 rCIGCTRL               24000000
4f000018 rCICOYSA1              32000000
4f00001c rCICOYSA2              32000000
4f000020 rCICOYSA3              32000000
4f000024 rCICOYSA4              32000000
4f000028rCICOCBSA1              3201fe00
4f00002c rCICOCBSA2             3201fe00
4f000030 rCICOCBSA3             3201fe00
4f000034 rCICOCBSA4             3201fe00
4f000038 rCICOCRSA1             3202fd00
4f00003c rCICOCRSA2             3202fd00
4f000040 rCICOCRSA3             3202fd00
4f000044 rCICOCRSA4             3202fd00
4f000048 rCICOTRGFMT            c1e0c110
4f00004c rCICOCTRL              00820840
4f000050 rCICOSCPRERATIO        a0010001
4f000054 rCICOSCPREDST          01b80118
4f000058 rCICOSCCTRL            60ea0107
4f00005c rCICOTAREA             0001fe00
4f000064 rCICOSTATUS            63c00000
4f00006c rCIPRCLRSA1            30015d04
4f000070 rCIPRCLRSA2            30015d04
4f000074 rCIPRCLRSA3            30015d04
4f000078 rCIPRCLRSA4            30015d04
4f00007c rCIPRTRGFMT            01e0c110
4f000080 rCIPRCTRL              00840000
4f000084 rCIPRSCPRERATIO        a0010001
4f000088 rCIPRSCPREDST          01b80118
4f00008c rCIPRSCCTRL            f0ea8107
4f000090 rCIPRTAREA             0001fe00
4f000098 rCIPRSTATUS            0da00000
4f0000a0 rCIIMGCPT              a0000000
```