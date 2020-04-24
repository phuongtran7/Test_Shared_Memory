#include "Writer.h"

#ifdef __linux__ 
using boost::interprocess::shared_memory_object;
#elif _WIN32
using boost::interprocess::windows_shared_memory;
#endif

using boost::interprocess::named_sharable_mutex;
using boost::interprocess::mapped_region;
using boost::interprocess::scoped_lock;

void create_data(flatbuffers::FlatBufferBuilder& builder, unsigned int count) {

	std::vector<flatbuffers::Offset<Gintama::Character>> member_vector;

	auto gin = Gintama::CreateCharacterDirect(builder, "Sakata Gintoki", "Idiots, in their own idiotic way, have their own idiotic worries.");
	member_vector.push_back(gin);

	auto shinpachi = Gintama::CreateCharacterDirect(builder, "Shimura Shinpachi", "This country is done for.");
	member_vector.push_back(shinpachi);

	auto kagura = Gintama::CreateCharacterDirect(builder, "Kagura", "HAAARLEEEY DAVIDSOOOON!!!");
	member_vector.push_back(kagura);

	auto forever_yorozuya = Gintama::CreateYorozuyaDirect(builder, count, &member_vector);

	builder.Finish(forever_yorozuya);

	fmt::print("Serialized size: {}\n", builder.GetSize());
}

int main(int argc, char** argv)
{
	flatbuffers::FlatBufferBuilder builder;

	// Create a shared memory object.
#ifdef __linux__ 
	shared_memory_object shm_obj(boost::interprocess::open_or_create,
		"shared_memory",
		boost::interprocess::read_write);

	//Set size
	shm_obj.truncate(1000);

#elif _WIN32
	windows_shared_memory shm_obj(boost::interprocess::open_or_create,
		"shared_memory",
		boost::interprocess::read_write,
		1000
	);
#endif

	// Open or create the named mutex
	named_sharable_mutex mutex(boost::interprocess::open_or_create, "PandoraBox");

	//Map the whole shared memory in this process
	mapped_region region(shm_obj, boost::interprocess::read_write);

	unsigned int count{ 0 };

	while (true) {
		// Introduce new scope so that the mutex is released before sleeping
		{
			create_data(builder, count);

			// Lock the mutex, exclusive lock
			scoped_lock<named_sharable_mutex> lock(mutex);

			// Write the generated flatbuffers to region
			std::memcpy(region.get_address(), builder.GetBufferPointer(), builder.GetSize());
		}

		builder.Clear();
		count++;

		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	}
}
