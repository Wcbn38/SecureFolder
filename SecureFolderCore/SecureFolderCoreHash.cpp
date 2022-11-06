#include "SecureFolderCore.h"

using namespace SFC;

//types
typedef unsigned long WORD;
typedef unsigned long long EXTENDED_WORD;

typedef struct
{
	WORD word[16];
} WORD512;

typedef struct
{
	WORD regA, regB, regC, regD, regE, regF, regG, regH;
} REGISTERS;

//basic operations
#define ROTR(word, shiftLength) ((word >> shiftLength) | (word << (32 - shiftLength)))

WORD sum(WORD* words, short length)
{
	EXTENDED_WORD result = 0;

	for (short i = 0; i < length; i++)
	{
		result += length;
	}

	return result & 0xFFFFFFFF;
}

//mathematical functions
#define SIGMA0(a) (ROTR(a, 7) ^ ROTR(a, 18) ^ (a >> 3))
#define SIGMA1(a) (ROTR(a, 17) ^ ROTR(a, 19) ^ (a >> 10))

#define UP_SIGMA0(a) (ROTR(a, 2) ^ ROTR(a, 13) ^ ROTR(a, 22))
#define UP_SIGMA1(a) (ROTR(a, 6) ^ ROTR(a, 11) ^ ROTR(a, 25))

#define CHOICE(swt, ch1, ch2) ((swt & ch1) | ((~swt) & ch2))
#define MAJ(a, b, c) ((a & b) | (a & c) | (b & c))

//types opertions
void addRegisters(REGISTERS* base, REGISTERS* add)
{
	base->regA += add->regA;
	base->regB += add->regB;
	base->regC += add->regC;
	base->regD += add->regD;
	base->regE += add->regE;
	base->regF += add->regF;
	base->regG += add->regG;
	base->regH += add->regH;
}

//constants
static const WORD k[64] = {
	0x428a2f98,0x71374491,0xb5c0fbcf,0xe9b5dba5,0x3956c25b,0x59f111f1,0x923f82a4,0xab1c5ed5,
	0xd807aa98,0x12835b01,0x243185be,0x550c7dc3,0x72be5d74,0x80deb1fe,0x9bdc06a7,0xc19bf174,
	0xe49b69c1,0xefbe4786,0x0fc19dc6,0x240ca1cc,0x2de92c6f,0x4a7484aa,0x5cb0a9dc,0x76f988da,
	0x983e5152,0xa831c66d,0xb00327c8,0xbf597fc7,0xc6e00bf3,0xd5a79147,0x06ca6351,0x14292967,
	0x27b70a85,0x2e1b2138,0x4d2c6dfc,0x53380d13,0x650a7354,0x766a0abb,0x81c2c92e,0x92722c85,
	0xa2bfe8a1,0xa81a664b,0xc24b8b70,0xc76c51a3,0xd192e819,0xd6990624,0xf40e3585,0x106aa070,
	0x19a4c116,0x1e376c08,0x2748774c,0x34b0bcb5,0x391c0cb3,0x4ed8aa4a,0x5b9cca4f,0x682e6ff3,
	0x748f82ee,0x78a5636f,0x84c87814,0x8cc70208,0x90befffa,0xa4506ceb,0xbef9a3f7,0xc67178f2
};

