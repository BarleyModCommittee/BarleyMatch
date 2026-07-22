import index;

import <fstream>;
using std::ifstream;
using std::string;
using nlohmann::json;

wchar_t dir[1000];

uint8_t code[] =
{
	0x68, 0, 0, 0, 0,
	0xFF, 0x15, 0xA4, 0x20, 0x65, 0,

	0x85, 0xC0,
	0x75, 9,

	0x6A, 0xFF,
	0xB8, 0x92, 0xDD, 0x61, 0,
	0xFF, 0xD0
};

char dll_pos[] = "../MatchCore.dll";

int main(int argc, char* argv[])
{
	std::ifstream config{"file.json"};
	json j;
	config >> j;

	for (string str : j.at("venv"))
	{
		::MultiByteToWideChar(0, 0, str.c_str(), -1, dir, str.size());
		uint32_t pid = ProcessOpener::OpenByFilePath(dir, L"PlantsVsZombies.exe");

		PVZ::InitPVZNoLock(pid);
		std::printf("%d\n", PVZ::Memory::Variable);
		PVZ::Memory::immediateExecute = true;

		auto pos = (int*)(code + 1);
		*pos = PVZ::Memory::Variable;

		PVZ::Memory::WriteArray(PVZ::Memory::Variable, dll_pos, sizeof(dll_pos));

		Injector inj(0x5513B5, 6, code, sizeof(code));

		PVZ::QuitPVZ();
	}
	std::system("pause");
	return 0;
}