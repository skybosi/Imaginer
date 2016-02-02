#ifndef _IMAGE_H_
#define _IMAGE_H_

class Image
{
	private:
		unsigned char *pbuf;//读入图像数据的指针
		int width;//图像的宽
		int height;//图像的高
		RGBQUAD *pColorTable;//颜色表指针
		int biBitCount;//图像类型，每像素位数
	public:
		virtual bool read_image(const char* bmppath)=0;
		virtual bool write_image()=0;
		virtual bool save_image(char *bmppath,unsigned char *imgBuf,int width,int height, int biBitCount, RGBQUAD *pColorTable)=0;
};
#endif
