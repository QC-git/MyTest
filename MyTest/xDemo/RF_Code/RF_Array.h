#pragma once

/*
	func: 卡数较少，暂采用单维map，承受数量10*1000；
	       考虑到效率，数量应尽量不大于1000，当大于1000时应采用二维map或者 hash-map
*/

template <typename RF_MAP_DataType_t>
class CRF_Map_M  //非线程 安全
{
public:
	CRF_Map_M()
	{
		m_listNum = MAP_SIZE_DEFAULT;

		m_list = new t_RF_Map_s[m_listNum];
		memset(m_list, 0, sizeof(t_RF_Map_s)*m_listNum);
	}

	CRF_Map_M(int num)
	{
		m_listNum = num;

		m_list = new t_RF_Map_s[m_listNum];
		memset(m_list, 0, sizeof(t_RF_Map_s)*m_listNum);
	}

	~CRF_Map_M()
	{
		Free();
	}

	int Save(int key, RF_MAP_DataType_t& data)
	{
		if ( 0 == key || NULL == m_list )
		{
			return -1;
		}

		int nRoom = GetNewRoom();
		if ( nRoom <= 0 )
		{
			return -2;
		}

		m_list[nRoom-1].key = key;
		m_list[nRoom-1].data = data;

		return 0;
	}

	int Read(int key, RF_MAP_DataType_t* pData_out)
	{
		if ( 0 == key || NULL == m_list || NULL == pData_out )
		{
			return -1;
		}

		int nRoom = GetOldRoom(key);
		if ( nRoom <= 0 )
		{
			return -2;
		}

		(*pData_out) = m_list[nRoom-1].data;

		return 0;
	}

	//int Remove(int id);

	bool Check(int key)  // if 存在  return true
	{
		return ( GetOldRoom(key) > 0 ) ? true : false ;
	}

	int Clear()
	{
		if ( NULL == m_list )
		{
			return -1;
		}

		for (int n=0; n<m_listNum; n++)
		{
			m_list[n].key = 0;
		}

		return 0;
	}

	int Size()
	{
		if ( NULL == m_list )
		{
			return -1;
		}

		int count = 0;
		for (int n=0; n<m_listNum; n++)
		{
			if ( 0 != m_list[n].key )
			{
				count++;
			}
		}

		return count;
	}

	int MaxSize()
	{
		return m_listNum;
	}

	RF_MAP_DataType_t operator[](int n)
	{
		return m_list[n].data;
	}

private:
	static const int MAP_SIZE_DEFAULT = 100;

	struct t_RF_Map_s
	{
		int key;  //id 为关键字  不可重复
		RF_MAP_DataType_t data;
	};
	t_RF_Map_s* m_list;

	int m_listNum;

	int GetOldRoom(int key)  // return > 0
	{
		if ( NULL == m_list || 0 == key )
		{
			return 0;
		}

		for (int n=0; n<m_listNum; n++)
		{
			if ( key == m_list[n].key )
			{
				return n+1;
			}
		}

		return 0;
	}

	int GetNewRoom()        // return > 0
	{
		if ( NULL == m_list )
		{
			return 0;
		}

		for (int n=0; n<m_listNum; n++)
		{
			if ( 0 == m_list[n].key )
			{
				return n+1;
			}
		}

		return 0;
	}

	void Free()
	{
		if (m_list)
		{
			delete[] m_list;
		}

		m_list = NULL;
	}

};
