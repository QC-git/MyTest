
#include "stdafx.h"

#ifdef ENABLE_VLD_DEBUG
//#include "vld.h"
#endif//ENABLE_VLD_DEBUG
//#include "DBWindow.h"
#include "SendingQueue.h"
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
// CNoteInfo
CNodeInfo::CNodeInfo()
{
	pData = NULL;
	nDataLen = 0;
}

CNodeInfo::~CNodeInfo()
{
	if (pData != NULL)
	{
		delete [] pData;
		pData = NULL;
	}
}

// CSendingQueue
CSendingQueue::CSendingQueue()
{
	m_nGap = 0;
	m_nNodeCount = 0;
	m_pLinkHeader = NULL;
	m_pLinkTail = NULL;
	m_pLinkBegin = NULL;
	m_pLinkEnd = NULL;
}

CSendingQueue::~CSendingQueue()
{
	Release();
}

int CSendingQueue::Initialize(int nQLen, int nMaxNodeSize)
{
	int iNode = 0;
	CNodeInfo *pNoteInfo = NULL;
	CNodeInfo *pCurNoteInfo = NULL;
	if (nQLen <= 0) return 1;	
	
	// Header
	pNoteInfo = new CNodeInfo;
	if (pNoteInfo != NULL)
	{
		pNoteInfo->pData = new char[nMaxNodeSize];
		if (pNoteInfo->pData != NULL)
		{
			memset(pNoteInfo->pData, 0, nMaxNodeSize);
			pNoteInfo->nDataLen = 0;
		}
		m_pLinkHeader = pNoteInfo;
		m_pLinkTail = pNoteInfo;
		m_pLinkBegin = pNoteInfo;
		m_pLinkEnd = pNoteInfo;
		pCurNoteInfo = pNoteInfo;
	}

	// Next
	for (iNode = 1; iNode < nQLen; iNode++)
	{
		pNoteInfo = new CNodeInfo;
		if (pNoteInfo != NULL)
		{			
			pNoteInfo->pData = new char[nMaxNodeSize];
			if (pNoteInfo->pData != NULL)
			{
				memset(pNoteInfo->pData, 0, nMaxNodeSize);
				pNoteInfo->nDataLen = 0;
			}
			pCurNoteInfo->pNext = pNoteInfo;
			pCurNoteInfo = pNoteInfo;
		}
	}

	// End
	m_pLinkTail = pCurNoteInfo;
	m_pLinkTail->pNext = m_pLinkHeader;	
	m_nNodeCount = nQLen;
	m_nNodeSize = nMaxNodeSize;
	m_nGap = 0;
	return 0;
}

int CSendingQueue::Release()
{
	CNodeInfo *ptCurNode = NULL;
	CNodeInfo *ptNextNode = NULL;

	ptCurNode  = m_pLinkHeader;
	while ((ptCurNode != NULL) && (ptCurNode != m_pLinkTail))
	{
		ptNextNode = ptCurNode->pNext;
		delete ptCurNode;
		ptCurNode = ptNextNode;		
	}
	// Release End Node.
	if (m_pLinkTail != NULL)
	{
		delete m_pLinkTail;
		m_pLinkTail = NULL;
	}

	m_pLinkHeader = NULL;

	return 0;
}

int CSendingQueue::ReadyForNode(char **ppNodeBuf, int *pnNodeLen)
{
	(*pnNodeLen) = m_nNodeSize;
	(*ppNodeBuf) = new char[m_nNodeSize];
	if ((*ppNodeBuf) != NULL)
	{
		memset((*ppNodeBuf), 0, m_nNodeSize);
		return 0;
	}
	// Fail
	return 1;
}

int CSendingQueue::ReleaseTheNode(char *pNodeBuf)
{
	if (pNodeBuf != NULL)
	{
		delete [] pNodeBuf;
		pNodeBuf = NULL;
		return 0;
	}
	return 1;
}

int CSendingQueue::InsertNode(char* pNodeBuf, int nNodeLen)
{
	// Check
	if (pNodeBuf == NULL)  return 1;
	if (nNodeLen > m_nNodeSize) return 1;
	if (m_pLinkHeader == NULL) return 1;
		
	m_nGap ++;
	// Find		
	//队列满了，丢掉排在最前面的
	if ((m_pLinkEnd == m_pLinkBegin) && (m_nGap != 1) ) 
	{// The circle link is full, need to cover the first item.	
		m_pLinkBegin = m_pLinkBegin->pNext;
		m_nGap--;
	}
	// Clean And Insert
	memset(m_pLinkEnd->pData, 0, m_nNodeSize);	
	memcpy(m_pLinkEnd->pData, pNodeBuf, nNodeLen);
	m_pLinkEnd->nDataLen = nNodeLen;
	m_pLinkEnd = m_pLinkEnd->pNext;
	//
#if CSENDING_QUEUE_DEBUG
	//DBGWrite(LOG_LEVEL_NORMAL, "InsertNote: m_nGap %d, NoteCount %d\n", m_nGap, m_nNodeCount);
	//printf("insert: %s\n------------------\n", pNodeBuf);
	//DisplayLink();
#endif
	//
	return 0;
}

int CSendingQueue::TakeNode(char* pNodeBuf, int *pnNodeLen)
{
	if (pNodeBuf == NULL) return 1;
	if (pnNodeLen == NULL) return 1;
	//	
	if (m_nGap <= 0)
	{// Circle link is empty.
		m_nGap = 0;
		return 2;
	}
	if ((m_pLinkBegin->nDataLen > 0) && (m_pLinkBegin->pData != NULL))
	{
		(*pnNodeLen) = m_pLinkBegin->nDataLen;
		memcpy(pNodeBuf, m_pLinkBegin->pData, m_pLinkBegin->nDataLen);
		m_pLinkBegin = m_pLinkBegin->pNext;
		m_nGap --;
	}
	//
#if CSENDING_QUEUE_DEBUG
	DBGWrite(LOG_LEVEL_NORMAL, "TakeNote: m_nGap %d, NoteCount %d\n", m_nGap, m_nNodeCount);
	//printf("TakeNode: %s\n------------------\n", pNodeBuf);
	//DisplayLink();
#endif
	return 0;
}

