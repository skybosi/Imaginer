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
//	bmp.read_image();
	PIXPOS tmp = {0};
	tmp.pix_X = atoi(argv[2]);
	tmp.pix_Y = atoi(argv[3]);
	bmp.get_image_msg();
	bmp.get_pot8(tmp);
	//输出图像的信息
	cout<<endl;
	return 0;
}
