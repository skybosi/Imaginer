#include "ibmp.h"
#include "Rbmp.h"
int main(int argc,char* argv[])
{
	if(argc < 3)
	{
		printf("%s usage: pic.bmp x y\n",argv[0]);
		return 0;
	}
	Rbmp bmp(argv[1]);
	PIXELS tmp;
	tmp.setXY(atoi(argv[2]),atoi(argv[3]));
	bmp.get_image_msg();
	bmp.get_pot(tmp);
	//bmp.get_pix(tmp);
	//bmp.read_image();
	//输出图像的信息
	cout<<endl;
	return 0;
}
