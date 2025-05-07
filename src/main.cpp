#include <application.h>

int main()
{

	Application voxelGame;

	if (!voxelGame.initialize()) {
		return -1;
	}
	voxelGame.run();
	voxelGame.shutdown();

	return 0;
}