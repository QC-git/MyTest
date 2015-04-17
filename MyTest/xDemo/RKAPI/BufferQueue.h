
// SendingQueue is a class for store video detect results.
// The other translate thread will get buffer form here and send the results to clients. 
// lutf@20110812

#ifndef SENDING_QUEUE_H
#define SENDING_QUEUE_H

#define CSENDING_QUEUE_DEBUG 0


class CNodeBufInfo
{
public:
	CNodeBufInfo();
	~CNodeBufInfo();
public:
	char *pData;
	int nDataLen;
	CNodeBufInfo *pNext;
};


class CBufferQueue
{	
public:
	CBufferQueue();
	~CBufferQueue();
	// int nQLen: the queue's node count.
	// int nMaxNodeSize: the max size of a node would be. All insert the queue can't over the max node size.
	// for the efficiency, all memory will be readied at beginning.
	// Return : success return 0, others mean fail.
	int Initialize(int nQLen, int nMaxNodeSize);	
	int Release();
	// Prepare buffer for take node.
	// char **ppNodeBuf: function allocate memory, need free by yourself.
	// int *pnNodeLen: the allocate memory size.
	// Return : success return 0, others mean fail.
	int ReadyForNode(char **ppNodeBuf, int *pnNodeLen);
	// To release the buffer of pNodeBuf.
	// Corresponding to ReadyForNode. The buffer get form ReadyForNode, must be call this function to release it.
	int ReleaseTheNode(char *pNodeBuf);
	// Insert and get node data.
	// Return : success return 0, others mean fail.
	int InsertNode(char* pNodeBuf, int nNodeLen);	
	// char* pNodeBuf : the buffer pointer must return by ReadyForNode.
	// int *pnNodeLen: the buffer length.
	// Return : success return 0, others mean fail.
	int TakeNode(char* pNodeBuf, int *pnNodeLen);
	// Empty all nodes.
	int EmptyAllNodes();
	// Do debug
#if CSENDING_QUEUE_DEBUG
	int DisplayLink();
#endif

private:	
	CRITICAL_SECTION m_CriticalSectionLinkBegin; // For m_pLinkBegin and m_nGap	
	// To record link list's first item and last item. These value would not changed after the link list be created.
	CNodeBufInfo *m_pLinkHeader; 
	CNodeBufInfo *m_pLinkTail;
	// Always point to the useful node link's beginning and ending. These values changed with Add and Take nodes.
	CNodeBufInfo *m_pLinkBegin;
	CNodeBufInfo *m_pLinkEnd;	
	// How many nodes between m_pLinkBegin and m_pLinkEnd
	int m_nNodeCount;
	int m_nNodeSize;
	int m_nGap;
};

#if CSENDING_QUEUE_DEBUG
int BufferQueue_TEST(int iOption);
#endif

#endif//SENDING_QUEUE_H