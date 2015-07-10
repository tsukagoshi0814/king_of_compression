#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <string>
#include <string.h>
#include <vector>

char fileName[256];		//デフォルトファイルネーム
char fileExtension[256];	//デフォルトの拡張子

void Compression();		//圧縮
void Develop();			//展開

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
	//デフォルトのファイル名の切り取り
	for (int i = 0; *(argv[1] + i) != '.'; i++)
	{
		fileName[i] = *(argv[1] + i);
		if (*(argv[1] + i + 1) == '.')
		{
			fileName[i + 1] = '\0';
			break;
		}
	}

	//デフォルトの拡張子の切り取り
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

	//データの読み込み
	FILE *fp;
	fp = fopen(argv[1], "rb");
	fseek(fp, 0, SEEK_END);
	org.len = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	org.data = new unsigned char[org.len];

	fread(org.data, org.len, 1, fp);
	fclose(fp);

	//拡張子を確認して圧縮・展開
	if (memcmp(fileExtension, "bmp", 3) == 0)
	{
		//圧縮
		Compression();
	}
	else if (memcmp(fileExtension, "cmp", 3) == 0)
	{
		//展開
		Develop();
	}
	else
	{
		printf("読み込み失敗:拡張子\n");

		rewind(stdin);
		getchar();
	}

}

//圧縮
void Compression()
{
	//圧縮ファイル名生成
	char compFileName[256];
	strcpy(compFileName, fileName);
	strcat(compFileName, ".cmp");


	//ブロックソート
	STR BSdata;
	unsigned int index;
	BlockSort_COMP(&org, &BSdata, &index);


	//RLE圧縮
	STR RLEdata;
	RLE_COMP(&org, &RLEdata);

	

	//ファイル出力
	FILE *fp;
	fp = fopen(compFileName, "wb");
	fwrite(RLEdata.data, 1, RLEdata.len, fp);
	fclose(fp);
	

	//ハフマン圧縮

}
//展開
void Develop()
{
	//展開ファイル名生成
	char devFileName[256];
	strcpy(devFileName, fileName);
	strcat(devFileName, ".bmp");

	//RLE展開
	STR RLEdata;
	RLE_DEV(&org, &RLEdata);

	//ファイル出力
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
	//RLE圧縮
	outData->len = 0;
	outData->data = new unsigned char[orgData->len * 2];

	unsigned int RLEcount = 0;

	for (unsigned int i = 0; i < orgData->len; i++)
	{
		//次の文字と同じ
		if (orgData->data[i] == orgData->data[i + 1])
		{
			//同じ文字を2つ、そのあとにそれ以上の連続数を数える
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
					//連続数の出力
					outData->data[outData->len] = Count;
					outData->len++;

					//iを読み込み数分進める
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
		//次の文字が同じなら連続モード
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

			//iを進める
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

	//辞書初期化
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

				//辞書にデータを追加
				DIC add;
				add.data = &orgData->data[LZcur];
				add.length = dic.at(i).length + 1;
				dic.push_back(add);

				//カウントを進める
				LZcur += dic.at(i).length + 1;
				break;
			}
		}
	}

	//文字列に出力
	outData->len = 0;
	outData->data = new unsigned char[LZdata.size() * 3];
	for (unsigned int i = 0; i < LZdata.size(); i++)
	{
		//インデックスのshort型をchar型に変換する
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

	//辞書初期化
	std::vector<DIC> dic;
	dic.clear();

	DIC stertDic;
	stertDic.data = (unsigned char *)"";
	stertDic.length = 0;
	dic.push_back(stertDic);

	for (unsigned int i = 0; i < orgData->len; i += 3)
	{
		unsigned short index = orgData->data[i] << 8 | orgData->data[i + 1];
		
		//辞書追加
		DIC addDic;
		addDic.data = &orgData->data[i + 2];
		addDic.length = dic.at(index).length + 1;
		dic.push_back(addDic);


	}
}