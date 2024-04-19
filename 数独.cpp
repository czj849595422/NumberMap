#include <iostream>
#include "NumberMap.h"
using namespace std;


int main()
{
	NumberMap* nbm=new NumberMap("savepoint.txt");
	//NumberMap* nbm = new NumberMap();
	nbm->FindOneAnswer();

	return 0;
}
