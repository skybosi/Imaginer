#include "ibmp.h"
#include "Rbmp.h"
#define beginY 443
#define rows   3
int main(int argc,char* argv[])
{
	if(argc < 5)
	{
		printf("%s usage: pic.bmp x y outpath\n",argv[0]);
		return 0;
	}
	//const char*path[] ={"","source/test24.bmp","source/test24.bmp","","source/test24.bmp",""};
	Rbmp bmp(argv[1]);
	//Rbmp bmp(path);
	//PIXELS tmp;
	//tmp.setXY(atoi(argv[2]),atoi(argv[3]));
	bmp.get_image_msg();
	//bmp.show_allData();
	bmp.get_pix(atoi(argv[2]),atoi(argv[3]));
	//bmp.get_pot(tmp);
	//bmp.read_image();
	//printf("\n%d %d\n",beginY,rows);
	//PIXELS** line = bmp.readIline(atoi(argv[2]),atoi(argv[3]));
	//bmp.delReadIline(line,atoi(argv[3]));
	//输出图像的信息
	bmp.getBoundaryLine();
	bmp.write_image(argv[4],"c");
	//bmp.imageSpatialize(argv[4]);
	//bmp.genHistogram(Red);
	//bmp.genHistogram(Green);
	//bmp.genHistogram(Blue);
	//bmp.genHistogram();
	cout<<endl;
	return 0;
}
