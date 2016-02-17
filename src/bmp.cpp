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
	Rbmp bmp(argv[1]);
	PIXELS tmp;
	tmp.setXY(atoi(argv[2]),atoi(argv[3]));
	bmp.get_image_msg();
	//bmp.show_allData();
	//bmp.get_pix(atoi(argv[2]),atoi(argv[3]));
	//bmp.get_pot(tmp);
	//bmp.read_image();
	//printf("\n%d %d\n",beginY,rows);
	PIXELS** line = bmp.readIline(atoi(argv[2]),atoi(argv[3]));
	bmp.delReadIline(line,atoi(argv[3]));
	//输出图像的信息
	bmp.write_image(argv[4]);
	cout<<endl;
	return 0;
}
