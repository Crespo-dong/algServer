#ifndef _COMMON_H_
#define _COMMON_H_
//����ͻ��˺ͷ������˹��õ����ݽṹ
#include <string>
using std::string;

/*
 ���ݰ���ʽ��
 |    4Byte    |    4Byte    |        nByte        |
 |    ����     |    ����     |        ����         |
               |--         nPackDataLen          --|
                           	 |--  nBlockDataLen  --|
 */

// ���Կͻ��˵����ݰ������ͣ�
enum ClientPackType
{
	FACEGROUP,
	HEARTBEAT
};
// ���Է���˵����ݰ�������
enum ServerPackType
{
	RECOGNIZE_RESULT
};

//������Ϣ��
class FaceInfo
{
public:
	string stGUID; //������GUID
	int nWidth; //ͼ����
	int nHeight; //ͼ��߶�
	int nChannel; //ͼ��ͨ����
	int nImageDataLen; //ͼ�����ݳ���
	char* pImageData; //ͼ������ ����ڴ��ɸ��ฺ���ͷ�
public:
	FaceInfo();
	~FaceInfo();
	//���ؿ������캯���͸�ֵ������������
	FaceInfo(const FaceInfo& faceInfo);
	FaceInfo& operator=(const FaceInfo& faceInfo);
	//faceInfoΪ���������pDataBlock��nDataLenΪ��������
	static void ObjectToDataBlock(const FaceInfo& faceInfo, char*& pDataBlock, int& nDataLen);
	//pDataBlockΪ���������dataLenΪ��������
	static FaceInfo DataBlockToObject(char* pDataBlock, int& dataLen);
};

//һ��ʶ������������������Ϣ������������������Ϣ
class FaceInfoGroup
{
public:
	FaceInfo faceInfoArr[2];

	//faceInfoGroupΪ���������pDataBlock��nDataLenΪ��������
	static void ObjectToDataBlock(const FaceInfoGroup& faceInfoGroup, char*& pDataBlock, int& nDataLen);
	//pDataBlockΪ���������dataLenΪ��������
	static FaceInfoGroup DataBlockToObject(char* pDataBlock, int& dataLen);
};

//
class FaceRecognizeResult
{
public:
	string gstGUID[2];
	bool bIsSameFace;
	int nConfidence;

	//resultΪ���������pDataBlock��nDataLenΪ��������
	static void ObjectToDataBlock(const FaceRecognizeResult& result, char*& pDataBlock, int& nDataLen);
	//pDataBlockΪ���������dataLenΪ��������
	static FaceRecognizeResult DataBlockToObject(char* pDataBlock, int& dataLen);
}; 

// ����GUID�ú��� 
string GenerateGuid();

#endif //_COMMON_H_