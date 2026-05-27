import index;

import <fstream>;
using std::ifstream;
using std::string;
using nlohmann::json;

wchar_t dir[1000];

int main(int argc, char* argv[])
{
	std::ifstream config{"file.json"};
	json j;
	config >> j;

	uint32_t velocity = j.at("acceleration_factor");
	bool draw_board = j.at("draw_board");

	for (string str : j.at("venv"))
	{
		::MultiByteToWideChar(0, 0, str.c_str(), -1, dir, str.size());
		uint32_t pid = ProcessOpener::OpenByFilePath(dir, L"BarleyMod.exe");

		PVZ::InitPVZNoLock(pid);
		std::printf("%d\n", PVZ::Memory::Variable);
		PVZ::Memory::immediateExecute = true;
		PVZ::Memory::WriteMemory<uint32_t>(0x4526D3, velocity);
		if (velocity > 1)
			PVZ::Memory::WriteMemory<uint8_t>(0x6A9EAB, 1);

		if (!draw_board)
			DisableBoardDraw();

		PVZ::Memory::InjectDll("../AutoTest-Core.dll");
		PVZ::QuitPVZ();
	}
	std::system("pause");
	return 0;
}