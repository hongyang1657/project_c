
#include "fd_share_memory.h"


#define DATA_LENGTH 1024
 
using namespace std;

int main()
{
	u8 buffer[DATA_LENGTH];
	char chin[DATA_LENGTH];
	
	u32 length = sizeof(buffer);

	CShareMemory csm("fd", length);

	while (cin.getline(chin,DATA_LENGTH))
	{
		if (strcmp(chin,"exit")==0)
		{
			//cout << "exit" << endl;
			return 0;
		}
		strcpy((char*)buffer,chin);
		csm.PutToShareMem(buffer,DATA_LENGTH);
	}


	
	return 0;
}
