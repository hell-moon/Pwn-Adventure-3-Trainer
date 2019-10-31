#pragma once

/*why namespace? We don't need anything persistent, don't need to pass objects w/ member variables*/ 
namespace mem
{
	void PatchEx(BYTE* dst, BYTE* src, unsigned int size, HANDLE hProcess);
	void NopEx(BYTE* dst, unsigned int size, HANDLE hProcess);
}
