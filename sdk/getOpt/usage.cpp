#include <iostream>
#include <string>
#include "getOpt.h"
using namespace std;

char* usage()
{
	std::string usage = std::string("-T\timageTranspose  <- bool AR) \n\t- Transpose a iamge\n")      + 
	"-R\timageRevolution <- int px,int py,float angle) \n\t- Revolution a image\n"               +
	"-s\timageSpherize   <- float radius) \n\t- Spherize a image\n"                          +
	"-Z\timageZoom       <- float sclaex,float scaley) \n\t- zoom a image\n"                     +
	"-M\timageMirror     <- Method method) \n\t- Mirror a image\n"                            +
	"-S\timageShear      <- bool XorY,float angle) \n\t- Shear a image\n"                        +
	"-m\timageMove       <- int x,int y) \n\t- move a image\n"                              +
	"-C\tgetImage3Color  <- colorType color) \n\t- get a image's 3(R,G,B) color image\n"        +
	"-H\tgenHistogram    <- colorType color) \n\t- get a image's 3(R,G,B) color Histogram\n"    +
	"-B\tgenBardiagram   <- colorType color) \n\t- get a image's 3(R,G,B) color Bar diagram\n"  +
	"-h\tboundarysHL     <- NONE) \n\t- change boundarys line to HightLight\n"                   +
	"-c\timageCutOut     <- NONE) \n\t- cutout the effective part of the image\n";
	return (char*)usage.c_str();
}

void test(int a,int b,int c)
{
	printf("argv: %d %d %d", a, b, c);
}
void function(const OPt& opt)
{
	char dealType = 0;
	size_t i = 0;
	while (i < opt.ssize())
	{
		//dealType = singleoptArray[i][0][1];
		dealType = opt.option(i);
		OPt::vargv argv = opt.argvs(i);;
		switch (dealType)
		{
			case 'T':
				cout << "  -T     imageTranspose  : Transpose a iamge\n";
				break;
			case 'R':
				cout << "  -R     imageRevolution : Revolution a image\n";
				printf("argv: %d %ld %f", (int)argv[1], (long)argv[2], (float)argv[3]);
				test(argv[1], argv[2], argv[3]);
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
	if (! opt.msize())
	{
		printf("Not Multi Option deal with!\n");
	}
	return;
}

int main(int argc, char **argv)
{
	OPt Opts(argc, argv,"B:cC:hH:m:M:R:s:S:T:Z:", usage());
	if (argc < 2)
	{
		printf("%s", Opts.manual());
	}
	//Opts.setMultioptStr("help_h|add_a:|");
	Opts.handle(function);
	printf("\n====================\n");
	Opts.showOptArray();
	return 0;
}
