#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "b64.h"
#include <memory>
#include <string>

#include <iostream>
#include <thread>
#include <mutex>
#include <atomic>
#include <vector>
#include <cstring>
#include <cstdint>
#define NOMINMAX
#include<Windows.h>
#include <assert.h>
#include <time.h>
#include <algorithm>
#include<stdexcept>

//int
//main(void) {
//	const char* str = "brian the monkey and bradley the kinkajou are friends";
//	char* enc = b64_encode((unsigned char*)str, strlen(str));
//
//	printf("%s\n", enc); // YnJpYW4gdGhlIG1vbmtleSBhbmQgYnJhZGxleSB0aGUga2lua2Fqb3UgYXJlIGZyaWVuZHM=
//
//	char* dec = (char*)b64_decode((const char*)enc, strlen(enc));
//
//	printf("%s\n", dec); // brian the monkey and bradley the kinkajou are friends
//	free(enc);
//	free(dec);
//	return 0;
//}

//1. Decode the first base64 and the result is UTF - 8 binary(2 bytes = 1 character)
//2. Convert UTF - 8 binary to UTF - 16 string
//3. Take the UTF - 16 string and decrypt it with the XOR function
//4. Decode the UTF - 16 string as base64 and you get PE file.




__declspec(noinline) char* ReadInputFile() {
	FILE* hamadFile = fopen("C:\\Users\\DFIR\\Documents\\samples\\mShell\\brutforce-shell\\x64\\Release\\input.txt", "rb");
	//FILE* hamadFile = fopen("C:\\Users\\DFIR\\Documents\\samples\\mShell\\brutforce-shell\\x64\\Release\\input2.txt", "rb");
	if (hamadFile) {
		fseek(hamadFile, 0, SEEK_END);
		long fileSize = ftell(hamadFile);
		fseek(hamadFile, 0, SEEK_SET);
		char* data = (char*)malloc(fileSize);
		if (!data)
			return nullptr;
		size_t bytesRead = fread(data, 1, fileSize, hamadFile);
		if (bytesRead != fileSize) {
			printf("fread: failed to read file | bytesRead = %u | fileSize = %u\n", bytesRead, fileSize);
			return nullptr;
		}
		fclose(hamadFile);
		return data;
	}
	printf("failed to open file\n");
	return nullptr;
}

__declspec(noinline) std::wstring utf8_to_utf16(const std::string& utf8)
{
	std::vector<unsigned long> unicode;
	size_t i = 0;
	while (i < utf8.size())
	{
		unsigned long uni;
		size_t todo;
		bool error = false;
		unsigned char ch = utf8[i++];
		if (ch <= 0x7F)
		{
			uni = ch;
			todo = 0;
		}
		else if (ch <= 0xBF)
		{
			throw std::logic_error("not a UTF-8 string");
		}
		else if (ch <= 0xDF)
		{
			uni = ch & 0x1F;
			todo = 1;
		}
		else if (ch <= 0xEF)
		{
			uni = ch & 0x0F;
			todo = 2;
		}
		else if (ch <= 0xF7)
		{
			uni = ch & 0x07;
			todo = 3;
		}
		else
		{
			throw std::logic_error("not a UTF-8 string");
		}
		for (size_t j = 0; j < todo; ++j)
		{
			if (i == utf8.size())
				throw std::logic_error("not a UTF-8 string");
			unsigned char ch = utf8[i++];
			if (ch < 0x80 || ch > 0xBF)
				throw std::logic_error("not a UTF-8 string");
			uni <<= 6;
			uni += ch & 0x3F;
		}
		if (uni >= 0xD800 && uni <= 0xDFFF)
			throw std::logic_error("not a UTF-8 string");
		if (uni > 0x10FFFF)
			throw std::logic_error("not a UTF-8 string");
		unicode.push_back(uni);
	}
	std::wstring utf16;
	for (size_t i = 0; i < unicode.size(); ++i)
	{
		unsigned long uni = unicode[i];
		if (uni <= 0xFFFF)
		{
			utf16 += (wchar_t)uni;
		}
		else
		{
			uni -= 0x10000;
			utf16 += (wchar_t)((uni >> 10) + 0xD800);
			utf16 += (wchar_t)((uni & 0x3FF) + 0xDC00);
		}
	}
	return utf16;
}

__declspec(noinline) void ConvertUTF8ToUTF16(uint8_t* bytes, size_t size) {
	std::string str;
	str.resize(size);
	for (int i = 0; i < size; i++) {
		str[i] = bytes[i];
	}
	std::wstring utf16String = utf8_to_utf16(str);
	uint8_t* data = (uint8_t*)utf16String.data();
	for (int i = 0; i < size; i++) {
		bytes[i] = data[i];
	}
	//printf("Converted to UTF16 bytes = ");
	//PrintBytesArray(bytes, 32);
}

