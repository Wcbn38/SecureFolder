#include "../SecureFolderCore/SecureFolderCore.h"
#include <iostream>

using namespace std;

int main()
{
	cout << "STARTING" << endl;

	SFC::SECURE_FOLDER_PROPS props;
	props.cypherMethod = SFC::CYPHER_METHOD::SEQUENTIAL;
	props.cypherPath = (char*) "..\\..\\testEnv\\test.sfp";
	props.decypherMethod = SFC::DECYPHER_METHOD::COPY;
	props.decypherPath = (char*) "..\\..\\testEnv\\cyph\\";
	props.passwd = (char*) "passwd";
	props.savingMethod = SFC::KEY_SAVING_METHOD::PASSWORD;

	//SFC::cypher(props);
	//SFC::decypher(&props);

	cout << "FINISHED" << endl;
	while (getchar() != '\n');
}