int CSendingQueue::EmptyAllNodes()
{
	CNodeInfo *pCurNode = NULL;	
	pCurNode = m_pLinkHeader;
	while(pCurNode != m_pLinkTail)
	{
		pCurNode->nDataLen = 0;
		pCurNode = pCurNode->pNext;
	}
	if (m_pLinkTail != NULL)
		m_pLinkTail->nDataLen = 0;
	//
	m_pLinkBegin = m_pLinkHeader;
	m_pLinkEnd = m_pLinkHeader;
	m_nGap = 0;
	//
	return 0;
}

#if CSENDING_QUEUE_DEBUG
int CSendingQueue::DisplayLink()
{
	if (m_nGap > 0)
	{
		int iNode = 0;
		CNodeInfo *pCurNode = NULL;
		pCurNode = m_pLinkBegin;		
		//while(pCurNode != m_pLinkEnd)
		do
		{
			iNode++;
			printf("%d ", iNode);
			if (pCurNode->pData != NULL)
			{
				printf("%s\n", pCurNode->pData);
			}
			pCurNode = pCurNode->pNext;
		}while(pCurNode != m_pLinkEnd);

	//	iNode++;
	//	printf("%d ", iNode);
	//	printf("%s\n", pCurNode->pData);
		printf("\n\n");
	}
	return 0;
}

int SendingQueue_TEST(int iOption)
{
	char *pBufGet = NULL;
	int nBufGet = 0, nTempBuf = 0;
	switch(iOption)
	{
	case 0:
		{	
			CSendingQueue csq;
			// 3 Nodes
			csq.Initialize(3, 100);
			csq.ReadyForNode(&pBufGet, &nBufGet);
			csq.InsertNode("1 Hello Node", strlen("1 Hello Node"));
			csq.InsertNode("2 Hello Node", strlen("1 Hello Node"));
			csq.InsertNode("3 Hello Node", strlen("1 Hello Node"));
			csq.InsertNode("4 Hello Node", strlen("1 Hello Node"));
			csq.InsertNode("5 Hello Node", strlen("1 Hello Node"));
			csq.TakeNode(pBufGet, &nTempBuf);
			csq.TakeNode(pBufGet, &nTempBuf);
			csq.TakeNode(pBufGet, &nTempBuf);
			csq.TakeNode(pBufGet, &nTempBuf);
			csq.ReleaseTheNode(pBufGet);
		}
		break;

	case 1:
		// 2 Nodes
		{	
			CSendingQueue csq;
			// 3 Nodes
			csq.Initialize(2, 100);
			csq.ReadyForNode(&pBufGet, &nBufGet);
			csq.InsertNode("1 Hello Node", strlen("1 Hello Node"));
			csq.InsertNode("2 Hello Node", strlen("1 Hello Node"));
			csq.InsertNode("3 Hello Node", strlen("1 Hello Node"));
			csq.InsertNode("4 Hello Node", strlen("1 Hello Node"));
			csq.InsertNode("5 Hello Node", strlen("1 Hello Node"));
			csq.TakeNode(pBufGet, &nTempBuf);
			csq.TakeNode(pBufGet, &nTempBuf);
			csq.TakeNode(pBufGet, &nTempBuf);
			csq.TakeNode(pBufGet, &nTempBuf);
			csq.ReleaseTheNode(pBufGet);
		}
		break;

	case 2:
		// 1 Nodes
		{	
			CSendingQueue csq;
			// 3 Nodes
			csq.Initialize(1, 100);
			csq.ReadyForNode(&pBufGet, &nBufGet);
			csq.InsertNode("1 Hello Node", strlen("1 Hello Node"));
			csq.InsertNode("2 Hello Node", strlen("1 Hello Node"));
			csq.InsertNode("3 Hello Node", strlen("1 Hello Node"));
			csq.InsertNode("4 Hello Node", strlen("1 Hello Node"));
			csq.InsertNode("5 Hello Node", strlen("1 Hello Node"));
			csq.TakeNode(pBufGet, &nTempBuf);
			csq.TakeNode(pBufGet, &nTempBuf);
			csq.TakeNode(pBufGet, &nTempBuf);
			csq.TakeNode(pBufGet, &nTempBuf);
			csq.ReleaseTheNode(pBufGet);
		}
		break;

	case 3:
		// 0 Nodes
		{	
			CSendingQueue csq;
			// 3 Nodes
			if ( 0 == csq.Initialize(0, 100))
			{
				csq.ReadyForNode(&pBufGet, &nBufGet);
				csq.InsertNode("1 Hello Node", strlen("1 Hello Node"));
				csq.InsertNode("2 Hello Node", strlen("1 Hello Node"));
				csq.InsertNode("3 Hello Node", strlen("1 Hello Node"));
				csq.InsertNode("4 Hello Node", strlen("1 Hello Node"));
				csq.InsertNode("5 Hello Node", strlen("1 Hello Node"));
				csq.TakeNode(pBufGet, &nTempBuf);
				csq.TakeNode(pBufGet, &nTempBuf);
				csq.TakeNode(pBufGet, &nTempBuf);
				csq.TakeNode(pBufGet, &nTempBuf);
				csq.ReleaseTheNode(pBufGet);
			}
		}
		break;
	}

	return 0;
}

#endif