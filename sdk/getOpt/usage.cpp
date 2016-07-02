#include <iostream>
#include "getOpt.h"
using namespace std;
class Test : public OPt
{
	public:
		Test(int argc, char** argv) :OPt(argc, argv) 
	{
		setoptStr("B:cC:hH:m:M:R:s:S:T:Z:");
	}
		void manual();
		void setFunction();
	private:
		vvstr singleoptArray;
		mvstr multioptArray;
};
void Test::manual()
{
	cout << "  -T\timageTranspose  <- bool AR)          \n\t- Transpose a iamge\n";
	cout << "  -R\timageRevolution <- int px,int py,float angle) \n\t- Revolution a image\n";
	cout << "  -s\timageSpherize   <- float radius)     \n\t- Spherize a image\n";
	cout << "  -Z\timageZoom       <- float sclaex,float scaley) \n\t- zoom a image\n";
	cout << "  -M\timageMirror     <- Method method)    \n\t- Mirror a image\n";
	cout << "  -S\timageShear      <- bool XorY,float angle) \n\t- Shear a image\n";
	cout << "  -m\timageMove       <- int x,int y)      \n\t- move a image\n";
	cout << "  -C\tgetImage3Color  <- colorType color)  \n\t- get a image's 3(R,G,B) color image\n";
	cout << "  -H\tgenHistogram    <- colorType color)  \n\t- get a image's 3(R,G,B) color Histogram\n";
	cout << "  -B\tgenBardiagram   <- colorType color)  \n\t- get a image's 3(R,G,B) color Bar diagram\n";
	cout << "  -h\tboundarysHL     <- NONE) \n\t- change boundarys line to HightLight\n";
	cout << "  -c\timageCutOut     <- NONE) \n\t- cutout the effective part of the image\n";
}
void Test::setFunction()
{
	singleoptArray = getOptSarry(); //single option's arguments
	multioptArray = getOptMarry();//Mulit option's arguments
	if (singleoptArray.empty())
	{
		printf("Not single Option deal with!\n");
	}
	char dealType = 0;
	size_t i = 0;
	while (i < singleoptArray.size())
	{
		dealType = singleoptArray[i][0][1];
		switch (dealType)
		{
			case 'T':
				cout << "  -T     imageTranspose  : Transpose a iamge\n";
				break;
			case 'R':
				cout << "  -R     imageRevolution : Revolution a image\n";
				break;
			case 's':
				cout << "  -s     imageSpherize   : Spherize a image\n";
				break;
			case 'Z':
				cout << "  -Z     imageZoom       : zoom a image\n";
				break;
			case 'M':
				cout << "  -M     imageMirror     : Mirror a image\n";
				break;
			case 'S':
				cout << "  -S     imageShear      : Shear a image\n";
				break;
			case 'm':
				cout << "  -m     imageMove       : move a image\n";
				break;
			case 'C':
				cout << "  -C     getImage3Color  : get a image's 3(R,G,B) color image\n";
				break;
			case 'H':
				cout << "  -H     genHistogram    : get a image's 3(R,G,B) color Histogram\n";
				break;
			case 'B':
				cout << "  -B     genBardiagram   : get a image's 3(R,G,B) color Bar diagram\n";
				break;
			case 'h':
				cout << "  -h     boundarysHL     : change boundarys line to HightLight\n";
				break;
			case 'c':
				cout << "  -c     imageCutOut     : cutout the effective part of the image\n";
				break;
			default:
				printf("Not deal with!\n");
				break;
		}
		i++;
	}
	if (multioptArray.empty())
	{
		printf("Not Multi Option deal with!\n");
	}
	return;
}

int main(int argc, char **argv)
{
	Test Opts(argc, argv);
	if (argc < 2)
		Opts.manual();
	//Opts.setMultioptStr("help_h|add_a:|");
	Opts.getOpt();
	Opts.setFunction();
	printf("\n====================\n");
	Opts.showOptArray();
	return 0;
}
