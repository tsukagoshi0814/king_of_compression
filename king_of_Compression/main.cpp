#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <string.h>
#include <vector>

char fileName[256];		//デフォルトファイルネーム
char fileExtension[256];	//デフォルトの拡張子
unsigned int orgLen;
unsigned char *orgData;

void Compression();		//圧縮
void Develop();			//展開

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
	orgLen = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	orgData = new unsigned char[orgLen];

	fread(orgData, orgLen, 1, fp);
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

#ifdef _DEBUG
	rewind(stdin);
	getchar();
#endif

}

//圧縮
void Compression()
{
	//圧縮ファイル名生成
	char compFileName[256];
	strcpy(compFileName, fileName);
	strcat(compFileName, ".cmp");

	//RLE圧縮
	unsigned int RLEoutCount = 0;
	unsigned char *RLEdata;
	RLEdata = new unsigned char[orgLen * 2];

	unsigned int RLEcount = 0;

	for (unsigned int i = 0; i < orgLen; i++)
	{
		//次の文字と同じ
		if (orgData[i] == orgData[i+1])
		{
			//同じ文字を2つ、そのあとにそれ以上の連続数を数える
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
					//連続数の出力
					RLEdata[RLEoutCount] = Count;
					RLEoutCount++;

					//iを読み込み数分進める
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

	//ファイル出力
	FILE *fp;
	fp = fopen(compFileName, "wb");
	fwrite(RLEdata, 1, RLEoutCount, fp);
	fclose(fp);
	
	/*
	//LZ78圧縮
	std::vector<LZ78> LZdata;
	LZdata.clear();

	//辞書初期化
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

				//辞書にデータを追加
				DIC add;
				add.data = &RLEdata[LZcur];
				add.length = dic.at(i).length + 1;
				dic.push_back(add);
				
				//カウントを進める
				LZcur += dic.at(i).length + 1;
				break;
			}
		}
	}





	////ファイル出力
	//FILE *fp;
	//fp = fopen(compFileName, "wb");
	//for (int i = 0; i < LZdata.size(); i++)
	//{
	//	fprintf(fp, "%d%c", LZdata.at(i).index, LZdata.at(i).data);
	//}
	//fclose(fp);

	*/

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
	unsigned int RLEoutCount = 0;
	unsigned char *RLEdata;
	RLEdata = new unsigned char[orgLen*64];

	for (unsigned int i = 0; i < orgLen; i++)
	{
		//次の文字が同じなら連続モード
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

			//iを進める
			i += 2;
		}
		else
		{
			RLEdata[RLEoutCount] = orgData[i];
			RLEoutCount++;
		}
	}

	//ファイル出力
	FILE *fp;
	fp = fopen(devFileName, "wb");
	fwrite(RLEdata, RLEoutCount, 1, fp);
	fclose(fp);

}