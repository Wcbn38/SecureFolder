#include "../SecureFolderCore/SecureFolderCore.h"
#include <iostream>

using namespace std;

SFC_CALLBACK callback(char* outFile, bool status)
{
	cout << outFile << endl;
}

int main()
{ 
	fstream dest("..\\..\\map.tmp", ios_base::binary | ios_base::out | ios_base::trunc);

	char path[256];
	char entry[256];
	strcpy_s<256>(path, "..\\..\\testEnv");
	strcpy_s<256>(entry, ".");
	SFC::mapFolder(&dest, path, entry, NULL, callback);
	dest.close();

	dest.open("..\\..\\map.tmp", ios_base::binary | ios_base::in);
	cout << "-----------------------" << endl;
	strcpy_s<256>(path, "../../UNMAP/");
	SFC::unmapFolder(&dest, path, NULL, callback);

	dest.close();
}