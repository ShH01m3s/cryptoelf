#include <stdio.h>
#include "stdafx.h"
#include <iostream>
#include <iomanip>
#include <string>
#include <math.h>
#include <iostream>
#include <vector>


using namespace std;

double GetDistributionCoefficient(vector<int> text);
void Crack(vector<int> cipher, vector<int> plainText, vector<int> cipherFunArray);
bool CheckKey(vector<int> stream, int key, double commonDistrib);
double GetDistributionCoefficientForCipher(vector<int> text);

double  normalDistributions[256];

int main() {
	unsigned char ch;
	uint32_t chr, chr2;

	vector<int> possibleKeyLengths;

	FILE* commonFile = fopen("D:\\pen\\coursera\\projects\\cryptography\\assignment 1\\common.txt", "r");
	vector<int> commonFileArray;
	while (fscanf(commonFile, "%c", &ch) != EOF) commonFileArray.push_back(int(ch));

	FILE* ciphered_file = fopen("D:\\pen\\coursera\\projects\\cryptography\\assignment 1\\ciphered.txt", "r");
	vector<int> cipherArray;
	while (fscanf(ciphered_file, "%02x", &chr) != EOF) cipherArray.push_back(chr);

	FILE* ciphered_file2 = fopen("D:\\pen\\coursera\\projects\\cryptography\\assignment 1\\fun.txt", "r");
	vector<int> cipherFunArray;
	while (fscanf(ciphered_file2, "%02x", &chr2) != EOF) cipherFunArray.push_back(chr2);

	Crack(cipherArray, commonFileArray, cipherFunArray);

	return 0;
}

double GetDistributionCoefficient(vector<int> text)
{
	double  distributionCoefficient = 0;
	double  characterArray[256];

	for (int i = 0; i < 256; i++) normalDistributions[i] = 0;
	for (int i = 0; i < 256; i++) characterArray[i] = 0;

	for (int character = 0; character < text.size(); character++) {
		if (characterArray[text.at(character)] == 0) characterArray[text.at(character)] = 1;
		else characterArray[text.at(character)] += 1;
	}

	for (int i = 0; i <= 255; i++) {
		//By the way, there are some ASCII characters that are not present in a normal English text
		//How must they be handled in this function?
		if (characterArray[i] == 0) continue;
		characterArray[i] = characterArray[i] / text.size();
		normalDistributions[i] = characterArray[i];
		characterArray[i] = characterArray[i] * characterArray[i];
		distributionCoefficient += characterArray[i];
	}
	return distributionCoefficient;
}

double GetDistributionCoefficientForCipher(vector<int> text)
{
	double  distributionCoefficient = 0;
	double  characterArray[256];

	for (int i = 0; i < 256; i++) characterArray[i] = 0;

	for (int character = 0; character < text.size(); character++) {
		if (characterArray[text.at(character)] == 0) characterArray[text.at(character)] = 1;
		else characterArray[text.at(character)] += 1;
	}

	for (int i = 0; i <= 255; i++) {
		//By the way, there are some ASCII characters that are not present in a normal English text
		//How must they be handled in this function?
		if (characterArray[i] == 0) continue;
		characterArray[i] = characterArray[i] / text.size();
		characterArray[i] = normalDistributions[i] * characterArray[i];
		distributionCoefficient += characterArray[i];
	}
	return distributionCoefficient;
}

void Crack(vector<int> cipher, vector<int> plainText, vector<int> cipherFunArray)
{
	double commonDisctibution = GetDistributionCoefficient(plainText);
	//for each character in plain text...

	for (int keyLength = 2; keyLength < cipher.size() / 2; keyLength++)
	{
		vector<vector<int>> cipherStreams;
		cipherStreams.resize(keyLength);
		vector<int> key;

		//sort characters of the cipher text between all streams
		for (int charIndex = 0; charIndex < cipher.size(); charIndex++) cipherStreams.at(charIndex % keyLength).push_back(cipher.at(charIndex));

		//check each stream for validity for each key
		for (int streamNumber = 0; streamNumber < cipherStreams.size(); streamNumber++)
		{
			for (size_t k = 1; k < 256; k++)
			{
				//if key fits, then save it
				bool check = CheckKey(cipherStreams.at(streamNumber), k, commonDisctibution);
				if (!check) continue;
				key.push_back(k);				
				break;
			}
		}
		if (key.size() == keyLength)
		{
			for (size_t i = 0; i < cipher.size(); i++) cout << (char)(cipher.at(i) ^ key.at(i % keyLength));
		}
		if (key.size() == keyLength)
		{
			cout << endl;
			for (size_t i = 0; i < cipherFunArray.size(); i++) cout << (char)(cipherFunArray.at(i) ^ key.at(i % keyLength));
		}

	}
	return;
}

bool CheckKey(vector<int> stream, int key, double commonDistrib)
{
	vector<int> streamDeciphered;
	for (size_t ch = 0; ch < stream.size(); ch++)
	{
		int decipheredChar = stream.at(ch) ^ key;
		
		if ((decipheredChar < 32) || (decipheredChar > 122)) return false;
		if((decipheredChar >= 48) && (decipheredChar <= 57)) return false;
		if ((decipheredChar >= 91) && (decipheredChar <= 96)) return false;
		//if ((decipheredChar >= 35) && (decipheredChar <= 43)) return false;

		streamDeciphered.push_back(decipheredChar);
	}
	double distr = GetDistributionCoefficientForCipher(streamDeciphered);
	double diff = abs(distr - commonDistrib);
	if (diff >= 0.02) return false;
	return true;
}