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
	PIXPOS tmp = {311,85,false};
	tmp.pix_X = atoi(argv[2]);
	tmp.pix_Y = atoi(argv[3]);
	bmp.get_pot(tmp);
	//输出图像的信息
	cout<<endl;
	return 0;
}
