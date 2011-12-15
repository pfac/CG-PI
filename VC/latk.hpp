#ifndef ___LATK_H___
#define ___LATK_H___

template<T>
class vector<T> {
	T *data;
public:
	vector (int size) { data = new T[size]; }
	~vector () { delete data; }

	vector<T>& operator= (const vector<T>& v) {

}

#endif/*___LATK_H___*/