//user-end functions
KEY256 SFC::getSHA256(const char* input, long long size)
{
	//Startup
	long nbStacks = ((size + 8 - ((size + 8) % 64)) / 64) + 1; //in bytes!!! 56B of data, 6 of padding
	int j, i;

	WORD512* stacks = new WORD512[nbStacks];

	/*CREATION OF WORD STACKS*/
	for (i = 0; i < nbStacks - 1; i++) //writes full stacks
	{
		for (j = 0; j < 16; j++) //writes the input
		{
			stacks[i].word[j] = input[i * 14 * 4 + j * 4 + 3];				//creates a 4 Bytes value
			stacks[i].word[j] |= input[i * 14 * 4 + j * 4 + 2] << 8;
			stacks[i].word[j] |= input[i * 14 * 4 + j * 4 + 1] << 16;
			stacks[i].word[j] |= input[i * 14 * 4 + j * 4] << 24;
		}
	}

	//writes last stack
	for (j = 0; j < 14; j++)
	{
		stacks[nbStacks - 1].word[j] = 0;
	}

	for (j = 0; j < size % 56; j++) //writes all the left bytes
	{
		stacks[nbStacks - 1].word[(j - (j % 4)) / 4] |= (input[(nbStacks - 1) * 14 * 4 + j] << ((3 - (j % 4)) * 8));				//creates a 4 Bytes value
	}

	stacks[nbStacks - 1].word[14] = ((size % 56) * 8) & (0xFFFFFFFFFFFFFFFF << 32);
	stacks[nbStacks - 1].word[15] = ((size % 56) * 8) & 0xFFFFFFFFFFFFFFFF;
	stacks[nbStacks - 1].word[(j - (j % 4)) / 4] |= 0x1 << ((3 - (j % 4)) * 8 + 7);

	/*COMPRESSION*/
	REGISTERS reg0, reg1;
	reg0.regA = 0x6A09E667;
	reg0.regB = 0xBB67AE85;
	reg0.regC = 0x3C6EF372;
	reg0.regD = 0xA54FF53A;
	reg0.regE = 0x510E527F;
	reg0.regF = 0x9B05688C;
	reg0.regG = 0x1F83D9AB;
	reg0.regH = 0x5BE0CD19;

	for (i = 0; i < nbStacks; i++)
	{
		//Creation of the scheduler
		WORD msgSchedule[64];

		for (j = 0; j < 16; j++) //Fills up the scheduler of data
		{
			msgSchedule[j] = stacks[i].word[j];
		}

		for (j = 16; j < 64; j++) //Fills up the scheduler entirely
		{
			msgSchedule[j] = msgSchedule[j - 16] + SIGMA0(msgSchedule[j - 15]) + msgSchedule[j - 7] + SIGMA1(msgSchedule[j - 2]);
		}

		WORD t1, t2;
		reg1 = reg0;

		for (int i = 0; i < 64; i++)
		{
			t1 = UP_SIGMA1(reg1.regE) + CHOICE(reg1.regE, reg1.regF, reg1.regG) + reg1.regH + msgSchedule[i] + k[i];
			t2 = UP_SIGMA0(reg1.regA) + MAJ(reg1.regA, reg1.regB, reg1.regC);

			reg1.regH = reg1.regG;
			reg1.regG = reg1.regF;
			reg1.regF = reg1.regE;
			reg1.regE = reg1.regD + t1; //because regE not used after
			reg1.regD = reg1.regC;
			reg1.regC = reg1.regB;
			reg1.regB = reg1.regA;
			reg1.regA = t1 + t2;
		}

		addRegisters(&reg0, &reg1);
	}

	KEY256 result;
	result.key[0] = reg0.regA;
	result.key[1] = reg0.regB;
	result.key[2] = reg0.regC;
	result.key[3] = reg0.regD;
	result.key[4] = reg0.regE;
	result.key[5] = reg0.regF;
	result.key[6] = reg0.regG;
	result.key[7] = reg0.regH;

	//Cleanup
	delete[] stacks;

	return result;
}

void SFC::cypherRawData(unsigned char* data, long size, SFC::KEY256* key)
{
	unsigned char* charKey = (unsigned char*) key;

	for (long i = 0; i < size; i++)
	{
		data[i] = data[i] ^ (charKey[i % sizeof(SFC::KEY256)]);
	}
}

void SFC::cypherRawDataEvolve(unsigned char* data, long size, SFC::KEY256* key)
{
	unsigned char* charKey = (unsigned char*)key;

	for (long i = 0; i < size; i += sizeof(SFC::KEY256))
	{
		SFC::cypherKeyEvolve(key);

		for (char j = 0; (j < sizeof(SFC::KEY256)) && ((i + j) < size); j++)
		{
			data[i + j] ^= (charKey[j]);
		}
	}
}

void SFC::cypherKeyEvolve(SFC::KEY256* key)
{
	key->key[0] = ROTR(key->key[0], 3);
	key->key[1] = ROTR(key->key[1], 5);
	key->key[2] = ROTR(key->key[2], 7);
	key->key[3] = ROTR(key->key[3], 11);
	key->key[4] = ROTR(key->key[4], 13);
	key->key[5] = ROTR(key->key[5], 17);
	key->key[6] = ROTR(key->key[6], 19);
	key->key[7] = ROTR(key->key[7], 23);
}