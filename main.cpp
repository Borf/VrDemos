#include <cstdlib>
#include "JohanDemo.h"

// --- Lib Stuff --- //
#include <VrLib/Kernel.h>



int main(int argc, char* argv[])
{
	vrlib::Kernel* kernel = vrlib::Kernel::getInstance();  // Get the kernel
	JohanDemo* application = new JohanDemo();             // Instantiate an instance of the app



	// Load any config files specified on the command line
	for( int i = 1; i < argc; ++i )
	{
		if(strcmp(argv[i], "--config") == 0)
		{
			i++;
			kernel->loadConfig(argv[i]); 
		}
	}

	kernel->setApp(application);
	kernel->start();
	return 0;
}
