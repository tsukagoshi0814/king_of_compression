#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <string.h>
#include <vector>

char fileName[256];		//�f�t�H���g�t�@�C���l�[��
char fileExtension[256];	//�f�t�H���g�̊g���q
unsigned int orgLen;
unsigned char *orgData;

void Compression();		//���k
void Develop();			//�W�J

struct DIC
{
	unsigned char *data;
	int length;
};

struct LZ78
{
	unsigned short index;
	unsigned char data;
};

void main(int argc, char* argv[])
{
	//�f�t�H���g�̃t�@�C�����̐؂���
	for (int i = 0; *(argv[1] + i) != '.'; i++)
	{
		fileName[i] = *(argv[1] + i);
		if (*(argv[1] + i + 1) == '.')
		{
			fileName[i + 1] = '\0';
			break;
		}
	}

	//�f�t�H���g�̊g���q�̐؂���
	bool extensionFlag = false;
	int extensionCount = 0;
	for (int i = 0; *(argv[1] + i) != '\0'; i++)
	{
		if (extensionFlag)
		{
			fileExtension[extensionCount] = *(argv[1] + i);
			extensionCount++;
		}
		if (*(argv[1] + i) == '.')
		{
			extensionFlag = true;
		}
	}

	//�f�[�^�̓ǂݍ���
	FILE *fp;
	fp = fopen(argv[1], "rb");
	fseek(fp, 0, SEEK_END);
	orgLen = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	orgData = new unsigned char[orgLen];

	fread(orgData, orgLen, 1, fp);
	fclose(fp);

	//�g���q���m�F���Ĉ��k�E�W�J
	if (memcmp(fileExtension, "bmp", 3) == 0)
	{
		//���k
		Compression();
	}
	else if (memcmp(fileExtension, "cmp", 3) == 0)
	{
		//�W�J
		Develop();
	}
	else
	{
		printf("�ǂݍ��ݎ��s:�g���q\n");

		rewind(stdin);
		getchar();
	}

#ifdef _DEBUG
	rewind(stdin);
	getchar();
#endif

}

//���k
void Compression()
{
	//���k�t�@�C��������
	char compFileName[256];
	strcpy(compFileName, fileName);
	strcat(compFileName, ".cmp");

	//RLE���k
	unsigned int RLEoutCount = 0;
	unsigned char *RLEdata;
	RLEdata = new unsigned char[orgLen * 2];

	unsigned int RLEcount = 0;

	for (unsigned int i = 0; i < orgLen; i++)
	{
		//���̕����Ɠ���
		if (orgData[i] == orgData[i+1])
		{
			//����������2�A���̂��Ƃɂ���ȏ�̘A�����𐔂���
			RLEdata[RLEoutCount] = orgData[i];
			RLEoutCount++;
			RLEdata[RLEoutCount] = orgData[i];
			RLEoutCount++;

			for (unsigned int Count = 0; Count < orgLen; )
			{
				if (orgData[i + 1 + Count] == orgData[i + 2 + Count] && Count < 254)
				{
					Count++;
				}
				else
				{
					//�A�����̏o��
					RLEdata[RLEoutCount] = Count;
					RLEoutCount++;

					//i��ǂݍ��ݐ����i�߂�
					i = i + 1 + Count;
					break;
				}
			}

		}
		else
		{
			RLEdata[RLEoutCount] = orgData[i];
			RLEoutCount++;
		}
	}

	//�t�@�C���o��
	FILE *fp;
	fp = fopen(compFileName, "wb");
	fwrite(RLEdata, 1, RLEoutCount, fp);
	fclose(fp);
	
	/*
	//LZ78���k
	std::vector<LZ78> LZdata;
	LZdata.clear();

	//����������
	std::vector<DIC> dic;
	dic.clear();

	DIC stertDic;
	stertDic.data = (unsigned char *)"";
	stertDic.length = 0;
	dic.push_back(stertDic);

	int LZcur = 0;
	while (LZcur < RLEoutCount)
	{
		for (int i = dic.size() - 1; i >= 0; i--)
		{
			if (memcmp(&RLEdata[LZcur], dic.at(i).data, dic.at(i).length) == 0)
			{
				LZ78 data;
				data.index = i;
				data.data = RLEdata[LZcur + dic.at(i).length];
				LZdata.push_back(data);

				//�����Ƀf�[�^��ǉ�
				DIC add;
				add.data = &RLEdata[LZcur];
				add.length = dic.at(i).length + 1;
				dic.push_back(add);
				
				//�J�E���g��i�߂�
				LZcur += dic.at(i).length + 1;
				break;
			}
		}
	}





	////�t�@�C���o��
	//FILE *fp;
	//fp = fopen(compFileName, "wb");
	//for (int i = 0; i < LZdata.size(); i++)
	//{
	//	fprintf(fp, "%d%c", LZdata.at(i).index, LZdata.at(i).data);
	//}
	//fclose(fp);

	*/

	//�n�t�}�����k

}

//�W�J
void Develop()
{
	//�W�J�t�@�C��������
	char devFileName[256];
	strcpy(devFileName, fileName);
	strcat(devFileName, ".bmp");

	//RLE�W�J
	unsigned int RLEoutCount = 0;
	unsigned char *RLEdata;
	RLEdata = new unsigned char[orgLen*64];

	for (unsigned int i = 0; i < orgLen; i++)
	{
		//���̕����������Ȃ�A�����[�h
		if (orgData[i] == orgData[i+1])
		{
			RLEdata[RLEoutCount] = orgData[i];
			RLEoutCount++;
			RLEdata[RLEoutCount] = orgData[i];
			RLEoutCount++;

			for (int Count = 0; Count < orgData[i + 2]; Count++)
			{
				RLEdata[RLEoutCount] = orgData[i];
				RLEoutCount++;
			}

			//i��i�߂�
			i += 2;
		}
		else
		{
			RLEdata[RLEoutCount] = orgData[i];
			RLEoutCount++;
		}
	}

	//�t�@�C���o��
	FILE *fp;
	fp = fopen(devFileName, "wb");
	fwrite(RLEdata, RLEoutCount, 1, fp);
	fclose(fp);

}