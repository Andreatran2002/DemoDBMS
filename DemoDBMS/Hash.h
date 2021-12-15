#pragma once
#include <iostream>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <conio.h>
#include <Windows.h>
#include <cstdlib>
#include <cstring>
#include <list>
#include <sstream>

const int T_S = 95;
using namespace std;

//===struct of hash table entry=====
struct HashTableEntry {
	string v, k;
	HashTableEntry* n;
	HashTableEntry* p;
	HashTableEntry(string k, string v) {
		this->k = k;
		this->v = v;
		this->n = NULL;
	}
};

//HTE-HTE-NULL
//HTE-HTE-HTE-NULL
//HTE-HTE-HTE-HTE-NULL
//HTE-HTE-HTE-HTE-HTE-NULL
//HTE-HTE-HTE-NULL
//HTE-HTE-NULL
//HTE-NULL
// 
// 
//for i (0 -> T_S)
//	HashTableEntry* en = ht[i]
//		while(en!+null) {
//			en->k
//			en->v
//			en = en->n
//		}

class HashMapTable {
public:
	const int PRIME_CONT = 13;
	HashTableEntry** ht;
	HashMapTable() {
		ht = new HashTableEntry * [T_S];
		for (int i = 0; i < T_S; i++)
			ht[i] = NULL;
	}
	//key -> value hashed
	int HashFunc(string key) {
		size_t sum = 0;
		for (int i = 0; i < key.length(); i++) {
			sum += (key[i] * (int)pow(PRIME_CONT, i)) % T_S;
		}
		return sum % T_S;
	}

	void Insert(string k, string v) {
		int hash_v = HashFunc(k);
		HashTableEntry* p = NULL;
		HashTableEntry* en = ht[hash_v];
		while (en != NULL) {
			p = en;
			en = en->n;
		}
		if (en == NULL) {
			en = new HashTableEntry(k, v);
			if (p == NULL) {
				cout << "[INSERT] [SUCCESS] insert \"" << k << "\": \"" << v << "\". index: " << hash_v % T_S << "." << endl;
				ht[hash_v] = en;
			}
			else {
				cout << "[INSERT] [NEXT>>]." << endl;
				cout << "[INSERT] [SUCCESS] insert \"" << k << "\": \"" << v << "\". index: " << hash_v % T_S << "." << endl;
				p->n = en;
			}
		}
		else {
			en->v = v;
		}
	}

	void Update(string k, string v) {
		int hash_v = HashFunc(k);
		bool flag = false;
		HashTableEntry* en = ht[hash_v];
		if (en != NULL) {
			while (en != NULL) {
				if (en->k == k) {
					en->v = v;
					flag = true;
				}
				if (flag) {
					cout << "[UPDATE] [SUCCESS] update \"" << k << "\": \"" << v << "\"" << endl;
				}
				en = en->n;
			}
		}
		if (!flag) {
			cout << "[UPDATE] [FAIL] Update fail \"" << k << "\". " << endl;
		}
	}

	void Remove(string k) {
		int hash_v = HashFunc(k);
		HashTableEntry* en = ht[hash_v];
		HashTableEntry* p = NULL;
		if (en == NULL || en->k != k) {
			cout << "[REMOVE] [FAIL] No Element found at key \"" << k << "\"" << endl;
			return;
		}
		while (en->n != NULL) {
			p = en;
			en = en->n;
		}
		if (p != NULL) {
			p->n = en->n;
		}
		delete en;
		cout << "[REMOVE] [SUCCESS] Element Deleted" << endl;
	}

	string SearchKey(string k) {
		int hash_v = HashFunc(k);
		bool flag = false;
		HashTableEntry* en = ht[hash_v];
		if (en != NULL) {
			while (en != NULL) {
				if (en->k == k) {
					flag = true;
				}
				if (flag) {
					cout << "[SEARCH] [SUCCESS] Element found at key \"" << k << "\": \"" << en->v << "\"" << endl;
					return en->v;
				}
				en = en->n;
			}
		}
		if (!flag) {
			cout << "[SEARCH] [FAIL] No Element found at key \"" << k << "\"" << endl;
			return "[empty]";
		}
	}
	~HashMapTable() {
		delete[] ht;
	}
};
