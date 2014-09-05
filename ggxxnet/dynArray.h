#pragma once

//******************************************************************
// include
//******************************************************************
#include <assert.h>

//******************************************************************
// class
//******************************************************************
template <class T> class TDynArray
{
public:
	TDynArray(int p_cap = 10, int p_ext = 10)
	{
		m_size = 0;
		m_capacity = p_cap;
		m_extend = p_ext;
		m_data = new T[p_cap];
	}
	
	TDynArray(const TDynArray &p_org)
	{
		m_extend = p_org.m_extend;
		m_capacity = p_org.m_capacity;
		m_size = p_org.m_size;
		m_data = new T[m_capacity];
		memcpy(m_data, p_org.m_data, sizeof(T) * m_size);
	}
	
	~TDynArray(void)
	{
		delete[] m_data;
	}
	
	T& operator[](int p_idx)
	{
		assert(p_idx >= 0 && p_idx < m_size);
		return m_data[p_idx];
	}

	void operator=(const TDynArray p_org)
	{
		delete[] m_data;

		m_extend = p_org.m_extend;
		m_capacity = p_org.m_capacity;
		m_size = p_org.m_size;
		m_data = new T[m_capacity];
		memcpy(m_data, p_org.m_data, sizeof(T) * m_size);
	}

	T& get(int p_idx)
	{
		assert(p_idx >= 0 && p_idx < m_size);
		return m_data[p_idx];
	}

	void add(T p_data)
	{
		if (m_size >= m_capacity) resize(m_capacity + m_extend);
		m_data[m_size] = p_data;
		m_size++;
	}
	
	void set(int p_idx, T p_data)
	{
		if (p_idx >= m_size) m_size = p_idx + 1;
		if (m_size >= m_capacity) resize(m_size);
		m_data[p_idx] = p_data;
	}

	void insert(int p_idx, T p_data)
	{
		if (p_idx == -1)
		{
			add(p_data);
		}
		else
		{
			if (m_size >= m_capacity) resize(m_capacity + m_extend);
			for (int i = m_size; i > p_idx; i--) m_data[i] = m_data[i - 1];
			m_data[p_idx] = p_data;
			m_size++;
		}
	}

	void remove(int p_idx)
	{
		assert(p_idx >= 0 && p_idx < m_size);
		for (int i = p_idx + 1; i < m_size; i++) m_data[i - 1] = m_data[i];
		m_size--;
	}
	
	inline void removeAll(void) { m_size = 0; }
	
	int find(T p_data)
	{
		for (int i = 0; i < m_size; i++)
		{
			if (p_data == m_data[i]) return i;
		}
		return -1;
	}

	inline T* data(void) { return m_data; }

	inline int size(void) { return m_size; }

	void resize(int p_size)
	{
		if (p_size <= m_capacity) return;
		
		T* newdata = new T[p_size];
		for (int i = 0; i < m_capacity; i++) newdata[i] = m_data[i];
		
		delete[] m_data;

		m_data = newdata;
		m_capacity = p_size;
	}
	
	void deleteAll(void)
	{
		for (int i = 0; i < m_size; i++)
		{
			if (m_data[i])
			{
				delete m_data[i];
				m_data[i] = NULL;
			}
		}
		m_size = 0;
	}

private:
	T*	m_data;
	int	m_size;
	int	m_capacity;
	int m_extend;
};
