#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <string>
#include <string.h>
#include <vector>

char fileName[256];		//�f�t�H���g�t�@�C���l�[��
char fileExtension[256];	//�f�t�H���g�̊g���q

void Compression();		//���k
void Develop();			//�W�J

struct STR
{
	unsigned char *data;
	unsigned int len;
};

STR org;

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

void BlockSort_COMP(STR* orgData, STR* outData, unsigned int *index);
void BlockSort_DEV(STR* orgData, STR* outData, unsigned int *index);
void RLE_COMP(STR* orgData, STR* outData);
void RLE_DEV(STR* orgData, STR* outData);
void LZ78_COMP(STR* orgData, STR* outData);
void LZ78_DEV(STR* orgData, STR* outData);

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
	org.len = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	org.data = new unsigned char[org.len];

	fread(org.data, org.len, 1, fp);
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

}

//���k
void Compression()
{
	//���k�t�@�C��������
	char compFileName[256];
	strcpy(compFileName, fileName);
	strcat(compFileName, ".cmp");


	//�u���b�N�\�[�g
	STR BSdata;
	unsigned int index;
	BlockSort_COMP(&org, &BSdata, &index);


	//RLE���k
	STR RLEdata;
	RLE_COMP(&org, &RLEdata);

	

	//�t�@�C���o��
	FILE *fp;
	fp = fopen(compFileName, "wb");
	fwrite(RLEdata.data, 1, RLEdata.len, fp);
	fclose(fp);
	

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
	STR RLEdata;
	RLE_DEV(&org, &RLEdata);

	//�t�@�C���o��
	FILE *fp;
	fp = fopen(devFileName, "wb");
	fwrite(RLEdata.data, RLEdata.len, 1, fp);
	fclose(fp);

}

void BlockSort_COMP(STR* orgData, STR* outData, unsigned int *index)
{
	unsigned char* buff;
	buff = new unsigned char[orgData->len*2];
	for (int i = 0; i < orgData->len; i++)
	{
		buff[i] = orgData->data[i];
		buff[orgData->len + i] = orgData->data[i];
	}

	std::string *BSdata;
	BSdata = new std::string[orgData->len];

	for (int i = 0; i < orgData->len; i++)
	{
		BSdata[i].copy((char*)buff, i, orgData->len);
	}

	int hoge;
	hoge = 0;

}
void BlockSort_DEV(STR* orgData, STR* outData, unsigned int *index)
{

}


void RLE_COMP(STR* orgData, STR* outData)
{
	//RLE���k
	outData->len = 0;
	outData->data = new unsigned char[orgData->len * 2];

	unsigned int RLEcount = 0;

	for (unsigned int i = 0; i < orgData->len; i++)
	{
		//���̕����Ɠ���
		if (orgData->data[i] == orgData->data[i + 1])
		{
			//����������2�A���̂��Ƃɂ���ȏ�̘A�����𐔂���
			outData->data[outData->len] = orgData->data[i];
			outData->len++;
			outData->data[outData->len] = orgData->data[i];
			outData->len++;

			for (unsigned int Count = 0; Count < orgData->len;)
			{
				if (orgData->data[i + 1 + Count] == orgData->data[i + 2 + Count] && Count < 254)
				{
					Count++;
				}
				else
				{
					//�A�����̏o��
					outData->data[outData->len] = Count;
					outData->len++;

					//i��ǂݍ��ݐ����i�߂�
					i = i + 1 + Count;
					break;
				}
			}

		}
		else
		{
			outData->data[outData->len] = orgData->data[i];
			outData->len++;
		}
	}
}
void RLE_DEV(STR* orgData, STR* outData)
{
	outData->len = 0;
	outData->data = new unsigned char[orgData->len * 64];

	for (unsigned int i = 0; i < orgData->len; i++)
	{
		//���̕����������Ȃ�A�����[�h
		if (outData->data[i] == outData->data[i + 1])
		{
			outData->data[outData->len] = outData->data[i];
			outData->len++;
			outData->data[outData->len] = outData->data[i];
			outData->len++;

			for (int Count = 0; Count < outData->data[i + 2]; Count++)
			{
				outData->data[outData->len] = outData->data[i];
				outData->len++;
			}

			//i��i�߂�
			i += 2;
		}
		else
		{
			outData->data[outData->len] = outData->data[i];
			outData->len++;
		}
	}
}

void LZ78_COMP(STR* orgData, STR* outData)
{
	std::vector<LZ78> LZdata;
	LZdata.clear();

	//����������
	std::vector<DIC> dic;
	dic.clear();

	DIC stertDic;
	stertDic.data = (unsigned char *)"";
	stertDic.length = 0;
	dic.push_back(stertDic);

	unsigned int LZcur = 0;
	while (LZcur < orgData->len)
	{
		for (int i = dic.size() - 1; i >= 0; i--)
		{
			if (memcmp(&orgData->data[LZcur], dic.at(i).data, dic.at(i).length) == 0)
			{
				LZ78 data;
				data.index = i;
				data.data = orgData->data[LZcur + dic.at(i).length];
				LZdata.push_back(data);

				//�����Ƀf�[�^��ǉ�
				DIC add;
				add.data = &orgData->data[LZcur];
				add.length = dic.at(i).length + 1;
				dic.push_back(add);

				//�J�E���g��i�߂�
				LZcur += dic.at(i).length + 1;
				break;
			}
		}
	}

	//������ɏo��
	outData->len = 0;
	outData->data = new unsigned char[LZdata.size() * 3];
	for (unsigned int i = 0; i < LZdata.size(); i++)
	{
		//�C���f�b�N�X��short�^��char�^�ɕϊ�����
		unsigned char top = LZdata.at(i).index>>8;
		unsigned char bottom = LZdata.at(i).index & 0x00FF;

		outData->data[outData->len] = top;
		outData->len++;
		outData->data[outData->len] = bottom;
		outData->len++;
		outData->data[outData->len] = LZdata.at(i).data;
		outData->len++;
	}

}
void LZ78_DEV(STR* orgData, STR* outData)
{
	std::vector<LZ78> LZdata;
	LZdata.clear();

	//����������
	std::vector<DIC> dic;
	dic.clear();

	DIC stertDic;
	stertDic.data = (unsigned char *)"";
	stertDic.length = 0;
	dic.push_back(stertDic);

	for (unsigned int i = 0; i < orgData->len; i += 3)
	{
		unsigned short index = orgData->data[i] << 8 | orgData->data[i + 1];
		
		//�����ǉ�
		DIC addDic;
		addDic.data = &orgData->data[i + 2];
		addDic.length = dic.at(index).length + 1;
		dic.push_back(addDic);


	}
}