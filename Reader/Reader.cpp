#include "Reader.h"

#ifdef __linux__ 
using boost::interprocess::shared_memory_object;
#elif _WIN32
using boost::interprocess::windows_shared_memory;
#endif

using boost::interprocess::named_sharable_mutex;
using boost::interprocess::mapped_region;
using boost::interprocess::sharable_lock;

int main()
{
	// Open already created shared memory object.
#ifdef __linux__ 
	shared_memory_object shm_obj(boost::interprocess::open_only,
		"shared_memory",
		boost::interprocess::read_only);

#elif _WIN32
	windows_shared_memory shm_obj(boost::interprocess::open_only,
		"shared_memory",
		boost::interprocess::read_only);
#endif

	// Open already created named mutex
	named_sharable_mutex mutex(boost::interprocess::open_only, "PandoraBox");

	//Map the whole shared memory in this process
	mapped_region region(shm_obj, boost::interprocess::read_only);

	while (true) {
		{
			// Lock the mutex, read-only mode
			sharable_lock<named_sharable_mutex> lock(mutex);
			auto data = Gintama::GetYorozuya(region.get_address());

			fmt::print("Index: {}\n", data->index());

			auto members = data->member();

			for (uint32_t i = 0; i < members->size(); i++) {
				auto mem = members->Get(i);
				fmt::print("Name: {}\n", mem->name()->c_str());
				fmt::print("Quote: {}\n", mem->quote()->c_str());
			}
			fmt::print("-------------------\n");
		}
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
}