//string PRVd0cjlOk(string pText, int pEncKey)
//{
//	StringBuilder vfT71MQCiJ = new StringBuilder(pText);
//	StringBuilder vyvYADwdYw = new StringBuilder(pText.Length);
//	char c2TxEJ0uqP;
//	for (int i = 0; i < pText.Length; i++)
//	{
//		c2TxEJ0uqP = vfT71MQCiJ[i];
//		c2TxEJ0uqP = (char)(c2TxEJ0uqP ^ pEncKey);
//		vyvYADwdYw.Append(c2TxEJ0uqP);
//	}
//	return vyvYADwdYw.ToString();
//}

char*  XOR_func(uint8_t* utf16String, int utf16String_size, int key, size_t* resultSize ) {
	
	*resultSize = utf16String_size / 2;
	char* asciiString = (char*) malloc(utf16String_size / 2);
	int asciiStringIndex = 0;
	for (int i = 0; i < utf16String_size; i += 2) {
		uint8_t firstByteOfChar = utf16String[i];
		uint8_t secondByteOfChar = utf16String[i+1]; // ignored

		uint8_t xorResult = (uint8_t)(firstByteOfChar ^ key);
		asciiString[asciiStringIndex] = xorResult;
		asciiStringIndex += 1;

	}
	return asciiString;
	//uint16_t* utf16String
	// 
	//int length = utf16String_size / 2;
	//for (int i; i < utf16String_size; i++) {
	//	uint8_t firstByteOfChar = (uint8_t)utf16String[i];

	//	uint8_t xorResult = (uint8_t)(firstByteOfChar ^ key);
	//	utf16String[i] = xorResult;

	//}
}

std::mutex cout_mutex;
const uint64_t MAX_KEY = 0xFFFFFFFF;
std::atomic<uint64_t> current_key(0);

clock_t oldTime = clock();
void searchForKey(uint8_t* hamadUTF16EncryptedOriginal, size_t hamadUTF16EncryptedSize, uint64_t* found_key, int threadId) {
	char* hamadUTF16Encrypted = (char*)malloc(hamadUTF16EncryptedSize);
	if (!hamadUTF16Encrypted)
		return;
	memcpy(hamadUTF16Encrypted, hamadUTF16EncryptedOriginal, hamadUTF16EncryptedSize);
	while (true) {
		uint64_t key = current_key.fetch_add(1);
		if (key > MAX_KEY) break;
		uint32_t theKey = key;
		if (clock() - oldTime > 20000) // 20 seconds
		{
			oldTime = clock();
			printf("\n\nthread %d: key = (%#.8x) %u\n", threadId, theKey, theKey);
		}

		size_t resultSize = 0;
		char* asciiBase64String = XOR_func((uint8_t*)hamadUTF16Encrypted, hamadUTF16EncryptedSize, theKey, &resultSize);

		size_t peFileSize = 0;
		uint8_t* peFile = (uint8_t*)b64_decode_ex((const char*)asciiBase64String, resultSize, &peFileSize);
	
		if (resultSize <= 10)
			continue;
		if (peFile[0] == 0x4D && peFile[1] == 0x5A) {
			printf("\n\nFOUNDKEY bytes IN thread %d: key = (%#.8x) %u \n", threadId, theKey, theKey);
			printf("Found bytes: %hhx %hhx = %c%c\n", peFile[0], peFile[1], peFile[0], peFile[1]); // 4d 5a = MZ
			printf("asciiBase64String = %s\n", asciiBase64String);
			*found_key = theKey;
			break;
		}

	}
}


int main() {
	char* firstBase64String = ReadInputFile();
	if (!firstBase64String)
		return 0;
	size_t decodedSize = 0;//stack 
	// lea r8, [ebp-0x10]
	char* hamad_utf8= (char*)b64_decode_ex((const char*)firstBase64String, strlen(firstBase64String), &decodedSize);// decode b64 to binary utf-8
	/*mov rcx, [ebp - 8];
	call strlen;
	mov rdx, rax;
	lea r8, [ebp - 0x10];
	call b64_decode_ex;
	mov rsi, [ebp - 0x10]*/


	uint8_t* bytes = (uint8_t * )hamad_utf8;
	printf("%x\n", bytes[0]);
	ConvertUTF8ToUTF16((uint8_t*)hamad_utf8, decodedSize);
	char* hamadUTF16Encrypted = (char*)hamad_utf8;

	int num_threads =  std::thread::hardware_concurrency();
	std::vector<std::thread> threads;
	std::vector<uint64_t> found_keys(num_threads, MAX_KEY);
	printf("num_threads = %d\n", num_threads);


	for (int i = 0; i < num_threads; ++i) {
		threads.emplace_back(searchForKey, (uint8_t*)hamadUTF16Encrypted, decodedSize, &found_keys[i], i);
	}

	for (auto& thread : threads) {
		thread.join();
	}

	for (int i = 0; i < num_threads; ++i) {
		if (found_keys[i] != MAX_KEY) {
			std::lock_guard<std::mutex> lock(cout_mutex);
			std::cout << "Found the key = " << found_keys[i] << std::endl;
		}
	}
	printf("Complete...\n");
	return 0;
}
